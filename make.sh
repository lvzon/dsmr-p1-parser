#!/bin/bash

ragel -s p1-parser.rl
gcc -Wall -Os -g -o p1-test p1-parser.c p1-lib.c p1-test.c crc16.c
gcc -std=gnu99 -Wall -Os -g -o p1-packmsg p1-parser.c p1-lib.c p1-packmsg.c crc16.c mpack.c net_ip.c

# To check for memory issues:
# valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./p1-packmsg
