/*
   File: crc16.c

   	  Functions to calculate CRC16
*/

#include <inttypes.h>

uint16_t crc16_ccitt (const uint8_t *data, unsigned int length)
{
	// Polynomial: x^16 + x^12 + x^5 + 1 (0x8408)
    // Initial value: 0xffff
    
    uint8_t x;
    uint16_t crc = 0xffff;

    while (length--) {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    
    return crc;
}


uint16_t crc16 (const uint8_t *data, unsigned int length)
{
    // Polynomial: x^16 + x^15 + x^2 + 1 (0xa001)
	
    uint16_t crc = 0;

    while (length--) {
    	
		int i;
	
		crc ^= *data++;
		for (i = 0 ; i < 8 ; ++i) {
			if (crc & 1)
				crc = (crc >> 1) ^ 0xa001;
			else
				crc = (crc >> 1);
		}    			
    }
    
    return crc;
}
