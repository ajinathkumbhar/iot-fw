#ifndef HEADER_BOARDCONFIG
#define HEADER_BOARDCONFIG

#include "Config.h"

class Boardconfig {
  public:
    char status;
    String deviceId;
    String ssid;
    String password;
    Boardconfig();

};
#endif
