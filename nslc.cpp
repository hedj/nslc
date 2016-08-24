/*
NSLC is a Non-Standard Line Control protocol
Copyright (C) 2016 John Hedditch; see LICENSE.txt

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Arduino.h"
#include "nslc.h"

/* The frame boundary octet is LF, 00001010, (0A in hexadecimal notation) */
#define FRAME_BOUNDARY_OCTET 0x0A

/* A "control escape octet", TAB, has the bit sequence '00001001', (09 hexadecimal) */
#define CONTROL_ESCAPE_OCTET 0x09

/* When transmitting, TAB -> TAB+SPACE  (0x09, 0x20), and LF -> TAB+LF (0x09, 0x0A) */
/* When receiving, sequence is reversed. 

/* The frame check sequence (FCS) is an 8-bit Pearson hash */

NSLC::NSLC (sendchar_type put_char, frame_handler_type hdlc_command_router, uint16_t max_frame_length) : sendchar_function(put_char), frame_handler(hdlc_command_router)
{
    this->frame_position = 0;
	this->max_frame_length = max_frame_length;
	this->receive_frame_buffer = (uint8_t *)malloc(max_frame_length+1); // char *ab = (char*)malloc(12);
    this->frame_checksum = 0;
    this->escape_character = false;
}

/* Function to send a byte throug USART, I2C, SPI etc.*/
void NSLC::sendchar(uint8_t data)
{
    (*this->sendchar_function)(data);
}

/* Find valid frame from incoming data */
void NSLC::charReceiver(uint8_t data)
{

    if(data == FRAME_BOUNDARY_OCTET) {
      for(int i=0; i< this->frame_position-1; i++) {
        this->frame_checksum = T[this->frame_checksum ^ this->receive_frame_buffer[i]];
      }

        if ((this->frame_position >=2) &&
            (this->frame_checksum == this->receive_frame_buffer[this->frame_position-1])) {
            // Valid frame found, call the handler
            (*frame_handler)(receive_frame_buffer,(uint8_t)(this->frame_position-1));
        }
        this->frame_position = 0;
        this->frame_checksum = 0;
        this->escape_character = false;
        return;
    }


    if(this->escape_character)
    {
        this->escape_character = false;
        if ( data == 0x20 ) { data = 0x09; }
    }
    else if(data == CONTROL_ESCAPE_OCTET)
    {
        this->escape_character = true;
        return;
    }

    receive_frame_buffer[this->frame_position] = data;

    this->frame_position++;

    if(this->frame_position == this->max_frame_length)
    {
        this->frame_position = 0;
        this->frame_checksum = 0;
        this->escape_character = false;
    }
}

/* Wrap given data in NSLC frame and send it out byte at a time*/
void NSLC::frameDecode(const char *framebuffer, uint8_t frame_length)
{
    uint8_t data;
    uint8_t fcs = 0;

    while(frame_length)
    {
        data = *framebuffer++;
        fcs = this->T[fcs ^ data];

        if( data == CONTROL_ESCAPE_OCTET ) {
            this->sendchar(CONTROL_ESCAPE_OCTET);
            this->sendchar(0x20);
        } else if (data == FRAME_BOUNDARY_OCTET) {
            this->sendchar(CONTROL_ESCAPE_OCTET);
            this->sendchar(FRAME_BOUNDARY_OCTET);
        } else {
            this->sendchar((uint8_t)data);
        }
        frame_length--;
    }


    if (fcs == CONTROL_ESCAPE_OCTET) {
        this->sendchar(CONTROL_ESCAPE_OCTET);
        this->sendchar(0x20);
    } else if (fcs == FRAME_BOUNDARY_OCTET) {
        this->sendchar(CONTROL_ESCAPE_OCTET);
        this->sendchar(FRAME_BOUNDARY_OCTET);
    } else {
        this->sendchar(fcs);
    }

    this->sendchar(FRAME_BOUNDARY_OCTET);
}


