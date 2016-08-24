#ifndef nslc_h
#define nslc_h

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>

typedef void (* sendchar_type) (uint8_t);
typedef void (* frame_handler_type)(const uint8_t *framebuffer, uint16_t framelength);

class NSLC
{
  public:
    NSLC (sendchar_type, frame_handler_type, uint16_t max_frame_length);
    void charReceiver(uint8_t data);
    void frameDecode(const char *framebuffer, uint8_t frame_length);

  private:
    /* User must define a function, that sends a 8bit char over the chosen interface, usart, spi, i2c etc. */
    sendchar_type sendchar_function;
    /* User must define a function, that will process the valid received frame */
    /* This function can act like a command router/dispatcher */
    frame_handler_type frame_handler;
    void sendchar(uint8_t data);

    bool escape_character;
    uint8_t * receive_frame_buffer;
    uint8_t frame_position;
    uint8_t frame_checksum;
	uint16_t max_frame_length;
    uint8_t T[256] = {13,161,243,130,182,147,68,235,82,83,221,191,112,38,135,184,143,206
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
};

#endif
