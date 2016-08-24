# nslc
A serial link control protocol.

Inspired heavily by [Arduhdlc](https://github.com/jarkko-hautakorpi/Arduhdlc), this is a protocol and handler implementation for
Arduino and Python.

Like HDLC, the protocol is frame-based, but nslc is *not* in any way shape or form compatible with HDLC.

Frames consists of a series of data bytes followed by a checksum byte, followed by a LF (0x0A).

TAB characters appearing in the frame are replaced with the two-byte sequence  TAB+SPACE (0x09, 0x20) before transmission, then
LF characters appearing in the frame are replaced with the two-byte sequence TAB+LF (0x09, 0x0A)  before transmission.

On reception, TAB+LF sequences are replaced with LF, and TAB+SPACE sequences with TAB.

This yields a checksummed protocol that does a reasonable job at preserving strings, producing only a possible garbage character
at the end of each line.

The checksum used is an 8-bit [Pearson Hash](https://en.wikipedia.org/wiki/Pearson_hashing) with the following table T:
```c++
(uint_8) T[] = {13,161,243,130,182,147,68,235,82,83,221,191,112,38,135,184,143,206
,240,116,253,131,106,220,231,250,120,75,223,79,196,16,69,230,140,101
,150,98,245,104,111,20,236,216,192,76,6,185,64,144,12,96,94,165
,152,190,99,203,63,66,226,232,164,224,193,145,119,160,159,51,17,108
,149,93,24,222,121,7,215,124,70,141,207,227,211,80,50,173,157,163
,167,233,14,72,11,202,225,100,1,40,142,113,168,132,86,84,62,178
,248,228,118,28,247,153,15,172,92,23,158,47,189,155,48,52,169,134
,25,219,170,49,218,59,45,67,136,156,186,244,29,35,3,199,209,176
,31,175,58,128,73,234,133,213,95,122,177,148,238,208,85,109,217,0
,36,33,174,171,241,205,146,204,229,61,10,26,129,107,183,74,195,42
,125,2,105,32,110,194,187,114,138,181,57,46,91,55,198,137,162,89
,78,41,115,88,123,44,34,201,39,43,103,53,4,212,30,237,81,180
,127,54,90,179,102,19,214,5,37,97,126,200,27,65,154,21,252,166
,71,22,151,77,9,251,117,188,56,197,8,139,210,249,239,246,254,255
,242,87,60,18};
```

With this choice of T, the strings "help\n" and "help\r\n" checksum correctly ( "p" is the checksum byte for "hel" and "\r" is the checksum byte for "help").
If devices implement a handler for these sequences, then even someone with a misconfigured terminal
will be able to type "help", press return and see help on how to use the device.


