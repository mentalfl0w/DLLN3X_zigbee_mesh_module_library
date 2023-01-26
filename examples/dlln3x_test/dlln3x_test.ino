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
      char temp[200];
      sprintf(temp, "Hello from port 0x%X: %s",orig_port,data); 
      Serial.println(temp);
      break;
    }
    default:
        break;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial || millis() < 5000)
    ;
  dlln33.init(&Serial1);// could be used like HardwareSerial(Serial1, Serial2 and etc.) or SoftwareSerial
  dlln33.setcallback(zigbee_call_back);
  while(dlln33.read_addr()==0)
  {
    Serial.println("Waiting for DLLN3X to connect........");
    delay(1000);
  }
  char temp[20];
  sprintf(temp, "DLLN3X addr: 0x%04X.",dlln33.read_addr());
  Serial.println(temp);
}

uint64_t interval = 5000, pre_milli = 0;
void loop() {
  dlln33.loop();
  if (millis() - pre_milli >= interval) {
    zframe.src_port = 0x81;
    zframe.des_port = 0x82;
    *((uint16_t *)&zframe.remote_addrL) = dlln33.read_addr(); // send pkg to self.
    strncpy((char*)zframe.data,"Hello to port 0x82!",19);
    zframe.length = 19;
    dlln33.send(&zframe);
    pre_milli = millis();
  }
}