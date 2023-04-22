#include "DLLN3X.h"

using namespace zigbee_protocol;

DLLN3X dlln33;  //Instantiating DLLN3X module
void zigbee_call_back(ZigbeeFrame &zframe) {
  switch (zframe.getDesPort()) {
    case 0x82:
    {
        /* Do everything you want to do */
      char temp[200];
      sprintf(temp, "Hello from port 0x%X: %.*s",zframe.getSrcPort(),zframe.getDataLength(),zframe.getData().data()); 
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
  char temp[50];
  sprintf(temp, "DLLN3X addr: 0x%04X.",dlln33.read_addr());
  Serial.println(temp);
  sprintf(temp, "DLLN3X network id: 0x%04X.",dlln33.read_network_id());
  Serial.println(temp);
  sprintf(temp, "DLLN3X channel: 0x%02X.",dlln33.read_channel());
  Serial.println(temp);
  sprintf(temp, "DLLN3X baud rate: %d.",dlln33.read_baudrate());
  Serial.println(temp);
  dlln33.set_addr(0xFFFF); //test error report
  //dlln33.get_link_quality(YOUR_ANOTHER_CHIP_ADDR); //test link quility
  dlln33.pin_control(DLLN3X::PIN::DLLN3X_PIN4, DLLN3X::PIN_CONTROL::OUT_HIGH); //test pin, PIN4 is TTx, PIN5 is TRx
}

uint64_t interval = 5000, pre_milli = 0;
void loop() {
  dlln33.loop();
  if (millis() - pre_milli >= interval) {
    ZigbeeFrame zf;
    zf.setSrcPort(0x81);
    zf.setDesPort(0x82);
    zf.setRemoteAddr(dlln33.read_addr());
    zf.setData("Hello to port 0x82!");
    dlln33.send(zf);
    zf = zf + "I'm port 0x81!";
    dlln33.send(zf);
    pre_milli = millis();
  }
}