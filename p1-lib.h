#include "p1-parser.h"

uint16_t crc_telegram (const uint8_t *data, unsigned int length);
size_t read_telegram (int fd, uint8_t *buf, size_t bufsize, size_t maxfailbytes);


typedef struct telegram_parser_struct {
	
	int fd;					// Input file descriptor
	int dumpfile;			// File descriptor used to write telegrams with parsing errors
	int terminal;			// Flag to indicate whether input is a terminal or a file
	struct termios 	oldtio, 
					newtio;	// Terminal settings
	
	int status;				// Ragel parser status
	struct parser parser;	// Ragel state machine structure
	
	size_t bufsize;			// Telegram buffer size
	size_t len;				// Telegram length
	char *buffer;			// Telegram buffer pointer
	
} telegram_parser;