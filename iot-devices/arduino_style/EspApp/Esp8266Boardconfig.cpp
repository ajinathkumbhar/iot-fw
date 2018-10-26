#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include "Esp8266Boardconfig.h"
#include "Config.h"

Esp8266Boardconfig::Esp8266Boardconfig() {
    Serial.println("Esp8266Boardconfig constructor");
    initDone = false;
}

bool Esp8266Boardconfig::setDeviceId(char * id) {
    strncpy(dev_id,id,BUF_SIZE - 1);
}

bool Esp8266Boardconfig::setSsid(char * ssid) {
    strncpy(this->ssid,ssid,BUF_SIZE - 1);
}

bool Esp8266Boardconfig::setPassword(char * password) {
    strncpy(this->password,password,BUF_SIZE - 1);
}

bool Esp8266Boardconfig::loadConfigFromFlash(void) {
 //Read File data
  File f = SPIFFS.open(BOARD_CONF, "r");
  if (!f) {
    Serial.println("file open failed");
    return false;
  }

  f.readBytes(status,BUF_SIZE);
  f.readBytes(dev_id,BUF_SIZE);
  f.readBytes(ssid,BUF_SIZE);
  f.readBytes(password,BUF_SIZE);
  f.close();  //Close file

  Serial.print("status: ");
  Serial.println(status[0]);
  Serial.print("device ID: ");
  Serial.println(dev_id);
  Serial.print("SSID     : ");
  Serial.println(ssid);
  Serial.print("Password : ");
  Serial.println(password);
  Serial.println(".........");
//   /delay(1000);
  return true;
}

bool Esp8266Boardconfig::loadConfigToFlash() {
  File f = SPIFFS.open(BOARD_CONF, "w");
  if (!f) {
    Serial.println("file open failed");
    return false;
  }
  //Write data to file
  Serial.println("storing wifi configuration");
  f.write((uint8_t *)status,BUF_SIZE);
  f.write((uint8_t *)dev_id,BUF_SIZE);
  f.write((uint8_t *)ssid,BUF_SIZE);
  f.write((uint8_t *)password,BUF_SIZE);
  f.close();
//   delay(1000);
  return true;
}

void Esp8266Boardconfig::doWifiSetup() {
  if (loadConfigFromFlash() != true ) {
      Serial.println("Err : Load board config.......fail");
  }
  if (status[0] != 0xff) {
    Serial.println("Err : user config corroupted, need to reconfigure.......fail");
    startSmartConfig();
    status[0] = 0xff;
    loadConfigToFlash();
    return;
  }
  Serial.println("User config found...");
  WiFi.begin(ssid, password);
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

bool Esp8266Boardconfig::startSmartConfig() {
  Serial.println("");
  WiFi.mode(WIFI_STA);
  delay(500);

  Serial.print("smart config...");
  WiFi.beginSmartConfig();
  while (!WiFi.smartConfigDone()) {
    delay(1000);
    Serial.print(".");
  }

  strncpy(this->ssid,WiFi.SSID().c_str(),sizeof(this->ssid));
  strncpy(this->password,WiFi.psk().c_str(),sizeof(this->password));

  Serial.println("done!");
  Serial.print("ssid:");
  Serial.println(ssid);
  Serial.print("password:");
  Serial.println(password);

  Serial.print("connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("done!");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  return true;
}

bool Esp8266Boardconfig::isWifiConfigured() {
    return status[0] != 0xff ? false : true;
}

bool Esp8266Boardconfig::factoryReset(bool configReset) {
  memset(this->status, 0, sizeof(this->status));
  memset(this->dev_id, 0, sizeof(this->dev_id));
  memset(this->ssid, 0, sizeof(this->ssid));
  memset(this->password, 0, sizeof(this->password));
  loadConfigToFlash();
}
