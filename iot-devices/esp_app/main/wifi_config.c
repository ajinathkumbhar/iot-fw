#include "wifi_config.h"
#include "esp_log.h"
#include "spiffs_params.h"
#include "esp_spiffs.h"
#include "spi_flash.h"
#include "esp_libc.h"

static const char *TAG = "wifi_config";

spiffs_config_t mSpiffs_config = spiffs_config_initializer;

int user_spiffs_fs_format() {
    int magic_num = FS_MAGIC_NUM;
    int erase_sector_count = mSpiffs_config.size / SECTOR_SIZE;
    int erase_sector_addr = 0x00;
    int ret;
    for ( int i = 0; i < erase_sector_count ; i++ ) {
        erase_sector_addr = i * SECTOR_SIZE + 0x8c;
        ESP_LOGI(TAG,"spi_flash_erase_sector with %02x",erase_sector_addr);
        ret = spi_flash_erase_sector(erase_sector_addr);
        ESP_LOGI(TAG,"user_spiffs_fs_format - spi_flash_erase_sector ret : %d",ret);

    }

    spi_flash_read(mSpiffs_config.start_offset, (void * )magic_num, sizeof(int));
    return TRUE;
}

int user_spiffs_write(wifi_config_t * wcfg) {
    int ret = spi_flash_write(mSpiffs_config.start_offset + 4, (void *)wcfg, sizeof(wifi_config_t));
    ESP_LOGI(TAG,"user_spiffs_write - spi_flash_write ret : %d",ret);
    return ret;
}

int user_spiffs_read(wifi_config_t * wcfg) {
    int ret = spi_flash_read(mSpiffs_config.start_offset + 4, (void *)wcfg, sizeof(wifi_config_t));
    ESP_LOGI(TAG,"user_spiffs_read - spi_flash_read ret : %d",ret);
    return ret;
}

int user_spiffs_fs_init(spiffs_config_t fs_config) {
    int magic_num = 0x0000;

    if ( fs_config.size < SECTOR_SIZE) {
        fs_config.size = SECTOR_SIZE;
    }

    mSpiffs_config = fs_config;
    spi_flash_read(mSpiffs_config.start_offset, (void *) magic_num, sizeof(magic_num));
    if ( magic_num != 0x8266 ) {
        ESP_LOGI(TAG,"fs not initilized ... format required");
        user_spiffs_fs_format();
    }

    spi_flash_read(fs_config.start_offset, (void *)magic_num, sizeof(magic_num));
    if ( magic_num != 0x8266 ) {
        ESP_LOGI(TAG,"Failed to initilized fs");
        return FALSE;
    }

    return TRUE;
}

int set_wifi_config(wifi_config_t wcfg) {
    int ret = user_spiffs_write(&wcfg);
    ESP_LOGI(TAG,"set_wifi_config - user_spiffs_write ret : %d",ret);
    return ret;

}

wifi_config_t get_wifi_config() {
    wifi_config_t wcfg = wifi_config_initializer;
    int ret = user_spiffs_read(&wcfg);
    ESP_LOGI(TAG,"set_wifi_config - user_spiffs_read ret : %d",ret);
    return wcfg;
}

// int user_spiffs_fs_init(spiffs_config_t fs_config);
// int user_spiffs_fs_format(void);
// int user_spiffs_write(wifi_config_t * wcfg);
// int user_spiffs_read(wifi_config_t * wcfg);

// int set_wifi_config(wifi_config_t cfg);
// wifi_config_t get_wifi_config(void);