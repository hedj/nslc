#include <nslc.h>

// A simple echo-server using the NSLC framing
// (C) John Hedditch, 2016

#define MAX_FRAME_SIZE 256

void frame_handler(const uint8_t *, uint16_t);
void send_byte(uint8_t);

NSLC lc = NSLC(&send_byte, &frame_handler, MAX_FRAME_SIZE);

void frame_handler(const uint8_t *data, uint16_t len) {
  lc.frameDecode((char *)data, len); // echo the message back to the sender.
}
void send_byte(uint8_t b) {
  Serial.write((char)b);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    lc.charReceiver( (char)Serial.read() );
  }
}
