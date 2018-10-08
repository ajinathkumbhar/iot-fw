


python /home/ajinath/0001-old-home/ajinath/work/00_Me/github/iot-fw/esp8266ex/toolchain-sdk/ESP8266_RTOS_SDK/components/esptool_py/esptool/esptool.py 
--chip esp8266 \
--port /dev/ttyUSB0  \
--baud 115200 \
--before default_reset \
0-after no_reset write_flash -z \
--flash_mode qio --flash_freq 40m \
--flash_size 4MB \
0x0000 /home/ajinath/0001-old-home/ajinath/work/00_Me/github/iot-fw/esp8266ex/esp_app/build/bootloader/bootloader.bin \
0x10000 /home/ajinath/0001-old-home/ajinath/work/00_Me/github/iot-fw/esp8266ex/esp_app/build/project_template.bin \
0x8000 /home/ajinath/0001-old-home/ajinath/work/00_Me/github/iot-fw/esp8266ex/esp_app/build/partitions_singleapp.bin \


## clear wifi conf
WIFI_CONF_OFFSET=0x8c000 ;
python /home/ajinath/0001-old-home/ajinath/work/00_Me/github/iot-fw/esp8266ex/toolchain-sdk/ESP8266_RTOS_SDK/components/esptool_py/esptool/esptool.py \
--chip esp8266 \
--port /dev/ttyUSB0 \
--baud 115200 \
--before default_reset \
--after no_reset write_flash \
-z --flash_mode qio \
--flash_freq 40m \
--flash_size 4MB \
$WIFI_CONF_OFFSET /home/ajinath/0001-old-home/ajinath/work/00_Me/github/iot-fw/esp8266ex/zero-256K.bin

