#include "wifi_config.h"
#include "esp_log.h"
#include "spiffs_params.h"

static const char *TAG = "wifi_config";

int user_spiffs_fs_format(spiffs_config_t fs_config) {
    int magic_num = FS_MAGIC_NUM;
    int erase_sector_count = fs_config / SECTOR_SIZE;
    int erase_sector_add = 0x00;
    for ( int i = 0; i < erase_sector_count ; i++ ) {
        erase_sector_addr = i * SECTOR_SIZE + fs_config.offset;
        ESP_LOGI(TAG,"spi_flash_erase_sector with %02x\n",erase_sector_addr);
        ret = spi_flash_erase_sector(erase_sector_addr);
    }

    spi_flash_read(fs_config.start_offset, magic_num, sizeof(magic_num));

    return TRUE;
}

int user_spiffs_write(wifi_config_t);
int user_spiffs_read(wifi_config_t) {

}

int user_spiffs_fs_init(spiffs_config_t fs_config) {
    int magic_num = 0x0000;

    if ( fs_config.size < SECTOR_SIZE) {
        fs_config.size = SECTOR_SIZE;
    }

    spi_flash_read(fs_config.start_offset, magic_num, sizeof(magic_num));
    if ( magic_num != 0x8266 ) {
        ESP_LOGI(TAG,"fs not initilized ... format required");
        user_spiffs_fs_format(fs_config);
    }

    spi_flash_read(fs_config.start_offset, magic_num, sizeof(magic_num));
    if ( magic_num != 0x8266 ) {
        ESP_LOGI(TAG,"Failed to initilized fs");
        return FALSE;
    }

}


int set_wifi_config(wifi_config_t cfg);
wifi_config_t get_wifi_config(void);



        spi_flash_erase_sector(0x8c);
        spi_flash_write(0x8c000, user, sizeof(user));
        spi_flash_write(0x8C020, password, sizeof(password));


    spi_flash_read(0x8c000, user, sizeof(user));
    spi_flash_read(0x8C020, password, sizeof(password));

