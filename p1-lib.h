#include <stdlib.h>
#include <termios.h>

#include "p1-parser.h"
#include "dsmr-data.h"

uint16_t crc_telegram (const uint8_t *data, unsigned int length);
size_t read_telegram (int fd, uint8_t *buf, size_t bufsize, size_t maxfailbytes);


// Default size of the buffer used to read and store telegrams, determines maximum telegram size

#define BUFSIZE_TELEGRAM 4096

// Default timeout for reading from serial devices, in seconds

#define READ_TIMEOUT 15


typedef struct telegram_parser_struct {
	
	int fd;					// Input file descriptor
	int timeout;			// Time-out for reading serial data, in seconds
	FILE *dumpfile;			// File descriptor used to write telegrams with parsing errors
	int terminal;			// Flag to indicate whether input is a terminal or a file
	struct termios 	oldtio, 
					newtio;	// Terminal settings
	
	int status;				// Ragel parser status
	struct parser parser;	// Ragel state machine structure
	
	struct dsmr_data_struct *data;	// Smart meter data structure
	
	size_t bufsize;			// Telegram buffer size
	size_t len;				// Telegram length
	uint8_t *buffer;		// Telegram buffer pointer
	
	char mode;				// Meter mode (A, B, C, D, E for IEC, or P for DSMR P1)
	
} telegram_parser;


int telegram_parser_open (telegram_parser *obj, char *infile, size_t bufsize, int timeout, char *dumpfile);
void telegram_parser_close (telegram_parser *obj);
int telegram_parser_read (telegram_parser *obj);

int telegram_parser_open_d0 (telegram_parser *obj, char *infile, size_t bufsize, int timeout, char *dumpfile);
int telegram_parser_read_d0 (telegram_parser *obj, int wakeup);
