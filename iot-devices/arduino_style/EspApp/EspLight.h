#ifndef HEADER_ESPLIGHT
#define HEADER_ESPLIGHT

#define MAX_DURATION_TIMEOUT    60
#define LED_ONMODULE    2
#define LED_ONBOARD     16


class EspLight {
  private:
    int gpioPin;

  public:
    EspLight(int gpioPin);
    void turnOn();
    void turnOff();
    void blink(int delayMs);
    void wifiConnecting();
    void smartConfigConnecting();
    void mqttConnecting();
};

#endif
