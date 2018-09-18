/*
   File: p1-parser.rl

   	  Ragel state-machine definition and supporting functions
   	  to parse Dutch Smart Meter P1-telegrams (and a subset of generic IEC 62056-21 smart meter telegrams).
   	  
   	  (c)2017-2018, Levien van Zon (levien at zonnetjes.net, https://github.com/lvzon)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <time.h>

#include "logmsg.h"

#include "p1-parser.h"


long long int TST_to_time (struct parser *fsm, int arg_idx) {
	
	// Get TST timestamp fields from stack and create a UNIX timestamp
	// The TST fields are: YYMMDDhhmmssX, with X = W for winter time or X = S for summer time
		
	struct tm tm;
	time_t time;
	
	tm.tm_year = fsm->arg[arg_idx] + 100;	// Years since 1900, our value was years since 2000
	tm.tm_mon = fsm->arg[arg_idx + 1] - 1;	// Months since start of year, starts at 0 (for January)
	tm.tm_mday = fsm->arg[arg_idx + 2];		// Ordinal day of the month
	tm.tm_hour = fsm->arg[arg_idx + 3];		// Hours past midnight, starts at 0
	tm.tm_min = fsm->arg[arg_idx + 4];		// Minutes past the hour
	tm.tm_sec = fsm->arg[arg_idx + 5];		// Seconds past the minute
	
	if (fsm->arg[arg_idx + 6] == 'S')		// Daylight saving time flag
		tm.tm_isdst = 1;						// Positive for daylight saving time (summer time)
	else if (fsm->arg[arg_idx + 6] == 'W')
		tm.tm_isdst = 0;						// Zero if DST is not in effect (winter time)
	else
		tm.tm_isdst = -1;						// Negative if DST information is not available
	
	logmsg(LL_DEBUG, "Time: %d %d %d %d %d %d %d\n", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_isdst);
	
	if (!fsm->meter_timezone)
		fsm->meter_timezone = METER_TIMEZONE;
	
	const char *TZ = "TZ";
	char *oldval_TZ = getenv(TZ);
	setenv(TZ, fsm->meter_timezone, 1);		// Set TZ timezone environment variable to meter timezone
		
	time = mktime(&tm);
	
	if (oldval_TZ)
		setenv(TZ, oldval_TZ, 1);			// Restore TZ timezone environment variable
	
	return time;
}


/* Ragel state-machine definition */

