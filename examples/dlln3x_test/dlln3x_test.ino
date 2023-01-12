#include "DLLN3X.h"

DLLN3X dlln33;  //Instantiating DLLN3X module
zigbee_frame zframe;
void zigbee_call_back(uint8_t orig_port,
                      uint8_t dest_port, uint16_t addr,
                      uint8_t data[], int length) {
  switch (dest_port) {
    case 0x82:
      {
        /* Do everything you want to do */
        printf("Hello from port 0x%X!",orig_port);
        break;

        default:
          break;
      }
  }
}

void setup() {
  dlln33.init(&Serial);// could be used like HardwareSerial(Serial1, Serial2 and etc.) or SoftwareSerial
  dlln33.setcallback(zigbee_call_back);
  printf("DLLN33 addr is 0x%X\n", dlln33.read_addr());
  zframe.src_port = 0x81;
  zframe.des_port = 0x82;
  *((uint16_t *)&zframe.remote_addrL) = dlln33.read_addr();
  strncpy((char*)zframe.data,"Hello to port 0x82!",21);
  zframe.length = 21;
  dlln33.send(&zframe);
}

void loop() {
  dlln33.loop();
}