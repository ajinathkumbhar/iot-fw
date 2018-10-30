#ifndef HEADER_ESP8266BOARDCONFIG
#define HEADER_ESP8266BOARDCONFIG

#include "Boardconfig.h"
#include "EspLight.h"
#include "Utils.h"

#define BOARD_CONF    "/config.txt"

class Esp8266Boardconfig {
  private:
    char status;
    String deviceId;
    String ssid;
    String password;
    Utils mUtils;
    EspLight * mOnBoardLED;
    bool initDone;
    bool startSmartConfig(void);
  public:
    Esp8266Boardconfig();
    bool setDeviceId(String id);
    bool setSsid(String ssid);
    bool setPassword(String password);
    bool loadConfigFromFlash(void);
    bool loadConfigToFlash(void);
    void doWifiSetup(void);
    bool isWifiConfigured(void);
    bool factoryReset(bool configReset);
    String getDeviceId(void);
    char getStatus(void);
    void setStatus(char st);
    bool wifiConnect(String ssid, String password);
    bool isInternetServiceAvailable(void);
};
#endif
