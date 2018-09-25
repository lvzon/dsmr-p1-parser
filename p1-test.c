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

#include "p1-lib.h"

// Size of the buffer used to read and store telegrams, determines maximum telegram size

#define BUFSIZE_TELEGRAM 4096

// Timeout for reading from serial devices, in seconds

#define READ_TIMEOUT 15

// Buffer for storing telegram data

char buf_telegram[BUFSIZE_TELEGRAM];



int main (int argc, char **argv)
{
	
	init_msglogger();
	logger.loglevel = LL_VERBOSE;
	
	char *infile;
	
	if (argc < 2) {
		logmsg(LL_NORMAL, "Usage: %s <input file or device> [<output for telegrams with parse errors>]\n", argv[0]);
		exit(1);
	}
	
	infile = argv[1];
	
	FILE *dumpfile = NULL;
	
	if (argc >= 3)
		dumpfile = fopen(argv[2], "a");
	
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
	
		newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		// Start at 115200 baud, 8-bit characters, ignore control lines, enable reading
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
	
	do {
	
		while (len = read_telegram(fd, buf_telegram, BUFSIZE_TELEGRAM, BUFSIZE_TELEGRAM)) {
			parser_init(&parser);
			parser_execute(&parser, buf_telegram, len, 1);
			status = parser_finish( &parser );	// 1 if final state reached, -1 on error, 0 if final state not reached
			if (status == 1) {
				uint16_t crc = crc_telegram(buf_telegram, len);
				logmsg(LL_VERBOSE, "Parsing successful, data CRC 0x%x, telegram CRC 0x%x\n", crc, parser.crc16);
			} 
			if (parser.parse_errors) {
				logmsg(LL_VERBOSE, "Parse errors: %d\n", parser.parse_errors);
				if (dumpfile) {
					fwrite(buf_telegram, 1, len, dumpfile);
					fflush(dumpfile);
				}
			}
		}
		
		if (terminal && len == 0) {
			
			// Try a different baud rate, maybe we have an old meter that runs at 9600 baud
			
			speed_t baudrate = cfgetispeed(&newtio);
			
			if (baudrate == B115200)
				cfsetispeed(&newtio, B9600);	
			else
				cfsetispeed(&newtio, B115200);
			
			tcflush(fd, TCIFLUSH);				// Flush any data still left in the input buffer, to avoid confusing the parsers
			tcsetattr(fd, TCSANOW, &newtio);	// Set new terminal attributes
		}
		
	} while (terminal);		// If we're connected to a serial device, keep reading, otherwise exit
	
	if (terminal) {
		tcsetattr(fd, TCSANOW, &oldtio);		// Restore old port settings
	}
	
	close(fd);
	
	if (dumpfile)
		fclose(dumpfile);
}

