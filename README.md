# dsmr-p1-parser
Ragel-based C-parser for Dutch Smart Meter P1-data

## DSMR P1

DSMR (Dutch Smart Meter Requirements) is a standard for "smart" utility meters used in The Netherlands. Meters that comply with DSMR have several interfaces, including:

   - P1, a send-only serial interface that can be used to connect local devices to an electricity meter. The connected devices can then receive data from the electricity meter and its slave devices.
   - P2, an [M-bus](https://en.wikipedia.org/wiki/Meter-Bus) interface used for communication between utility meters (e.g. a gas meter, a water meter and an electricity meter, whereby the electricity meter acts as a master and the other meters as slaves). 
   - P3, an infrastructure to periodically send aggregated meter data from an electricity meter to a central server, using a cellular modem (GPRS, CDMA or LTE), ethernet or power-line communication (PLC). Data is generally sent at most once per day, aggregated at 10-minute intervals, for a subset of variables.  
   - P4, which is strictly speaking not an interface at the meter level, but a standard for storing data on and retrieving data from a central server architecture.

In addition to the above interfaces, most DSMR-meters also have additional interfaces, such as an S0 (DIN 43864) pulse-output and an optical [IEC 62056-21](https://en.wikipedia.org/wiki/IEC_62056#IEC_62056-21) serial interface.


## The Parser

This repository contains a parser for DSMR data-telegrams, based on the [Ragel state machine compiler](http://www.colm.net/open-source/ragel/), as well as the DSMR P1 specification documents (in `doc/`) and an example program in C for reading and parsing DSMR-data from a serial port in Linux (on any other POSIX system such as BSD or MacOS). To compile the Ragel parser and the example program, you need to install the Ragel state machine compile (on Debian, Ubuntu and derivatives, simply do: `sudo apt-get install ragel`) and run `./make.sh`. In principle this parser can also be adapted for use on microcontrollers, although I haven't tried this yet. The parser has been tested with example data from all currently known versions of DSMR (2.2, 3.0, 4.x and 5.0.2), and with real data from a Landis-Gyr DSMR 4.2.2 electricity meter, with a slave gas meter connected. This code is open-source under the Apache 2.0 licence.


## Hardware

The DSMR P1-port uses an RJ12 (6P6C) [modular connector](https://en.wikipedia.org/wiki/Modular_connector), which has 6 pins:

   1. Vcc: +5V power supply, max. 250 mA continuous (DSMR >= 4.0)
   2. RTS: data request, connect to +5V (4.0 - 5.5 V) to request data, connect to 0V/GND to stop data transmission
   3. GND: data ground
   4. NC: not connected
   5. RXD: data line (open collector output, logically inverted 5V serial signal, 8N1, 115200 baud for DSMR >= 4.0, 9600 baud for DSMR < 4.0)
   6. VccGND: power ground (DSMR >= 4.0)

All P1-pins are galvanically isolated from mains, and have over-voltage protection.
The data-line is an inverted open-collector serial line output. It requires a pull-up resistor (2.2 - 10 kOhm) to be connected between RXD (pin 5) and +5V (RTS / pin 2 or Vcc / pin 1). To convert the inverted serial signal into a normal serial signal, either an inverter IC (e.g. [SN7404](http://www.ti.com/product/sn7404)) or circuit should be used, or a serial interface that can invert the signals (e.g. an FTDI USB to serial converter).

If RTS is set to HIGH, an ASCII text-based data-packet called a "telegram" is sent by the smart meter, either every 10 seconds (DSMR < 5.0) or every second (DSMR >= 5.0). The format of the telegrams is derived from the [IEC 62056-61](https://en.wikipedia.org/wiki/IEC_62056) standard for exchanging data with utility meters.
