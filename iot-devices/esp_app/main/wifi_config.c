#include "wifi_config.h"
#include "esp_log.h"
#include "spiffs_params.h"
#include "esp_spiffs.h"
#include "spi_flash.h"
#include "esp_libc.h"

static const char *TAG = "wifi_config";

spiffs_config_t mSpiffs_config = spiffs_config_initializer;
int force_format = FALSE;

int user_spiffs_fs_format() {
    char magic_num[FS_MAGIC_SIZE] = fs_magic_num_initializer;
    char fdr_check[FS_FDR_CHK_SIZE] = fs_fdr_check_initializer;
    int erase_sector_count = mSpiffs_config.size / SECTOR_SIZE;
    int erase_sector_addr = 0x00;
    int ret;

    // Erase sector from FS1_ERASE_ADDR
    for ( int i = 0; i < erase_sector_count ; i++ ) {
        erase_sector_addr = FS1_ERASE_ADDR ;//+ ( i * SECTOR_SIZE) ;
        ESP_LOGI(TAG,"spi_flash_erase_sector with %02x",erase_sector_addr);
        ret = spi_flash_erase_sector(erase_sector_addr);
        if ( ret != ESP_OK ) {
            ESP_LOGE(TAG,"spi_flash_erase_sector fail err: %8x",ret);
        }
    }
    // Write magic number
    ret = spi_flash_write(FS_MAGIC_OFFSET, magic_num, FS_MAGIC_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"magic setup fail err: %8x",ret);
    }
    // Write fdr check bit
    ret = spi_flash_write(FS_FDR_CHK_OFFSET, fdr_check, FS_FDR_CHK_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"fdr check setup fail err: %8x",ret);
    }

    return ret;
}


int user_spiffs_fs_init(spiffs_config_t fs_config) {
    char magic_num[FS_MAGIC_SIZE] = {0};
    esp_err_t ret;
    mSpiffs_config = fs_config;

    if ( mSpiffs_config.size < SECTOR_SIZE) {
        mSpiffs_config.size = SECTOR_SIZE;
    }

    ret = spi_flash_read(FS_MAGIC_OFFSET,magic_num, FS_MAGIC_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"magic read fail err: %8x",ret);
    }

    if ( force_format || strncmp(magic_num,"82@",3) != 0 ) {
        ESP_LOGI(TAG,"fs not initilized ... format required");
        ret = user_spiffs_fs_format();
        if ( ret != ESP_OK ) {
            ESP_LOGE(TAG,"user_spiffs_fs_format fail err: %8x",ret);
        }
    } else
        ESP_LOGI(TAG,"FS initilized format not required");

    ret = spi_flash_read(FS_MAGIC_OFFSET,magic_num, FS_MAGIC_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"magic read fail err: %8x",ret);
    }

    if (strncmp(magic_num,"82@",3) != 0 ) {
        ESP_LOGI(TAG,"Failed to initilized fs");
        ret = ESP_FAIL;
    }
    return ret;
}


int set_user_config(user_config_t * user_cfg) {
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG,"user_cfg.mgic  : %s",user_cfg->magic);
    ESP_LOGI(TAG,"user_cfg.dev   : %s",user_cfg->device_id);
    ESP_LOGI(TAG,"user_cfg.ssd   : %s",user_cfg->ssid);
    ESP_LOGI(TAG,"user_cfg.pass  : %s",user_cfg->password);

    ESP_LOGI(TAG,"FS_MAGIC_OFFSET   : %8x",FS_MAGIC_OFFSET);
    ESP_LOGI(TAG,"FS_FDR_CHK_OFFSET : %8x",FS_FDR_CHK_OFFSET);
    ESP_LOGI(TAG,"DEV_ID_OFFSET,    : %8x",DEV_ID_OFFSET);
    ESP_LOGI(TAG,"SSID_OFFSET,      : %8x",SSID_OFFSET);
    ESP_LOGI(TAG,"PASSWORD_OFFSET   : %8x",PASSWORD_OFFSET);



    ret = spi_flash_write(FS_MAGIC_OFFSET, user_cfg->magic,FS_MAGIC_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"magic write fail err: %8x",ret);
    }

    ret = spi_flash_write(FS_FDR_CHK_OFFSET, user_cfg->fdr_check, FS_FDR_CHK_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"fdr write fail err: %8x",ret);
    }

    ret = spi_flash_write(DEV_ID_OFFSET, user_cfg->device_id, DEV_ID_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"device id write fail err: %8x",ret);
    }

    ret = spi_flash_write(SSID_OFFSET, user_cfg->ssid, SSID_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"ssid write fail err: %8x",ret);
    }

    ret = spi_flash_write(PASSWORD_OFFSET, user_cfg->password, PASSWORD_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"password write fail err: %8x",ret);
    }

    return ret;
}

user_config_t get_user_config() {
    user_config_t user_cfg = user_config_initializer;
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG,"FS_MAGIC_OFFSET : %8x",FS_MAGIC_OFFSET);
    ESP_LOGI(TAG,"DEV_ID_OFFSET,  : %8x",DEV_ID_OFFSET);
    ESP_LOGI(TAG,"SSID_OFFSET,    : %8x",SSID_OFFSET);
    ESP_LOGI(TAG,"PASSWORD_OFFSET : %8x",PASSWORD_OFFSET);

    ret = spi_flash_read(FS_MAGIC_OFFSET, user_cfg.magic,FS_MAGIC_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"magic read fail err: %8x",ret);
    }

    ret = spi_flash_read(FS_FDR_CHK_OFFSET, user_cfg.fdr_check, FS_FDR_CHK_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"fdr read fail err: %8x",ret);
    }

    ret = spi_flash_read(DEV_ID_OFFSET, user_cfg.device_id, DEV_ID_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"device id read fail err: %8x",ret);
    }

    ret = spi_flash_read(SSID_OFFSET, user_cfg.ssid, SSID_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"ssid read fail err: %8x",ret);
    }

    ret = spi_flash_read(PASSWORD_OFFSET, user_cfg.password, PASSWORD_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG,"password read fail err: %8x",ret);
    }

    return user_cfg;
}

