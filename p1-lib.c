#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include "logmsg.h"

#include "crc16.h"

#include "p1-lib.h"


uint16_t crc_telegram (const uint8_t *data, unsigned int length)
{
	// Calculate the CRC16 of a telegram, for verification
	
	if (data[length - 3] == '!') {
		
		// Old-style telegrams end with "!\r\n" and do not contain a CRC, so there's no point in checking it
		
		return 0;
		
	} else if (data[length - 7] == '!') {
		
		// Calculate CRC16 from start of telegram until '!' (inclusive)
		// Length is full telegram length minus 2 bytes CR + LF minus 4 bytes hex-encoded CRC16
		
		return crc16(data, length - 6);
	}
	
	// Invalid telegram
	
	return 0;
}


size_t read_telegram (int fd, uint8_t *buf, size_t bufsize, size_t maxfailbytes)
{
	// Try to read a full P1-telegram from a file-handle and store it in a buffer
	
	int telegram = 0;
	uint8_t byte;
	size_t offset = 0, failed = 0;
	ssize_t len;
	
	do {
		len = read(fd, &byte, 1);
		if (len > 0) {
			if (!telegram && byte == '/') {
				// Possible start of telegram
				logmsg(LL_VERBOSE, "Possible telegram found at offset %lu\n", offset);
				telegram = 1;
				buf[offset++] = byte;
			} else if (telegram && offset < bufsize) {
				// Possible telegram content
				buf[offset++] = byte;
				if (byte == '!') {
					// Possible end of telegram, try to read either cr + lf or CRC value
					logmsg(LL_VERBOSE, "Possible telegram end at offset %lu\n", offset);
					len = read(fd, buf + offset, 1);
					len += read(fd, buf + offset + 1, 1);
					if (len == 2) {
						if (buf[offset] == '\r') {
							// Old-style telegram without CRC
							logmsg(LL_VERBOSE, "Old-style telegram with length %lu\n", offset + len);
							return offset + len;
						} else {
							// Possible start of CRC, try reading 4 more bytes
							offset += len;
							len = read(fd, buf + offset, 1);			// Call read 4 times, because otherwise it can return with <4 bytes
							len += read(fd, buf + offset + 1, 1);
							len += read(fd, buf + offset + 2, 1);
							len += read(fd, buf + offset + 3, 1);
							if (len == 4 && buf[offset + 2] == '\r') {
								// New style telegram with CRC
								logmsg(LL_VERBOSE, "New-style telegram with length %lu\n", offset + len);
								return offset + len;
							}							
						}
					}
					// If we reach this point, we haven't found a valid telegram, try again
					logmsg(LL_VERBOSE, "Invalid telegram, restart scanning\n");
					failed += offset + len;
					offset = 0;
					telegram = 0;
				}
			} else if (offset >= bufsize) {
				// Buffer overflow before telegram end, restart search for telegrams
				logmsg(LL_VERBOSE, "Buffer overflow before valid telegram end, restart scanning\n");
				failed += offset;
				offset = 0;
				telegram = 0;
			}
		}
	} while (len > 0 && (maxfailbytes == 0 || failed < maxfailbytes));
	
	// Return zero if we get a read error, or if we've read the maximum number of non-valid bytes
	
	return 0;
}


