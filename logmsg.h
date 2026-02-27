/*
   Header: logmsg.h

   	  Macros, functions and structures used for writing messages to a logfile.
   	  
   	  (c)2013-2015, Levien van Zon (levien@zonnetjes.net)
*/


#include <stdio.h>


/* Logging macros */

#define LL_FATAL		1
#define LL_ERROR		2
#define LL_WARNING		3
#define LL_NORMAL		4
#define LL_VERBOSE		5
#define LL_DEBUG		6

typedef struct msglogger_struct {
	
	char *logfile_name;
	FILE *logfile;
	int loglevel;
	
} messagelogger;

static messagelogger logger;

static void init_msglogger() {
		logger.logfile_name = NULL;
		logger.logfile = stdout;
		logger.loglevel = LL_VERBOSE;
}


#define logmsg(level, format, args...) { \
	if (level <= logger.loglevel && logger.logfile) { \
		if (level == LL_WARNING) \
			fprintf(logger.logfile, "WARNING: "); \
		else if (level == LL_ERROR) \
			fprintf(logger.logfile, "ERROR: "); \
		else if (level == LL_FATAL) \
			fprintf(logger.logfile, "FATAL ERROR: "); \
		fprintf(logger.logfile, format, ##args); \
		fflush(logger.logfile); \
	} \
}


