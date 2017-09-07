#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include "logmsg.h"

#include "crc16.h"

#include "p1-parser.h"

// Size of the buffer used to read and store telegrams, determines maximum telegram size

#define BUFSIZE_TELEGRAM 4096

// Timeout for reading from serial devices, in seconds

#define READ_TIMEOUT 15

char buf_telegram[BUFSIZE_TELEGRAM];


uint16_t crc_telegram (const uint8_t *data, unsigned int length)
{
	
	
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
					len = read(fd, buf + offset, 2);
					if (len == 2) {
						if (buf[offset] == '\r') {
							// Old-style telegram without CRC
							logmsg(LL_VERBOSE, "Old-style telegram with length %lu\n", offset + len);
							return offset + len;
						} else {
							// Possible start of CRC, try reading 4 more bytes
							offset += len;
							len = read(fd, buf + offset, 4);
							if (buf[offset + 2] == '\r') {
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


int main (int argc, char **argv)
{
	init_msglogger();
	logger.loglevel = LL_VERBOSE;
	
	char *infile;
	
	if (argc < 2) {
		logmsg(LL_NORMAL, "Usage: %s <input file or device>\n", argv[0]);
		exit(1);
	}
	
	infile = argv[1];
	
	struct parser parser;
	parser_init(&parser);
	
	int fd = open(argv[1], O_RDONLY | O_NOCTTY);	// If we open a serial device, make sure it doesn't become the controlling TTY
	
	if (fd < 0) {
		logmsg(LL_ERROR, "Could not open input file/device %s: %s\n", infile, strerror(errno));
		exit(1);
	}
	
	int terminal = 0;
	struct termios oldtio, newtio;
	
	if (tcgetattr(fd, &oldtio) == 0) {
		
		logmsg(LL_VERBOSE, "Input device seems to be a serial terminal\n");
		
		terminal = 1;					// If we can get terminal attributes, assume we're reading from a serial device
		
		memset(&newtio, 0, sizeof(struct termios));		/* Clear the new terminal data structure */
	
		newtio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;		// Start at 19200 baud, 8-bit characters, ignore control lines, enable reading
		newtio.c_iflag = 0;
		newtio.c_oflag = 0;	
		newtio.c_lflag = 0;								// Set input mode (non-canonical, no echo, etc.)
		newtio.c_cc[VTIME] = (READ_TIMEOUT * 10);  		// Inter-character timer or timeout in 0.1s (0 = unused)
		newtio.c_cc[VMIN]  = 0;   						// Blocking read until 1 char received or timeout
		
		tcflush(fd, TCIFLUSH);				// Flush any data still left in the input buffer, to avoid confusing the parsers
		tcsetattr(fd, TCSANOW, &newtio);	// Set new terminal attributes
	}
	
	
	size_t len;
	int status = 0;
	
	while (len = read_telegram(fd, buf_telegram, BUFSIZE_TELEGRAM, 0)) {
		parser_init(&parser);
		parser_execute(&parser, buf_telegram, len, 1);
		status = parser_finish( &parser );	// 1 if final state reached, -1 on error, 0 if final state not reached
		if (status == 1) {
			uint16_t crc = crc_telegram(buf_telegram, len);
			logmsg(LL_VERBOSE, "Parsing successful, data CRC 0x%x, telegram CRC 0x%x\n", crc, parser.crc16);
		}
	}
	
	if (terminal) {
		tcsetattr(fd, TCSANOW, &oldtio);		// Restore old port settings
	}
	
	close(fd);

}

