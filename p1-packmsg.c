#include "logmsg.h"

#include "p1-lib.h"

// For TCP/IP functions
#include "net_ip.h"

// For Messagepack:
#include "mpack.h"

// For file output
#include <stdio.h>

// Messagepack buffer

#define BUFSIZE 10240

char buffer[BUFSIZE];

// Number of phases

int phases = 1;

// Global counter for last gas meter value

double last_gas_count = 0;
	
// Global server address and socket file descriptor

char *server = NULL, *port = NULL;
int sockfd = 0;


int connect_server () {
	
	if (sockfd > 0)
		close(sockfd);
	
	if (server && port) {
        sockfd = socket_connect_tcp(server, port);
    }
    
    return sockfd;
}


int send_data (char *data, size_t size) {
	
	if (data == NULL)
		return -1;
	
	if (sockfd <= 0)
		return -2;
	
    int len = size;
    int result = sendall(sockfd, data, &len);
	
    return result;
}


int send_header (struct dsmr_data_struct *data, FILE *out) {
	
	if (buffer == NULL || data == NULL)
		return -1;
	
	// Initialise messagepack buffer
	
	char *mpackdata;
	
	mpackdata = buffer;
	
	size_t size = BUFSIZE;
	mpack_writer_t writer;

	mpack_writer_init(&writer, mpackdata, BUFSIZE);
	
	// Write PMSG header
	
	mpack_start_array(&writer, 1);
	mpack_write_cstr(&writer, "PMSG");
	mpack_finish_array(&writer);
	
	// Write dataset name
	
	mpack_start_array(&writer, 2);
	mpack_write_cstr(&writer, "SET");
	mpack_write_cstr(&writer, "DSMR_P1");
	mpack_finish_array(&writer);

	// TODO: determine actual number of devices
	
	// Start device map
	mpack_start_array(&writer, 2);
	mpack_write_cstr(&writer, "DEVS");
	mpack_start_map(&writer, 2);
	
	// Device 1 is the electricity meter
	mpack_write_u8(&writer, 1);
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "id");
	mpack_write_cstr(&writer, data->equipment_id);
	mpack_finish_map(&writer);
	
	// Device 2 is the gas meter
	mpack_write_u8(&writer, 2);
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "id");
	mpack_write_cstr(&writer, data->dev_id[0]);
	mpack_finish_map(&writer);
	
	// End device map
	mpack_finish_map(&writer);
	mpack_finish_array(&writer);
	
	// Write variable descriptions

	// Electricity imported
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "VAR");
	mpack_write_cstr(&writer, "E_in");
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "unit");
	mpack_write_cstr(&writer, "Wh");
	mpack_finish_map(&writer);
	mpack_finish_array(&writer);
	
	// Electricity exported
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "VAR");
	mpack_write_cstr(&writer, "E_out");
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "unit");
	mpack_write_cstr(&writer, "Wh");
	mpack_finish_map(&writer);
	mpack_finish_array(&writer);
	
	// Net power on phase 1
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "VAR");
	mpack_write_cstr(&writer, "P_L1");
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "unit");
	mpack_write_cstr(&writer, "W");
	mpack_finish_map(&writer);
	mpack_finish_array(&writer);

	if (phases > 1) {
		
		// Net power on phase 2
		mpack_start_array(&writer, 3);
		mpack_write_cstr(&writer, "VAR");
		mpack_write_cstr(&writer, "P_L2");
		mpack_start_map(&writer, 1);
		mpack_write_cstr(&writer, "unit");
		mpack_write_cstr(&writer, "W");
		mpack_finish_map(&writer);
		mpack_finish_array(&writer);
		
		// Net power on phase 3
		mpack_start_array(&writer, 3);
		mpack_write_cstr(&writer, "VAR");
		mpack_write_cstr(&writer, "P_L3");
		mpack_start_map(&writer, 1);
		mpack_write_cstr(&writer, "unit");
		mpack_write_cstr(&writer, "W");
		mpack_finish_map(&writer);
		mpack_finish_array(&writer);
	}
	
	// Gas imported
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "VAR");
	mpack_write_cstr(&writer, "gas_in");
	mpack_start_map(&writer, 1);
	mpack_write_cstr(&writer, "unit");
	mpack_write_cstr(&writer, "m^3");
	mpack_finish_map(&writer);
	mpack_finish_array(&writer);
	
	// Specify variables for device 1
	
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "DVARS");
	mpack_write_u8(&writer, 1);
	if (phases == 3) {
		mpack_start_array(&writer, 5);
	} else {
		mpack_start_array(&writer, 3);
	}
	mpack_write_cstr(&writer, "E_in");
	mpack_write_cstr(&writer, "E_out");
	mpack_write_cstr(&writer, "P_L1");
	if (phases == 3) {
		mpack_write_cstr(&writer, "P_L2");
		mpack_write_cstr(&writer, "P_L3");
	}
	mpack_finish_array(&writer);
	mpack_finish_array(&writer);
	
	// Specify variables for device 2
	
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "DVARS");
	mpack_write_u8(&writer, 2);
	mpack_start_array(&writer, 1);
	mpack_write_cstr(&writer, "gas_in");
	mpack_finish_array(&writer);
	mpack_finish_array(&writer);
	
	// Write data to output stream	
	// We can either send the data manually after destroying the writer,
	// or define a flush-function with mpack_writer_set_flush()  
	
	if (mpack_writer_destroy(&writer) != mpack_ok) {
		fprintf(stderr, "An error occurred encoding the data!\n");
		exit(1);
	} else {
		size = mpack_writer_buffer_used(&writer);
		printf("Wrote %lu bytes of total msgpack data\n", size);
	}
	

	// Dump header to file
    
    if (out) {
	    fwrite(mpackdata, size, 1, out);
	    fflush(out);
    }

    int result = -1;
    
    if (server && port) {
    	
    	do {
    		// Open server socket    		
    		sockfd = connect_server();
    		
    		if (sockfd > 0) {
    			
    			// Send header message to server    		
    			result = send_data(mpackdata, size);
    			
    			if (result < 0)
    				sleep(1);
    			
    		} else {
    			
    			sleep(1);
    		}
    		
    	} while (sockfd <= 0 || result < 0);
    }
	
    return result;
}