%%{
	machine parser;
	access fsm->;

	include parser "parser-tools.rl";

	rest_of_line := [^\r^\n]* [\r\n]{2} @clearargs @{ fgoto main; };	# Helper machine to parse the rest of the line in case of errors
		
	# Actions associated with commands
	
	action header { 
		logmsg(LL_VERBOSE, "Header: %s\n", fsm->strarg[0]); 
		strncpy((char *)(fsm->data.header), fsm->strarg[0], LEN_HEADER); 
	}
	
	action crc { 
		logmsg(LL_VERBOSE, "CRC: 0x%x\n", (unsigned int)fsm->arg[0]); 
		fsm->crc16 = fsm->arg[0]; 
	}
	
	action P1_version { 
		fsm->data.P1_version_major = fsm->arg[0] >> 4;
		fsm->data.P1_version_minor = fsm->arg[0] & 0xf;
		logmsg(LL_VERBOSE, "P1 version: %d.%d\n", (int)(fsm->data.P1_version_major), (int)(fsm->data.P1_version_minor)); 
	}
	
	action timestamp {
		fsm->data.timestamp = TST_to_time(fsm, 0);
		logmsg(LL_VERBOSE, "Timestamp: %lu\n", (unsigned long)(fsm->data.timestamp));
	}
	
	action equipment_id { 
		logmsg(LL_VERBOSE, "Equipment ID: %s\n", fsm->strarg[0]);
		strncpy((char *)(fsm->data.equipment_id), fsm->strarg[0], LEN_EQUIPMENT_ID); 
	}
	
	action tariff { 
		fsm->data.tariff = fsm->arg[0];
		logmsg(LL_VERBOSE, "Tariff: %u\n", (unsigned int)(fsm->data.tariff));
	}
	
	action switchpos { 
		fsm->data.switchpos = fsm->arg[0];
		logmsg(LL_VERBOSE, "Switch position: %d\n", (int)(fsm->data.switchpos));
	}	
	
	action E_in {
		unsigned int tariff = fsm->arg[0];
		double value = (double)fsm->arg[1] / (double)fsm->arg[2];
		if (tariff > MAX_TARIFFS) {
			logmsg(LL_ERROR, "Tariff %u out of range, max. %u, E_in %f %s\n", tariff, MAX_TARIFFS, value, fsm->strarg[0]);
		} else {
			fsm->data.E_in[tariff] = value;
			strncpy((char *)(fsm->unit_E_in[tariff]), fsm->strarg[0], LEN_UNIT + 1);
			logmsg(LL_VERBOSE, "Energy in, tariff %u: %f %s\n", value, fsm->strarg[0]); 
		}
	}
	
	action E_out {
		unsigned int tariff = fsm->arg[0];
		double value = (double)fsm->arg[1] / (double)fsm->arg[2];
		if (tariff > MAX_TARIFFS) {
			logmsg(LL_ERROR, "Tariff %u out of range, max. %u, E_out %f %s\n", tariff, MAX_TARIFFS, value, fsm->strarg[0]);
		} else {
			fsm->data.E_out[tariff] = value;
			strncpy((char *)(fsm->unit_E_out[tariff]), fsm->strarg[0], LEN_UNIT + 1);
			logmsg(LL_VERBOSE, "Energy out, tariff %u: %f %s\n", value, fsm->strarg[0]); 
		}
	}
	
	action E_in_t1 { logmsg(LL_VERBOSE, "Energy in, tariff 1: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action E_in_t2 { logmsg(LL_VERBOSE, "Energy in, tariff 2: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action E_out_t1 { logmsg(LL_VERBOSE, "Energy out, tariff 1: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action E_out_t2 { logmsg(LL_VERBOSE, "Energy out, tariff 2: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	
	action P_in { logmsg(LL_VERBOSE, "Power in: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_out { logmsg(LL_VERBOSE, "Power out: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_threshold { logmsg(LL_VERBOSE, "Power threshold: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action I_L1 { logmsg(LL_VERBOSE, "Current L1: %lld %s\n", fsm->arg[0], fsm->strarg[0]); }
	action I_L2 { logmsg(LL_VERBOSE, "Current L2: %lld %s\n", fsm->arg[0], fsm->strarg[0]); }
	action I_L3 { logmsg(LL_VERBOSE, "Current L3: %lld %s\n", fsm->arg[0], fsm->strarg[0]); }
	action I_threshold { logmsg(LL_VERBOSE, "Current threshold: %lld %s\n", fsm->arg[0], fsm->strarg[0]); }
	action V_L1 { logmsg(LL_VERBOSE, "Voltage L1: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action V_L2 { logmsg(LL_VERBOSE, "Voltage L2: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action V_L3 { logmsg(LL_VERBOSE, "Voltage L3: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_in_L1 { logmsg(LL_VERBOSE, "Power in L1: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_in_L2 { logmsg(LL_VERBOSE, "Power in L2: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_in_L3 { logmsg(LL_VERBOSE, "Power in L3: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_out_L1 { logmsg(LL_VERBOSE, "Power out L1: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_out_L2 { logmsg(LL_VERBOSE, "Power out L2: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action P_out_L3 { logmsg(LL_VERBOSE, "Power out L3: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }	
	action pfail { logmsg(LL_VERBOSE, "Power failures: %lld\n", fsm->arg[0]);}
	action longpfail { logmsg(LL_VERBOSE, "Long power failures: %lld\n", fsm->arg[0]);}
	action pfailevent { logmsg(LL_VERBOSE, "Power failure event at %lld, %lld %s\n", TST_to_time(fsm, 0), fsm->arg[7], fsm->strarg[0]);}
	action pfailevents { logmsg(LL_VERBOSE, "Power failure events: %lld\n", fsm->arg[0]);}
	action V_sags_L1 { logmsg(LL_VERBOSE, "Voltage sags L1: %lld\n", fsm->arg[0]);}
	action V_sags_L2 { logmsg(LL_VERBOSE, "Voltage sags L2: %lld\n", fsm->arg[0]);}
	action V_sags_L3 { logmsg(LL_VERBOSE, "Voltage sags L3: %lld\n", fsm->arg[0]);}
	action V_swells_L1 { logmsg(LL_VERBOSE, "Voltage swells L1: %lld\n", fsm->arg[0]);}
	action V_swells_L2 { logmsg(LL_VERBOSE, "Voltage swells L2: %lld\n", fsm->arg[0]);}
	action V_swells_L3 { logmsg(LL_VERBOSE, "Voltage swells L3: %lld\n", fsm->arg[0]);}
	action textmsgcodes { logmsg(LL_VERBOSE, "Text message codes: %s\n", fsm->strarg[0]);}
	action textmsg { logmsg(LL_VERBOSE, "Text message: %s\n", fsm->strarg[0]); }
	action dev_type { logmsg(LL_VERBOSE, "Device %lld type: %lld\n", fsm->arg[0], fsm->arg[1]);}
	action dev_id { logmsg(LL_VERBOSE, "Device %lld ID: %s\n", fsm->arg[0], fsm->strarg[0]);}
	action dev_counter { logmsg(LL_VERBOSE, "Device %lld counter at %lld: %f %s\n", fsm->arg[0], TST_to_time(fsm, 1), (double)fsm->arg[8] / (double)fsm->arg[9], fsm->strarg[0]); }
	action dev_valve { logmsg(LL_VERBOSE, "Device %lld valve position: %lld\n", fsm->arg[0], fsm->arg[1]);}	
	action dev_timeseries_head { logmsg(LL_VERBOSE, "Device %lld timeseries, starting time %lld, status %lld, period %lld, values %lld:\n", fsm->arg[0], TST_to_time(fsm, 1), fsm->arg[7], fsm->arg[8], fsm->arg[9]); }
	action dev_timeseries_counter_head { logmsg(LL_VERBOSE, "counter values, unit %s\n", fsm->strarg[0]); } 
	action dev_timeseries_counter_cold_head { logmsg(LL_VERBOSE, "cold counter values, unit %s\n", fsm->strarg[0]); } 
	action dev_timeseries_counterval { logmsg(LL_VERBOSE, "counter value: %f\n", (double)fsm->arg[0] / (double)fsm->arg[1]); }
	action gas_id_old { logmsg(LL_VERBOSE, "Gas meter ID: %s\n", fsm->strarg[0]);}
	action gas_count_old { logmsg(LL_VERBOSE, "Gas meter counter: %f %s\n", (double)fsm->arg[0] / (double)fsm->arg[1], fsm->strarg[0]); }
	action gas_valve_old { logmsg(LL_VERBOSE, "Gas meter valve position: %lld\n", fsm->arg[0]);}	
	action error {logmsg(LL_VERBOSE, "Error while parsing\n"); fsm->parse_errors++ ; fhold ; fgoto rest_of_line; } 
	
	
	action unknown { logmsg(LL_VERBOSE, "Unknown: %s\n", fsm->strarg[0]); fsm->strargc = 0 ; fsm->buflen = 0; }

	# Helpers that collect arguments
	
	digitpair = (digit @add_digit){2}  >cleararg %addarg;	# Parse and store two digits
	dst = ([SW] >cleararg @addchar);	# Parse and store daylight saving time character
	
	mbusid = ( [1234] @add_digit ) >cleararg %addarg;	# We can have 4 additional MBUS devices: gas meter, water meter, thermal meter, slave meter 


	# Definitions of statements and parts of statements

	crlf = '\r\n';	# Lines are terminated by carriage return + line feed
	#crlf = [\r\n]{2};	# Lines are terminated by carriage return + line feed, but we'll also match some converted line ends
	
	fixedpoint = fpval;			# Fixed point value, stored as an integer value and an integer divider 
	
	TST = digitpair{6} dst;
	TST_old = digitpair{6};
	unit = [* ] ([^)]+ >addstr $str_append %str_term);	# Formally only '*' is a valid unit separator, but some meters use a space (and thus put the unit in the value string)
	timeseries_unit = [^)]+ >addstr $str_append %str_term;
	
	billing_period = ('*' digit+)?;		# The billing period specifier is part of IEC 62056-21, but currently isn't present in DSMR telegrams, so we make it optional
	
	headerstr = ([^\r^\n]+ >addstr $str_append %str_term);
	#msgstr = ([^)]* >addstr $str_append %str_term);
	msgstr = hexstring;
	idstr = hexstring;
	
	header = '/' headerstr crlf crlf @header @clearargs;	
	end = '!' hexint? crlf @crc @clearargs;		# Telegram end with optional CRC
	
	fixedpointval = '(' fixedpoint unit ')';	# The value can be either integer or non-integer
	tstval = '(' TST ')';
	tstval_old = '(' TST_old ')';
	
	# COSEM-objects supported by DSMR
	
	P1_version = '1-3:0.2.8(' hexint ')' crlf @P1_version;		# P1 version
	timestamp = '0-0:1.0.0' tstval crlf @timestamp;			# Telegram timestamp
	
	equipment_id_p1 = '0-0:96.1.1' billing_period '(' idstr ')' crlf @equipment_id;	# Equipment ID in P1-meters
	equipment_id_iec = digit '-0:0.0.0' billing_period '(' uinteger ')' crlf @equipment_id;	# Equipment ID in IEC 62056-21 meters
	
	E_in = '1-0:1.8.' uinteger fixedpointval crlf @E_in;	# Electricity delivered to client
	E_out = '1-0:2.8.' uinteger fixedpointval crlf @E_out;	# Electricity delivered by client
	
	# 1-0:1.8.0*255(0091158 kWh)
	
	E_in_t1 = '1-0:1.8.1' billing_period fixedpointval crlf @E_in_t1;	# Electricity delivered to client in tariff 1
	E_in_t2 = '1-0:1.8.2' billing_period fixedpointval crlf @E_in_t2;	# Electricity delivered to client in tariff 2
	E_out_t1 = '1-0:2.8.1' billing_period fixedpointval crlf @E_out_t1;	# Electricity delivered by client in tariff 1
	E_out_t2 = '1-0:2.8.2' billing_period fixedpointval crlf @E_out_t2;	# Electricity delivered by client in tariff 2

	tariff = '0-0:96.14.0(' uinteger ')' crlf @tariff;		# TODO: can be non-integer, in theory?
	switchpos = '0-0:' ('96.3.10' | '24.4.0') '(' uinteger ')' crlf @switchpos;	# Switch position electricity (in/out/enabled), absent from DSMR>=4.0.7

	P_in = '1-0:1.7.0' fixedpointval crlf @P_in;	# Actual power delivered to client
	P_out = '1-0:2.7.0' fixedpointval crlf @P_out;	# Actual power delivered by client
	P_threshold = '0-0:17.0.0' fixedpointval crlf @P_threshold;
	
	pfail = '0-0:96.7.21(' uinteger ')' crlf @pfail; 
	longpfail = '0-0:96.7.9(' uinteger ')' crlf @longpfail; 
	
	pfailevents = '1-0:99.97.0(' uinteger ')' @pfailevents @clearargs;	# Power failure events
	pfailevent = tstval '(' uinteger unit ')' @pfailevent @clearargs;		# Single power failure event
	pfaileventlog = pfailevents '(0-0:96.7.19)'? pfailevent* crlf;	# Power failure event log, with zero or more events
	
	V_sags_L1 = '1-0:32.32.0(' uinteger ')' crlf @V_sags_L1;
	V_sags_L2 = '1-0:52.32.0(' uinteger ')' crlf @V_sags_L2;
	V_sags_L3 = '1-0:72.32.0(' uinteger ')' crlf @V_sags_L3;
	
	V_swells_L1 = '1-0:32.36.0(' uinteger ')' crlf @V_swells_L1;	
	V_swells_L2 = '1-0:52.36.0(' uinteger ')' crlf @V_swells_L2;
	V_swells_L3 = '1-0:72.36.0(' uinteger ')' crlf @V_swells_L3;
	
	textmsgcodes = '0-0:96.13.1(' msgstr ')' crlf @textmsgcodes;
	textmsgcodes_empty = '0-0:96.13.1()' crlf;
	textmsg = '0-0:96.13.0(' msgstr ')' crlf @textmsg;
	textmsg_empty = '0-0:96.13.0()' crlf;

	V_L1 = '1-0:32.7.0' fixedpointval crlf @V_L1;
	V_L2 = '1-0:52.7.0' fixedpointval crlf @V_L2;
	V_L3 = '1-0:72.7.0' fixedpointval crlf @V_L3;
	
	I_L1 = '1-0:31.7.0(' uinteger unit ')' crlf @I_L1;
	I_L2 = '1-0:51.7.0(' uinteger unit ')' crlf @I_L2;
	I_L3 = '1-0:71.7.0(' uinteger unit ')' crlf @I_L3;

	I_threshold = '0-0:17.0.0(' uinteger unit ')' crlf @I_threshold;
	
	P_in_L1 = '1-0:21.7.0' fixedpointval crlf @P_in_L1;
	P_in_L2 = '1-0:41.7.0' fixedpointval crlf @P_in_L2;
	P_in_L3 = '1-0:61.7.0' fixedpointval crlf @P_in_L3;
	
	P_out_L1 = '1-0:22.7.0' fixedpointval crlf @P_out_L1;
	P_out_L2 = '1-0:42.7.0' fixedpointval crlf @P_out_L2;
	P_out_L3 = '1-0:62.7.0' fixedpointval crlf @P_out_L3;
	
	dev_type = '0-' mbusid ':24.1.0(' uinteger ')' crlf @dev_type;
	dev_id = '0-' mbusid ':96.1.0(' idstr ')' crlf @dev_id;
	dev_counter = '0-' mbusid ':24.2.1' tstval fixedpointval crlf @dev_counter;	
	dev_valve = '0-' mbusid ':24.4.0(' uinteger ')' crlf @dev_valve;	# Valve position (on/off/released), absent from DSMR>=4.0.7
	
	# This describes the rather horrible "profile generic dataset" representation used in DSMR 3.x	
	dev_timeseries_head = '0-' mbusid ':24.3.0' tstval_old '(' uinteger ')' '(' uinteger ')' '(' uinteger ')' @dev_timeseries_head @clearargs;
	dev_timeseries_counter_head = '(0-' digit+ ':24.2.1)(' timeseries_unit ')' @dev_timeseries_counter_head @clearargs;
	dev_timeseries_counter_cold_head = '(0-' digit+ ':24.3.1)(' timeseries_unit ')'  @dev_timeseries_counter_cold_head @clearargs;
	dev_timeseries_counterval = '(' fixedpoint ')' @dev_timeseries_counterval @clearargs;
	dev_counter_timeseries = dev_timeseries_head dev_timeseries_counter_head dev_timeseries_counterval+ crlf;
	dev_counter_cold_timeseries = dev_timeseries_head dev_timeseries_counter_cold_head dev_timeseries_counterval+ crlf;
	
	gas_id_old = '7-0:0.0.0(' idstr ')' crlf @gas_id_old;
	gas_count_old = '7-0:23.1.0' tstval fixedpointval crlf @gas_count_old;
	#gas_count_Tcomp_old = '7-0:23.2.0' tstval fixedpointval crlf @gas_count_Tcomp_old;
	#gas_valve_old = '7-0:24.4.0(' uinteger ')' crlf @gas_valve_old;	
	#heat_id_old = '5-0:0.0.0(' idstr ')' crlf @heat_id_old;
	#cold_id_old = '6-0:0.0.0(' idstr ')' crlf @cold_id_old;
	#water_id_old = '8-0:0.0.0(' idstr ')' crlf @water_id_old;
	#heat_count_old = '5-0:1.0.0' tstval fixedpointval crlf @heat_count_old;
	#cold_count_old = '6-0:1.0.0' tstval fixedpointval crlf @cold_count_old;
	#water_count_old = '8-0:1.0.0' tstval fixedpointval crlf @water_count_old;
	
	# P1 Telegram components
	
	equipment_id = equipment_id_p1 | equipment_id_iec;
	metadata_object = P1_version | timestamp;
	emeter_object = equipment_id | tariff | switchpos | E_in_t1 | E_in_t2 | E_out_t1 | E_out_t2;
	power_object = P_in | P_out | P_in_L1 | P_out_L1 | P_in_L2 | P_out_L2 | P_in_L3 | P_out_L3 | P_threshold;
	current_object = I_L1 | I_L2 | I_L3 | I_threshold;
	voltage_object = V_L1 | V_L2 | V_L3;
	power_quality_object = pfail | longpfail | pfaileventlog | V_sags_L1 | V_swells_L1 | V_sags_L2 | V_swells_L2 | V_sags_L3 | V_swells_L3;
	mbusdev_object = dev_type | dev_id | dev_counter | dev_valve | dev_counter_timeseries | dev_counter_cold_timeseries;
	slavedev_legacy_object = gas_id_old | gas_count_old;
	message_object = textmsgcodes | textmsg | textmsgcodes_empty | textmsg_empty;
	
	object = 	metadata_object | emeter_object | power_object | current_object | voltage_object | power_quality_object |
				message_object | mbusdev_object | slavedev_legacy_object;
	
	line = object $err(error) @clearargs;	# Clear argument stacks at the end of each line, handle parsing errors
	
	telegram = header? line* end;	# Make header optional, so we can recover from errors in the middle of a telegram
	
	main := telegram* $err(error);		# Parse zero or more telegrams
	
}%%

%% write data;


void parser_init( struct parser *fsm )
{
	int arg;
	
	fsm->buflen = 0;
	fsm->argc = 0;
	for (arg = 0 ; arg < PARSER_MAXARGS ; arg++)
		fsm->arg[arg] = 0;
	fsm->multiplier = 1;
	fsm->bitcount = 0;
	fsm->strargc = 0;
	for (arg = 0 ; arg < PARSER_MAXARGS ; arg++)
		fsm->strarg[arg] = NULL;
	fsm->parse_errors = 0;
	
	%% write init;
}

void parser_execute(struct parser *fsm, const char *data, int len, int eofflag)
{
	const char *p = data;
	const char *pe = data + len;
	const char *eof = 0;

	if (eofflag)
		eof = pe;
	
	%% write exec;
	
	fsm->pe = pe;
}

int parser_finish(struct parser *fsm)
{
	if ( fsm->cs == parser_error )			// Machine failed before matching
		return -1;
	if ( fsm->cs >= parser_first_final )	// Final state reached
		return 1;
	return 0;								// Final state not reached
}

