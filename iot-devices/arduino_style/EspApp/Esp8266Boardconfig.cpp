#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include "Esp8266Boardconfig.h"
#include "Config.h"

Esp8266Boardconfig::Esp8266Boardconfig() {
    Serial.println("Esp8266Boardconfig constructor");
    initDone = false;
    this->status = 0;
}

bool Esp8266Boardconfig::setDeviceId(String id) {
    this->deviceId = id ;
    Serial.print("set device id : ");
    Serial.println(this->deviceId);
}

bool Esp8266Boardconfig::setSsid(String ssid) {
    this->ssid = ssid;
}

bool Esp8266Boardconfig::setPassword(String password) {
    this->password = password;
}

bool Esp8266Boardconfig::loadConfigFromFlash(void) {
 //Read File data
  char devId[BUF_SIZE] = {0};
  char ssidName[BUF_SIZE] = {0};
  char ssidPassword[BUF_SIZE] = {0};

  File f = SPIFFS.open(BOARD_CONF, "r");
  if (!f) {
    Serial.println("file open failed");
    return false;
  }

  this->status = f.read();
  f.readBytes(devId,BUF_SIZE);
  f.readBytes(ssidName,BUF_SIZE);
  f.readBytes(ssidPassword,BUF_SIZE);
  f.close();  //Close file

  this->deviceId = String(devId);
  this->ssid = String(ssidName);
  this->password = String(ssidPassword);

  Serial.print("Wifi configuration  : ");
  Serial.println(isWifiConfigured());
  Serial.print("device ID           : ");
  Serial.println(this->deviceId);
  Serial.print("SSID                : ");
  Serial.println(this->ssid);
  Serial.print("Password            : ");
  Serial.println(this->password);
  Serial.println(".........");
//   /delay(1000);
  return true;
}

bool Esp8266Boardconfig::loadConfigToFlash() {
  File f = SPIFFS.open(BOARD_CONF, "w");
  if (!f) {
    Serial.println("Board config file open failed");
    return false;
  }
  //Write data to file
  Serial.println("Store wifi configuration");
  f.write((uint8_t)this->status);
  f.write((uint8_t *)this->deviceId.c_str(),BUF_SIZE);
  f.write((uint8_t *)this->ssid.c_str(),BUF_SIZE);
  f.write((uint8_t *)this->password.c_str(),BUF_SIZE);
  f.close();
//   delay(1000);
  return true;
}

void Esp8266Boardconfig::doWifiSetup() {
  if (loadConfigFromFlash() != true ) {
      Serial.println("Load board config.......fail");
  }

  if ( !isWifiConfigured() ) {
    Serial.println("Wifi not configured... starting smart connfig...");
    startSmartConfig();
    this->status = this->status | 0x01;
    this->deviceId = mUtils.genDeviceId();
    loadConfigToFlash();
    return;
  }

  Serial.println("Wifi already configured");
  WiFi.begin(this->ssid.c_str(), this->password.c_str());
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

  this->ssid = WiFi.SSID();
  this->password = WiFi.psk();

  Serial.println("done!");
  Serial.print("ssid:");
  Serial.println(this->ssid);
  Serial.print("password:");
  Serial.println(this->password);

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
    bool result = this->status & 0x01;
    return result;
}

bool Esp8266Boardconfig::factoryReset(bool configReset) {
  this->status = '\0';
  this->deviceId.remove(0);
  this->ssid.remove(0);
  this->password.remove(0);
  loadConfigToFlash();
}

String Esp8266Boardconfig::getDeviceId() {
  return this->deviceId;
}

char Esp8266Boardconfig::getStatus() {
    return this->status;
}

void Esp8266Boardconfig::setStatus(char st) {
    this->status = st;
}
