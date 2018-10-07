
# IoT fw

- ESP8266EX board

![alt text](https://github.com/ajinathkumbhar/iot-fw/blob/master/esp8266ex/documents/Ndemcu_module-500x500.JPG)

Setup toolchain and sdk
```sh
$ cd iot-fw/esp8266ex/toolchain-sdk
$ chmod +x setup.sh
$ setup.sh
```
Setup environment
```sh
$ cd iot-fw/esp8266ex/toolchain-sdk
$ source build/envsetup.sh
```

Build hello world
```sh
$ cd iot-fw/esp8266ex/toolchain-sdk
$ source build/envsetup.sh
$ cd ESP8266_RTOS_SDK/examples/get-started/project_template
$ make menuconfig
$ make
```
connect ESP8266 board and flash
```sh
$ make flash
```
The ESP8266 board has default baud rate 74880
So, you need to set baud rate on host machine to get
serial logs from esp board

Open gtkterm in one terminal
```sh
$ sudo gtkterm
```
select /dev/ttyUSB0 and baudrate 115200
Open another terminal and change baudrate with folloing command
```sh
$ setup-custom-baudrate /dev/ttyUSB0 74880
```

# Reference

[Espressif](https://www.espressif.com/en/products/hardware/esp-wroom-02/overview)

[Espressif github](https://github.com/espressif)