int send_values (struct dsmr_data_struct *data, FILE *out) {
	
	if (buffer == NULL || data == NULL)
		return -1;
	
	// Initialise messagepack buffer
	
	char *mpackdata;
	
	mpackdata = buffer;
	
	size_t size = BUFSIZE;
	mpack_writer_t writer;

	int result = -1;
	
	mpack_writer_init(&writer, mpackdata, BUFSIZE);
	
	// Write variable values for device 1
	
	mpack_start_array(&writer, 4);
	mpack_write_cstr(&writer, "DVALS");
	mpack_write_u8(&writer, 1);
	mpack_write_u32(&writer, data->timestamp);
	if (phases == 3) {
		mpack_start_array(&writer, 5);
	} else {
		mpack_start_array(&writer, 3);
	}
	
	// TODO: write 64-bit integers of total Wh-energy counters, without casting from double
	// Also, correctly handle units, rather than assuming hard-coded units
	
	mpack_write_u32(&writer, (data->E_in[1] + data->E_in[2]) * 1000);
	mpack_write_u32(&writer, (data->E_out[1] + data->E_out[2]) * 1000);
	mpack_write_i16(&writer, (data->P_in[0] - data->P_out[0]) * 1000);
	if (phases == 3) {
		mpack_write_i16(&writer, (data->P_in[1] - data->P_out[1]) * 1000);
		mpack_write_i16(&writer, (data->P_in[2] - data->P_out[2]) * 1000);
	}
	
	mpack_finish_array(&writer);
	mpack_finish_array(&writer);
	
	if (last_gas_count != data->dev_counter[0]) {
		
		// Write variable values for device 2
		
		mpack_start_array(&writer, 4);
		mpack_write_cstr(&writer, "DVALS");
		mpack_write_u8(&writer, 2);
		mpack_write_u32(&writer, data->dev_counter_timestamp[0]);
		mpack_start_array(&writer, 1);			
		mpack_write_float(&writer, data->dev_counter[0]);
		mpack_finish_array(&writer);
		mpack_finish_array(&writer);			
		
		last_gas_count = data->dev_counter[0];
	}
			
	// Write data to output stream	
	// We can either send the data manually after destroying the writer,
	// or define a flush-function with mpack_writer_set_flush()  
	
	if (mpack_writer_destroy(&writer) != mpack_ok) {
		fprintf(stderr, "An error occurred encoding the data!\n");
		return -2;
	} else {
		size = mpack_writer_buffer_used(&writer);
		printf("Wrote %lu bytes of total msgpack data to buffer\n", size);
	}
	
	// Dump messages to file
	
	if (out) {
		fwrite(mpackdata, size, 1, out);
		fflush(out);
	}

	if (server && port) {
		
		// Send messages to server
		
		if (sockfd <= 0) {
			
			// Socket seems to be closed, try reopening connection
			send_header(data, out);
			return -3;
			
		} else if (sockfd > 0) {
			
			result = send_data(mpackdata, size);
			
			if (result < 0) {
				
				// Sending failed, try reconnecting
				send_header(data, out);
				return -4;
				
				// TODO: use separate buffers for header and values, resend values
			}
		}
	}
	
	return 0;
}	


int main (int argc, char **argv)
{
	
	init_msglogger();
	logger.loglevel = LL_VERBOSE;
	
	char *infile, *outfile = NULL;
	
	if (argc < 4 && argc != 3) {
		logmsg(LL_NORMAL, "Usage: %s <input file or device> [<server> <port>] [<outfile>]\n", argv[0]);
		exit(1);
	}
	
	infile = argv[1];

    if (argc == 3) {
        outfile = argv[2];
    } else {
	    server = argv[2];
	    port = argv[3];
        if (argc == 5) {
            outfile = argv[4];
        }
	}


	telegram_parser parser;
	
	telegram_parser_open(&parser, infile, 0, 0, NULL);
	telegram_parser_read(&parser);

	// TODO: Exit on errors, time-outs, etc.
	
	struct dsmr_data_struct *data = parser.data;
	
	// Assume we have a three-phase meter if we see voltage or power on L2 or L3
	
	if (data->V[1] > 0 || data->V[2] > 0 || data->P_in[1] > 0 || data->P_in[2] > 0 || data->P_out[1] > 0 || data->P_out[2] > 0) {
		phases = 3;
	}
	
	// Dump messages to file if specified
    
    FILE *out = NULL;
    if (outfile) {
	    out = fopen(outfile, "w");
    }
	
	// Send packmsg header
    
	send_header(data, out);
	
	// Start main reading loop
	
	do {
		
		int result = telegram_parser_read(&parser);
		// TODO: handle errors, time-outs, etc.
		
		if (result == 0) {	// Ignore CRC-errors etc.
			send_values(data, out);
		}
		
	} while (parser.terminal);		// If we're connected to a serial device, keep reading, otherwise exit
	
    if (out) {
	    fclose(out);
    }

    if (sockfd > 0) {
        close(sockfd);
    }

	telegram_parser_close(&parser);
	
	return 0;
}

