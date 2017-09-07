/*
   File: parser-tools.rl

   	  Ragel state-machine actions and definitions used to build parsers.
   	  
   	  (c)2015-2017, Levien van Zon (levien at zonnetjes.net, https://github.com/lvzon)			
*/


%%{
	machine parser;
	access fsm->;

	# A buffer to collect command arguments

	# Append to the string buffer.
	action str_append {
		if ( fsm->buflen < PARSER_BUFLEN )
			fsm->buffer[fsm->buflen++] = fc;
	}

	# Terminate a buffer.
	action str_term {
		if ( fsm->buflen < PARSER_BUFLEN )
			fsm->buffer[fsm->buflen++] = 0;
	}

	# Clear out the buffer
	action clearbuf { fsm->buflen = 0; }
	
	
	# Add a new integer argument to the stack
	action addarg { 
		fsm->arg[fsm->argc] *= fsm->multiplier;
		if ( fsm->argc < PARSER_MAXARGS )
			fsm->argc++;
		fsm->multiplier = 1;
	}
	
	# Push a string onto the string argument stack
	action addstr {
		if ( fsm->strargc < PARSER_MAXARGS )
			fsm->strarg[fsm->strargc++] = fsm->buffer + fsm->buflen;
	}

	# Add a single character argument to the stack
	action addchar { 
		fsm->arg[fsm->argc] = fc;
		if ( fsm->argc < PARSER_MAXARGS )
			fsm->argc++;
	}
	
	# Set current argument to zero
	action cleararg { 
		fsm->arg[fsm->argc] = 0;
		fsm->bitcount = 0;
	}
	
	# Set all arguments to zero
	action clearargs {
		int arg;
		for (arg = 0 ; arg < fsm->argc ; arg++) {
			// printf("clearing argument %d of %d\n", arg, fsm->argc);
			fsm->arg[arg] = 0;
		}
		fsm->multiplier = 1;
		fsm->argc = 0;
		fsm->bitcount = 0;
		
		for (arg = 0 ; arg < fsm->strargc ; arg++) {
			fsm->strarg[arg] = NULL;
		}
		fsm->strargc = 0;
		fsm->buflen = 0;
	}
	
	# Set next lowest bit in the current argument
	action addbit1_low { 
		fsm->arg[fsm->argc] = (fsm->arg[fsm->argc] << 1) | 1;
	}

	# Clear next lowest bit in the current argument
	action addbit0_low { 
		fsm->arg[fsm->argc] <<= 1;
	}
	
	# Set next highest bit in the current argument
	action addbit1_high { 
		fsm->arg[fsm->argc] = fsm->arg[fsm->argc] | (1 << fsm->bitcount++);
	}

	# Clear next highest bit in the current argument
	action addbit0_high { 
		fsm->arg[fsm->argc] = fsm->arg[fsm->argc] & ~(1 << fsm->bitcount++);
	}
	
	# Add a decimal digit to the current argument
	action add_digit { 
		fsm->arg[fsm->argc] = fsm->arg[fsm->argc] * 10 + (fc - '0');
	}
		
	# Add a hexadecimal digit to the current argument
	action add_hexdigit { 
		
		int value;
		
		if (isdigit(fc)) {
			value = fc - '0';
		} else if (isupper(fc)) {
			value = fc - 'A' + 10;
		} else {
			value = fc - 'a' + 10;
		}
		
		fsm->arg[fsm->argc] = fsm->arg[fsm->argc] * 16 + value;
	}
	
	# Negate the current argument
	action negate { 
		fsm->multiplier = -1;
	}
	
	# Get an unsigned int from the stack and append it as byte-value to the byte-buffer.
	action byte_append {
		
		fsm->argc--;
		unsigned char byte = fsm->arg[fsm->argc] & 0xff;
		unsigned char *dest;
		
		if ( fsm->buflen < PARSER_BUFLEN ) {
			dest = (unsigned char *)(fsm->buffer) + fsm->buflen++;
			*dest = byte; 
		}
		
		fsm->arg[fsm->argc] = 0;
		fsm->bitcount = 0;
	}
	
	# Write a high nibble-value to the last byte of the byte-buffer.
	action hexnibblehigh_append {
		
		unsigned int value;
		
		if (isdigit(fc)) {
			value = fc - '0';
		} else if (isupper(fc)) {
			value = fc - 'A' + 10;
		} else {
			value = fc - 'a' + 10;
		}
		
		unsigned char byte = value << 4; 
		unsigned char *dest;
		
		if ( fsm->buflen < PARSER_BUFLEN ) {
			dest = (unsigned char *)(fsm->buffer) + fsm->buflen;
			*dest = byte; 
		}		
	}

	# Append a low nibble-value to the byte-buffer.
	action hexnibblelow_append {
		
		unsigned int value;
		
		if (isdigit(fc)) {
			value = fc - '0';
		} else if (isupper(fc)) {
			value = fc - 'A' + 10;
		} else {
			value = fc - 'a' + 10;
		}
		
		unsigned char nibble = value & 0x0f;
		unsigned char *dest;
		
		if ( fsm->buflen < PARSER_BUFLEN ) {
			dest = (unsigned char *)(fsm->buffer) + fsm->buflen++;
			*dest = (*dest & 0xf0) | nibble;
		}		
	}
	
	# Helpers to collect arguments
	
	string = ^[\0\n;]+ >addstr $str_append %str_term;
	integer = ( ('-' @negate) | '+' )? ( digit @add_digit )+ >cleararg %addarg;	# Parse and store signed integer argument
	uinteger = ( digit @add_digit )+ >cleararg %addarg;				# Parse and store unsigned integer arument
	hexint = '0x'? ( xdigit @add_hexdigit )+ >cleararg %addarg;		# Parse and store unsigned hexadecimal integer argument
	bitmask = ( '0b'? '0'@addbit0_low | '1'@addbit1_low )+ >cleararg %addarg;		# Parse and store binary argument (MSB first)
	reversebitmask = ( '0'@addbit0_high | '1'@addbit1_high )+ >cleararg %addarg;		# Parse and store reversed bitstring argument (LSB first)
	bitval = ('0x' hexint | '0b' bitmask | reversebitmask);		# A bitmask in hex or binary or as reverse-bitstring
	uintval = ('0x' hexint | uinteger);		# A numeric value as hex or unsigned integer
	bytes = (uintval @byte_append space+?)+;	# A sequence of numeric byte values (0x00 - 0xff or 0 - 255) separated by spaces
	hexoctet = (xdigit @hexnibblehigh_append) (xdigit @hexnibblelow_append);	# A byte represented as two hex digits
	hexstring = hexoctet+ >addstr %str_term;		# Parse and store unsigned hexadecimal integer argument
}%%
