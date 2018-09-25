#include "logmsg.h"

#include "p1-lib.h"


int main (int argc, char **argv)
{
	
	init_msglogger();
	logger.loglevel = LL_VERBOSE;
	
	char *infile, *dumpfile;
	
	if (argc < 2) {
		logmsg(LL_NORMAL, "Usage: %s <input file or device> [<output for telegrams with parse errors>]\n", argv[0]);
		exit(1);
	}
	
	infile = argv[1];
	dumpfile = NULL;
	
	if (argc >= 3)
		dumpfile = argv[2];
	
	telegram_parser parser;
	
	telegram_parser_open(&parser, infile, 0, 0, dumpfile);
		
	do {
		
		telegram_parser_read(&parser);
		// TODO: figure out how to handle errors, time-outs, etc.
			
	} while (parser.terminal);		// If we're connected to a serial device, keep reading, otherwise exit
	
	telegram_parser_close(&parser);
	
	return 0;
}

