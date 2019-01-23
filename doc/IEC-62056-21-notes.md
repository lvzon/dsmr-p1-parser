# IEC 62056-21 and derivatives (e.g. DSMR P1)

**Levien van Zon**

### Hardware

Optical "D0" reading/writing head for IEC 62056-21 can be ordered e.g. here:

   - <https://www.amazon.de/dp/B01B8N0ASY/ref=pe_3044161_185740101_TE_item>

It uses infrared light (between 800 and 1000 nm) for signal transmission and reception, and is generally kept in place with a magnet. The cable should always point downward.


### Serial connection

The optical IEC 62056-21 interface initially operates at 300 baud, 7N1, even parity. It can be used for both reading and writing, although this should to be done at the same time (it is half-duplex).

The DSMR P1 interface is read-only and uses a fixed baud rate of either 9600 baud, or 115200 baud for DSMR version >=4.0, and telegrams are sent every 1 or 10 seconds, as long as the RTS line is high.

Both interfaces use an inverted serial signal relative to a "normal" serial port. This means that a binary 1 is represented by the electrical or optical signal being low (0V or no IR light), and a binary 0 is represented by +5V or an IR light signal. If you use a regular TTL serial port, extra hardware (e.g. an inverter IC or a transistor) is usually needed to invert the serial signal. RS232 also uses an inverted signal.


### Wake-up

Especially battery powered meters require a wake-up sequence to be sent before further communication on the optical IEC 62056-21 interface. In most cases, sending a string of 65 zero-characters ('\0') should do the trick, followed by a pause of 1.5 seconds. If serial data is sent asynchronously, make sure you also wait for the data to be sent, which probably requires a total waiting time of 2.7 seconds after sending the wake-up string. 

The DSMR P1 interface does not require wake-up, but it sonly sends data if the RTS pin is high (>4V).


### IEC 62056-21 Protocol modes

IEC 62056-21 meters may support several different protocol modes:

   A. Fixed rate (300 baud), bidirectional ASCII protocol. The master device sends a sign-on sequence, the slave device (meter) responds with an identifier followed by a data telegram. The master may optionally enter programming mode after receiving the telegram.

   B. Bidirectional ASCII protocol with baud-rate switching. This is similar to mode A, but after transmitting the identifier at 300 baud, the slave device may switch to a higher baud rate, which is specified in the identifier.

   C. This is similar to mode B, but it allows manufacturer-specific extentions and a device in mode C will not automatically send a data telegram following the identifier. Instead, the master has to switch to readout or programming mode, and may also specify whether the baud rate should be switched or not.

   D. Fixed rate (2400 baud) unidirectional ASCII protocol. Data is either pushed by the meter or requested some other way (e.g. by pushing a button). The meter sends an identifier followed by a data telegram.

   E. This is an extended version of modes A-C, whereby the meter may specify that it supports other (e.g. binary) transmission protocols. 

DSMR P1 is essentially a slightly non-standard variant of IEC 62056-21 protocol mode D, using a higher baud rate, whereby the telegram is requested by a signal on the RTS pin of the P1 port.


### Sign-on sequence

When using the optical IEC 62056-21 interface in all modes except D, a sign-on sequence is required before a telegram is sent by the meter. This sign-on sequence is sent at 300 baud, and is as follows: `"/?!\r\n"` It may include an optional device address between ? and !.

After successful reception of the sign-on-sequence, the meter should send a data line, which starts with '/', followed by an identification string and the end-of-line sequence "\r\n" (CR+LF). Example: "/ISk5MT171-0133\r\n".

The first three characters of the identification string are the vendor ID, which consists of two upper case letters and a third letter which may be either upper or lower case. If the third letter is lower case, the device has a minimum reaction time of 20ms. If the device transmits only upper case letters, a minimum reaction time of 200ms should be assumed, although the device *may* still support 20ms. 

The fourth character is the baud rate identifier:

   0.   300 Bd
   1.   600 Bd
   2.  1200 Bd
   3.  2400 Bd
   4.  4800 Bd
   5.  9600 Bd
   6. 19200 Bd

The above baud rate identifier values are used when a meter operates in protocol mode C, D or E. If the meter operates in mode A, the baud rate is always 300 baud and the identifier can be any character except [0-9], [A-I], '/' or '!'. If the meter operates in mode B, the baud rate identifier is:

   A.   300 Bd
   B.   600 Bd
   C.  1200 Bd
   D.  2400 Bd
   E.  4800 Bd
   F.  9600 Bd
   G. 19200 Bd

