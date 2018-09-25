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


