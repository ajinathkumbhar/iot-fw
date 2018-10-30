#include <Arduino.h>
#include "EspLightPoint.h"
#include "Config.h"

EspLightPoint::EspLightPoint(String deviceId) {
    Serial.println("EspLightPoint constructer");
    mqttClient.setClient(espWifiClient);
    mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->messageCallback(topic, payload, length); });
    mqttClientId = deviceId;
    mBoard.loadConfigFromFlash();
}

bool EspLightPoint::setGatewayServer(String hostname, int port) {
    this->hostname = hostname;
    this->port = port;
    mqttClient.setServer(this->hostname.c_str(),this->port);
}

bool EspLightPoint::connectToGateway() {
    if (!mqttClient.connected()) {
        mqttClient.connect(mqttClientId.c_str());
    }
}

bool EspLightPoint::isConnected() {
     if (!mqttClient.connected()) {
        return false;
    }
    return true;
}

bool EspLightPoint::doSubcription() {
    Serial.println(" Subscribed to :");
    Serial.println(TOPIC_LP_LUMNIOUS_ACTION);
    Serial.println(TOPIC_LP_POWER_ACTION);
    Serial.println(TOPIC_FACTORY_RESET);

    mqttClient.subscribe(TOPIC_LP_LUMNIOUS_ACTION);
    mqttClient.subscribe(TOPIC_LP_POWER_ACTION);
    mqttClient.subscribe(TOPIC_FACTORY_RESET);
}


bool EspLightPoint::sendRegistrationReq() {
    char payload[BUF_SIZE] = {0};
    String topicRegRes(TOPIC_REG_RES);
    topicRegRes += mqttClientId;

    if (isRegistered()) {
        Serial.println("EspLightPoint already registerd");
        return true;
    }

    Serial.println("EspLightPoint not registerd");
    mqttClient.publish(TOPIC_REG_REQ,mqttClientId.c_str());
    Serial.println("Device registration req sent...");
    Serial.print("Topic   : ");
    Serial.println(TOPIC_REG_REQ);
    Serial.print("payload : ");
    Serial.println(mqttClientId.c_str());

    Serial.print("Subscribe to : ");
    Serial.println(topicRegRes.c_str());
    mqttClient.subscribe(topicRegRes.c_str());
}

bool EspLightPoint::isRegistered() {
    bool result = mBoard.getStatus() & 0x02;
    return result;
}

void EspLightPoint::reconnect() {
     // Loop until we're reconnected
    Serial.print("reconnect required ... ");
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (!mqttClient.connect(mqttClientId.c_str())) {
            Serial.println("connected");
            doSubcription();
            break;
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void EspLightPoint::sendLumniousStatus() {
  if ( !isRegistered() ) {
      Serial.println("Waiting for device registration....");
      return;
  }

 long now = millis();
 long lastMeasure = 0;
 char payload[BUF_SIZE] = {0};
 long randNumber = random(300);
 bool need_to_pub = (now - lastMeasure) > 1000;

 if (!need_to_pub)
   return;

 lastMeasure = now;

 /* Read temperature from sensor i.e gpio */
 float temp = randNumber;
 if (isnan(temp)) {
   Serial.println("Failed to read data from sensor!");
   return;
 }
 dtostrf(temp, 6, 2, payload);

 mqttClient.publish(TOPIC_LP_LUMNIOUS_STATUS, payload);
 Serial.print("Publish : ");
 Serial.print(TOPIC_LP_LUMNIOUS_STATUS);
 Serial.print("   ");
 Serial.println(payload);
}

bool EspLightPoint::setupCallback() {
}

void EspLightPoint::messageCallback(String topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    // Registration response
    String topicRegRes(TOPIC_REG_RES);  //"esp8266/regRes/"
    topicRegRes += mqttClientId;
    String res_topic = topic.substring(0,topicRegRes.length());

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }

    Serial.println();
    if (topic == topicRegRes){
        Serial.println("Registration response received... ");
        if(messageTemp == "success") {
            Serial.println("Registration response success ... ");
            mBoard.setStatus(mBoard.getStatus() | 0x02);
            mBoard.loadConfigToFlash();
        }
    }

    if(topic == TOPIC_LP_LUMNIOUS_ACTION) {
        setLpLumnious(messageTemp);
    }


    if(topic == TOPIC_LP_POWER_ACTION){
        setLpPowerState(messageTemp);
    }

    if( topic == TOPIC_FACTORY_RESET )
        factoryReset(messageTemp);

    Serial.println();
}


void EspLightPoint::setLpPowerState(String msg) {
    if(msg == "on"){
        Serial.print(" Turning Lightpoint On");
    }
    else if(msg == "off"){
        Serial.print(" Turning Lightpoint Off");
    }
}

void EspLightPoint::setLpLumnious(String msg) {
    Serial.print("Changing brightness to ");
    Serial.println(msg);
}

bool EspLightPoint::start(void) {
    if(!mqttClient.loop()) {
        return false;
    }
    return true;
}

bool EspLightPoint::factoryReset(String msg) {
    if(msg == "yes"){
        Serial.print(" Do factory reset");
        mBoard.factoryReset(true);
    }
}