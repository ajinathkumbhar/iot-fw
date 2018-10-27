//Include File System Headers
#include <FS.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "EspSpiffs.h"
#include "Esp8266Boardconfig.h"
#include "EspLightPoint.h"
#include "Config.h"
#include "Utils.h"


EspSpiffs mSpiffs;
Esp8266Boardconfig mBoardconfig;
Utils mUtils;
EspLightPoint * mLightPoint;

//setup light point device
void setupLightPoint() {
  String devId = String(mBoardconfig.getDeviceId());
  String mqttHost = String(MQTT_BROKER_HOST);
  int mqttPort = MQTT_BROKER_PORT;

  mLightPoint = new EspLightPoint(devId);
  mLightPoint->setGatewayServer(mqttHost,mqttPort);
  mLightPoint->connectToGateway();

  while(mLightPoint->isConnected() != true ) {
    delay(500);;
  }
  mLightPoint->sendRegistrationReq();
  mLightPoint->doSubcription();
  Serial.println("setupLightPoint.......ok");
}

// ESP board setup
void setupBoard() {
  mBoardconfig.doWifiSetup();
  Serial.println("wifi_setup.......ok");
}

void setup() {
  Serial.begin(115200);
  // spiffs setup
  if (!mSpiffs.init()) {
    Serial.println("failed to setup file system");
  }

  mSpiffs.dumpStorageStats();
  setupBoard();
  setupLightPoint();
}


// Board loop function
void loop() {

  if (!mLightPoint->isConnected()) {
    mLightPoint->reconnect();
  }

  if(!mLightPoint->start()) {
    mLightPoint->reconnect();
  }

  delay(1000);
  while( !mLightPoint->isRegistered() ) {
      Serial.println("Waiting for device registration....");
      return;
  }

  mLightPoint->sendLumniousStatus();

}
