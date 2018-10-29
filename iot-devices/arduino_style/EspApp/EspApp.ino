//Include File System Headers
#include <FS.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include "EspSpiffs.h"
#include "Esp8266Boardconfig.h"
#include "EspLightPoint.h"
#include "Config.h"
#include "Utils.h"


EspSpiffs mSpiffs;
Esp8266Boardconfig mBoardconfig;
Utils mUtils;
EspLightPoint * mLightPoint;

void ICACHE_RAM_ATTR sendLumniousStatus() {
  mLightPoint->sendLumniousStatus();
  timer1_write(1);
}

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
  mLightPoint->start();
  timer1_disable();
  timer1_attachInterrupt(sendLumniousStatus);
  timer1_isr_init();
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(1);
  Serial.println("setupLightPoint.......ok");
}

// ESP board setup
void setupBoard() {
  #ifndef USE_FIX_SSID
  mBoardconfig.doWifiSetup();
  #else
  mBoardconfig.wifiConnect(FIX_SSID, FIX_PASSWORD);
  #endif
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

}