int telegram_parser_open (telegram_parser *obj, char *infile, size_t bufsize, int timeout, char *dumpfile)
{
	if (obj == NULL) {
		return -1;
	}
	
	parser_init(&(obj->parser));	// Initialise Ragel state machine
	
	obj->data = &(obj->parser.data);
	obj->status = 0;
	
	obj->buffer = NULL;
	obj->bufsize = 0;
	obj->len = 0;
	
	obj->fd = -1;
	obj->terminal = 0;
	
	if (timeout <= 0) {
		timeout = READ_TIMEOUT;		// In seconds
	}
	
	obj->timeout = timeout;

	if (infile) {
		obj->fd = open(infile, O_RDONLY | O_NOCTTY);	// If we open a serial device, make sure it doesn't become the controlling TTY
		
		if (obj->fd < 0) {
			logmsg(LL_ERROR, "Could not open input file/device %s: %s\n", infile, strerror(errno));
			return -2;
		}
		
		if (tcgetattr(obj->fd, &(obj->oldtio)) == 0) {
			
			logmsg(LL_VERBOSE, "Input device seems to be a serial terminal\n");
			
			obj->terminal = 1;					// If we can get terminal attributes, assume we're reading from a serial device
			
			memset(&(obj->newtio), 0, sizeof(struct termios));		/* Clear the new terminal data structure */
		
			obj->newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;	// Start at 115200 baud, 8-bit characters, ignore control lines, enable reading
			obj->newtio.c_iflag = 0;
			obj->newtio.c_oflag = 0;	
			obj->newtio.c_lflag = 0;						// Set input mode (non-canonical, no echo, etc.)
			obj->newtio.c_cc[VTIME] = (timeout * 10);  		// Inter-character timer or timeout in 0.1s (0 = unused)
			obj->newtio.c_cc[VMIN]  = 0;   					// Blocking read until 1 char received or timeout
			
			tcflush(obj->fd, TCIFLUSH);						// Flush any data still left in the input buffer, to avoid confusing the parsers
			tcsetattr(obj->fd, TCSANOW, &(obj->newtio));	// Set new terminal attributes
		}
	}
	
	if (dumpfile) {
		// TODO: use a file descriptor rather than a stdio pointer
		obj->dumpfile = fopen(dumpfile, "a");
		if (obj->dumpfile == NULL) {
			logmsg(LL_ERROR, "Could not open output file %s\n", dumpfile);
			return -3;
		}
	} else {
		obj->dumpfile = NULL;
	}
	
	if (bufsize == 0) {
		bufsize = PARSER_BUFLEN;
	}
	
	obj->buffer = malloc(bufsize);
	if (obj->buffer) {
		obj->bufsize = bufsize;
	} else {
		logmsg(LL_ERROR, "Could not allocate %lu byte telegram buffer\n", (unsigned long)bufsize);
		return -4;
	}
		
	return 0;	
}


void telegram_parser_close (telegram_parser *obj)
{
	if (obj == NULL) {
		return;
	}
	
	if (obj->bufsize && obj->buffer) {
		free(obj->buffer);
		obj->buffer = NULL;
		obj->bufsize = 0;
		obj->len = 0;
	}
	
	if (obj->fd > 0) {
		if (obj->terminal) {
			tcsetattr(obj->fd, TCSANOW, &(obj->oldtio));	// Restore old port settings
		}		
		close(obj->fd);
		obj->fd = -1;
		obj->terminal = 0;
	}
	
	if (obj->dumpfile) {
		fclose(obj->dumpfile);
		obj->dumpfile = NULL;
	}
}


int telegram_parser_read (telegram_parser *obj)
{
	if (obj == NULL) {
		return -1;
	}
	
	if (obj->buffer == NULL || obj->bufsize == 0) {
		return -2;
	}
	
	if (obj->fd <= 0) {
		return -3;
	}
	
	obj->len = read_telegram(obj->fd, obj->buffer, obj->bufsize, obj->bufsize);

	if (obj->len) {
		parser_init(&(obj->parser));
		parser_execute(&(obj->parser), obj->buffer, obj->len, 1);
		obj->status = parser_finish(&(obj->parser));	// 1 if final state reached, -1 on error, 0 if final state not reached
		if (obj->status == 1) {
			uint16_t crc = crc_telegram(obj->buffer, obj->len);
			// TODO: actually report CRC error
			logmsg(LL_VERBOSE, "Parsing successful, data CRC 0x%x, telegram CRC 0x%x\n", crc, obj->parser.crc16);
		} 
		if (obj->parser.parse_errors) {
			logmsg(LL_VERBOSE, "Parse errors: %d\n", obj->parser.parse_errors);
			if (obj->dumpfile) {
				fwrite(obj->buffer, 1, obj->len, obj->dumpfile);
				fflush(obj->dumpfile);
			}
		}
	}
	
	if (obj->terminal && obj->len == 0) {
		
		// Try a different baud rate, maybe we have an old DSMR meter that runs at 9600 baud
		
		speed_t baudrate = cfgetispeed(&(obj->newtio));
		
		if (baudrate == B115200)
			cfsetispeed(&(obj->newtio), B9600);	
		else
			cfsetispeed(&(obj->newtio), B115200);
		
		tcflush(obj->fd, TCIFLUSH);				// Flush any data still left in the input buffer, to avoid confusing the parsers
		tcsetattr(obj->fd, TCSANOW, &(obj->newtio));	// Set new terminal attributes
	}

	// TODO: report errors
	
	return 0;
}	
