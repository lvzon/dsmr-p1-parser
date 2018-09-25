#!/bin/bash

ragel -s p1-parser.rl
gcc -Wall -Os -g -o p1-test p1-parser.c p1-lib.c p1-test.c crc16.c