The rest of the identification string is the model identifier, up to "\r\n" or (in mode E) an optional '\'-character, which acts as sequence-delimiter and may be followed by mode-information, e.g.: `/ISk5\2MT382-1000`
In this case, the character following `\` is the mode identifier, which may be '2' for binary mode (HDLC). The model identifier (with optional mode information) may be up to 16 bytes long.

If the baud rate identifier is valid for mode C or E, and the optical interface is used, the communication rate can/should be updated. To do this, send an acknowledgement message (still at 300 baud), which starts with an ACK byte (0x06), followed by a protocol control character (use '0' for a normal protocol procedure), the baud rate identifier (see above), a mode control character (use '0' to set the mode to reading data) and "\r\n". Wait 300 ms before changing the baud rate. 

After sign-on (and in mode C/E, acknowledgement of readout mode), the meter will send a telegram, after which it will return to 300 baud. In the case of DSMR P1, sign-on, acknowledgement or baud rate change is not needed. DSMR P1 uses a fixed baud rate of either 9600 baud, or 115200 baud for DSMR version >=4.0, and telegrams are sent every 1 or 10 seconds, as long as the RTS line is high.


### Telegrams

A telegram starts with '/', followed by an identifier string, as described above.
Each subsequent line of a telegram is a data object, which starts with an object identifier, followed by a value, an optional unit and a line terminator "\r\n".Example: `"1-0:1.8.1(000581.161*kWh)\r\n"`

The section up to the front boundary character '(' is the OBIS object identification (see below), which may have a maximum size of 16 characters, and may include anay character except "(", ")", "/" and "!".

After the front boundary character '(' comes a value. This value may have a maximum size of 32 characters, or 128 in protocol mode C. All characters are allowed, except "(", " * ", ")", "/" and "!". A decimal point is used, rather than a decimal comma, and this counts in the number of characters.

The value may be followed by a separator character '*' and a unit of maximum 16 characters, which may contain any character except "(", ")", "/" and "!". 

The data set ends with the read boundary character ')', and the line ending '\r\n'

Some meters (e.g. the ISKRA MT171) seem to include the unit in the value (e.g.: `"1-0:1.8.1*255(0000000 kWh)\r\n"), which may be technically allowed but makes things harder to parse...

The telegram ends with '!'. In DSMR P1 v4 and above, this is followed by a CRC16 over the preceding characters (from / to !, both inclusive). The CRC is 4 characters long: 2 bytes, hex-encoded, MSB first. After the optional CRC, one or two line-ends (\r\n) are usually sent. 


### OBIS object identifiers

The OBIS object identifiers are described by [IEC 62056-61: Electricity metering - Data exchange for meter reading – Part 61 : Object identification system (OBIS)](http://webstore.iec.ch/webstore/webstore.nsf/mysearchajax?Openform&key=62056-61&sorting=&start=1&onglet=1).
An OBIS code consists of six value groups: `A-B:C.D.E*F`
The first value A specifies the concept or physical medium that the object refers to:

   0. Abstract objects
   1. Electricity
   4. Heating costs
   5. Cooling energy
   6. Heat
   7. Gas
   8. Water
   9. Warm water

The second value B is the channel, which for electricity is generally '0' (not used), but which can also be 'd' (difference), '1' (tentative value), '2' (final value) or 'p' (processing status).

The third value C specifies the variable type, e.g. general purpose (0), imported active power/energy (1), exported active power/energy (2), frequency (14), phase A/B/C imported active power/energy (21/41/61), phase A/B/C exported active power/energy (22/42/62), phase A/B/C current (31/51/71), phase A/B/C voltage (32/52/72), service variable ('C'), error message ('F'), list object ('L'), etc.

The fourth value D specifies the measurement or value type, e.g. energy (8), the instantaneous value (7) or various types of minimum, maximum and average values.

The fifth value E specifies the tariff, whereby 0 is total, 1 is tariff 1, 2 is tariff 2, etc.

The final separator '*' and value F seem to be optional, and specify the billing periods.

Common objects:
   - 0.0.0 is the 8-character meter number, e.g. `1-0:0.0.0*255(38820967)`
   - 0.2.0 is the meter firmware version, e.g. `1-0:0.2.0*255(V1.0)`
   - 0.1.2*xx is the timestamp of billing period xx
   - F.F is an error message (although I've seen at least one meter incorrectly report this as "`FF(00000000)`")



### ACK and sign-off

After receiveing a telegram, the master should send an ACK (0x06) or NAK (0x15) byte to either confirm reception of the telegram, or request a re-send.

The master may sign off explicitly, using a break sequence: SOH (start of header, 0x01) 'B' (exit command) '0' (complete sign-off) ETX (end of frame, 0x03) 'q' (Block check character, the calculated length parity over the characters of the data message beginning immediately after the STX up to the included ETX.)

In the case of DSMR P1, telegrams are sent as long as the RTS line is high.


### Notes

In battery-powered devices (e.g. gas, thermal and water meters), each wake-up and request may reduce the battery life span by several hours to days.


Sources:

   - IEC 62056-21, "Electricity metering – Data exchange for meter reading, tariff and load control – Part 21: Direct local data exchange" (which is not available for free, but can be easily found with Google). 
   - <http://manuals.lian98.biz/doc.en/html/u_iec62056_struct.htm>
   - <https://github.com/ohitz/smartmeter-readout>
   - <http://www.satec-global.com/sites/default/files/EM720-IEC-62056-21.pdf>

