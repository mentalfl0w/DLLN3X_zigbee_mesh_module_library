#include "DLLN3X.h"

DLLN3X dlln33;  //Instantiating DLLN3X module

void zigbee_call_back(unsigned char orig_port,
                      unsigned char dest_port, unsigned short addr,
                      unsigned char data[], int length) {
  switch (orig_port) {
    case 0x82:
      {
        /* Do everything you want to do */
        Serial.println("Hello from port 0x82!");
        break;

        default:
          break;
      }
  }
}

void setup() {
  dlln33.init(&Serial1);
  dlln33.setcallback(zigbee_call_back);
  Serial.printf("DLLN33 addr is 0x%X\n", dlln33.read_addr());
  dlln33.send(0x81, 0x82, dlln33.read_addr(), (uint8_t *)"Hello to port 0x81!", 21);
}

void loop() {
  dlln33.loop();
}