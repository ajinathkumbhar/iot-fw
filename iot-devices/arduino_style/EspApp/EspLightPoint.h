#ifndef HEADER_ESPLIGHTPOINT
#define HEADER_ESPLIGHTPOINT

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "Esp8266Boardconfig.h"

#define SIZE_256       256

#define TOPIC_LP_LUMNIOUS_STATUS    "LP/ESP8266-C705/lp_hall/lumnious/STATUS"
#define TOPIC_LP_LUMNIOUS_ACTION    "LP/ESP8266-C705/lp_hall/lumnious/ACTION"
#define TOPIC_LP_POWER_ACTION       "LP/ESP8266-C705/lp_hall/power/ACTION"

class EspLightPoint {
    private:
        WiFiClient espWifiClient;
        PubSubClient mqttClient;
        Esp8266Boardconfig mBoard;
        String hostname;
        String mqttClientId;
        int port;

        void setLpPowerState(String msg);
        void setLpLumnious(String msg);
        bool factoryReset(String msg);
    public:
        EspLightPoint(String deviceId);
        void messageCallback(String topic, byte* message, unsigned int length);
        bool setGatewayServer(String host, int port);
        bool setupCallback(void);
        bool connectToGateway(void);
        bool isConnected(void);
        bool sendRegistrationReq(void);
        bool isRegistered(void);
        bool doSubcription(void);
        bool start(void);
        void reconnect(void);
        void sendLumniousStatus(void);

};

#endif
