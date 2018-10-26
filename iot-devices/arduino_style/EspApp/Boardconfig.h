#ifndef HEADER_BOARDCONFIG
#define HEADER_BOARDCONFIG

#include "Config.h"

class Boardconfig {
  public:
    char status[BUF_SIZE]; //= {0};
    char dev_id[BUF_SIZE]; //= {0};
    char ssid[BUF_SIZE]; //= {0};
    char password[BUF_SIZE];// = {0};
    Boardconfig();

};
#endif
