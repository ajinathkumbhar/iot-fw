#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include "Esp8266Boardconfig.h"
#include "Config.h"

Esp8266Boardconfig::Esp8266Boardconfig() {
    Serial.println("Esp8266Boardconfig constructor");
    initDone = false;
    this->status = 0;
    mOnBoardLED  = new EspLight(LED_ONBOARD);
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
  Serial.println("Store board configuration");
  f.write((uint8_t)this->status);
  f.write((uint8_t *)this->deviceId.c_str(),BUF_SIZE);
  f.write((uint8_t *)this->ssid.c_str(),BUF_SIZE);
  f.write((uint8_t *)this->password.c_str(),BUF_SIZE);
  f.close();
//   delay(1000);
  return true;
}

void Esp8266Boardconfig::doWifiSetup() {
  int lineCharCount = 0;
  if (loadConfigFromFlash() != true ) {
      Serial.println("Load board config.......fail");
  }

  if ( FORCE_SMARTCONFIG || !isWifiConfigured() ) {
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
    delay(200);
    if ( lineCharCount++ > SERIAL_LOG_CHARCOUNT_IN_LINE ) {
      Serial.println();
      lineCharCount = 0;
    }
    Serial.print(".");
    mOnBoardLED->wifiConnecting();
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

bool Esp8266Boardconfig::startSmartConfig() {
  int lineCharCount = 0;
  Serial.println("");
  WiFi.mode(WIFI_STA);
  delay(500);

  Serial.print("smart config...");
  WiFi.beginSmartConfig();
  while (!WiFi.smartConfigDone()) {
    delay(200);
    Serial.print(".");
    if (lineCharCount++ > SERIAL_LOG_CHARCOUNT_IN_LINE) {
      Serial.println();
      lineCharCount = 0;
    }
    Serial.print(".");
    mOnBoardLED->smartConfigConnecting();
  }

  this->ssid = WiFi.SSID();
  this->password = WiFi.psk();

  Serial.println("done!");
  Serial.print("ssid:");
  Serial.println(this->ssid);
  Serial.print("password:");
  Serial.println(this->password);

  Serial.print("connecting...");

  lineCharCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (lineCharCount++ > SERIAL_LOG_CHARCOUNT_IN_LINE) {
      Serial.println();
      lineCharCount = 0;
    }
    Serial.print(".");
    mOnBoardLED->wifiConnecting();

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

bool Esp8266Boardconfig::wifiConnect(String ssid, String password) {
  int lineCharCount = 0;
  Serial.println("Use fix wifi connection");
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("ssid:");
  Serial.println(ssid);
  Serial.print("password:");
  Serial.println(password);
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (lineCharCount++ > SERIAL_LOG_CHARCOUNT_IN_LINE) {
      Serial.println();
      lineCharCount = 0;
    }
    Serial.print(".");
    mOnBoardLED->wifiConnecting();
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  this->status = this->status | 0x01;
  this->deviceId = mUtils.genDeviceId();
  loadConfigToFlash();
}

bool Esp8266Boardconfig::isInternetServiceAvailable() {
  WiFiClient client;
  char server[] = TEST_GOOGLE_SERVER;

  if (!client.connect(server, 80)) {
    Serial.println("Internet connection not available ...");
    return false;
  }
  Serial.println("connected to internet");
  // Make a HTTP request:
  client.println("GET /search?q=arduino HTTP/1.1");
  client.println("Host: www.google.com");
  client.println("Connection: close");
  client.println();
  return true;
}