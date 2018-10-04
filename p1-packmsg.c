#include "logmsg.h"

#include "p1-lib.h"

// For TCP/IP functions
#include "net_ip.h"

// For Messagepack:
#include "mpack.h"

// For file output
#include <stdio.h>

int main (int argc, char **argv)
{
	
	init_msglogger();
	logger.loglevel = LL_VERBOSE;
	
	char *infile, *server, *port;
	
	if (argc < 4) {
		logmsg(LL_NORMAL, "Usage: %s <input file or device> <server> <port>\n", argv[0]);
		exit(1);
	}
	
	infile = argv[1];
	server = argv[2];
	port = argv[3];
	
	telegram_parser parser;
	
	telegram_parser_open(&parser, infile, 0, 0, NULL);
	telegram_parser_read(&parser);

	// TODO: Exit on errors, time-outs, etc.
	
	struct dsmr_data_struct *data = parser.data;
	
	
	// Initialise messagepack buffer
	
	#define BUFSIZE 10240
	
	char buffer[BUFSIZE];
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
	mpack_start_array(&writer, 3);
	mpack_write_cstr(&writer, "E_in");
	mpack_write_cstr(&writer, "E_out");
	mpack_write_cstr(&writer, "P_L1");
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
	

	// Dump messages to file

	FILE *out = fopen("msgpack.dat", "w");
	fwrite(mpackdata, size, 1, out);
	fflush(out);

    // Open server socket
    // TODO check for errors

    int sockfd = socket_connect_tcp(server, port);

    // Send messages to server
    // TODO check for errors

    int len = size;
    int result = sendall(sockfd, mpackdata, &len);

	double last_gas_count = 0;
	
	do {
		
		telegram_parser_read(&parser);
		// TODO: handle errors, time-outs, etc.
		
		mpack_writer_init(&writer, mpackdata, BUFSIZE);
		
		// Write variable values for device 1
		
		mpack_start_array(&writer, 4);
		mpack_write_cstr(&writer, "DVALS");
		mpack_write_u8(&writer, 1);
		mpack_write_u32(&writer, data->timestamp);
		mpack_start_array(&writer, 3);
		
		// TODO: write 64-bit integers of total Wh-energy counters, without casting from double
		// Also, correctly handle units, rather than assuming hard-coded units
		
		mpack_write_u32(&writer, (data->E_in[0] + data->E_in[1]) * 1000);
		mpack_write_u32(&writer, (data->E_out[0] + data->E_out[1]) * 1000);
		mpack_write_i16(&writer, (data->P_in[0] - data->P_out[0]) * 1000);
		
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
			exit(1);
		} else {
			size = mpack_writer_buffer_used(&writer);
			printf("Wrote %lu bytes of total msgpack data\n", size);
		}
		
		// Dump messages to file
	
		fwrite(mpackdata, size, 1, out);
		fflush(out);

        // Send messages to server
        
        len = size;
        result = sendall(sockfd, mpackdata, &len);
        // TODO check for errors
				
	} while (parser.terminal);		// If we're connected to a serial device, keep reading, otherwise exit
	
	fclose(out);
    close(sockfd);
	telegram_parser_close(&parser);
	
	return 0;
}

/*


*/
