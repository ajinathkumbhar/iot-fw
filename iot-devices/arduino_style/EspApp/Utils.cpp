#include <Arduino.h>
#include "Utils.h"
#include "Config.h"

String Utils::genDeviceId(){
  String dev_id = "ESP8266C-";   // Create a random MQTT client ID
  dev_id += String(random(0xffff), HEX);
  Serial.print("Device id generated : ");
  Serial.println(dev_id);
  return dev_id;
}
