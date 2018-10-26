#include <Arduino.h>
#include "Utils.h"
#include "Config.h"

void Utils::genDeviceId(char * id){
  String dev_id = "ESP8266C-";   // Create a random MQTT client ID
  dev_id += String(random(0xffff), HEX);
  Serial.print("Device id generated : ");
  Serial.println(dev_id);
  strncpy(id,dev_id.c_str(),BUF_SIZE - 1);
  return;
}
