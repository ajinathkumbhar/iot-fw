#ifndef HEADER_ESP8266BOARDCONFIG
#define HEADER_ESP8266BOARDCONFIG

#include "Boardconfig.h"

#define BOARD_CONF    "/config.txt"

class Esp8266Boardconfig : public Boardconfig {
  private:
    bool initDone;
    bool startSmartConfig(void);
  public:
    Esp8266Boardconfig();
    bool setDeviceId(char * id);
    bool setSsid(char * ssid);
    bool setPassword(char * password);
    bool loadConfigFromFlash(void);
    bool loadConfigToFlash(void);
    void doWifiSetup(void);
    bool isWifiConfigured(void);
    bool factoryReset(bool configReset);
};
#endif
