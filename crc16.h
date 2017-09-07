/*
   File: crc16.h

   	  Functions to calculate CRC16
*/

#include <inttypes.h>

uint16_t crc16_ccitt (const uint8_t *data, unsigned int length);
uint16_t crc16 (const uint8_t *data, unsigned int length);
