#ifndef HEADER_CONFIG
#define HEADER_CONFIG

#define BUF_SIZE                16


#define TOPIC_REG_REQ       "esp8266/RegReq"
#define TOPIC_REG_RES       "esp8266/RegRes/"
#define TOPIC_FACTORY_RESET "esp8266/reset"

// topic for ac controller
#define TOPIC_AC_TEMP_STATUS     "esp8266/ac/temperature/status"
#define TOPIC_AC_TEMP_ACTION     "esp8266/ac/temperature/action"
#define TOPIC_AC_POWER           "esp8266/ac/power/action"

#define MQTT_BROKER_HOST  "iot.eclipse.org"
#define MQTT_BROKER_PORT  1883


#endif
