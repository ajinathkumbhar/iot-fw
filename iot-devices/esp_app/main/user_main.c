/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>

#include "esp_system.h"
#include <esp_log.h>
#include "wifi_config.h"
#include "string.h"
#include "spiffs_params.h"

static const char *TAG = "esp_app";
/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main(void)
{
    spiffs_config_t spiffs_cfg = spiffs_config_initializer;
    int ret;

    spiffs_cfg.start_offset = 0x8c000;
    spiffs_cfg.size = FS1_FLASH_SIZE;

    ESP_LOGI(TAG,"--------------------------------------");
    ESP_LOGI(TAG,"SDK version:%s\n", esp_get_idf_version());
    ESP_LOGI(TAG,"--------------------------------------");

    ret = user_spiffs_fs_init(spiffs_cfg);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG, "Failed to user_spiffs_fs_init ");
    } else
        ESP_LOGI(TAG, "spiffs init.....ok");

    user_config_t user_cfg = user_config_initializer;
    strcpy(user_cfg.device_id,"AD4545");
    strcpy(user_cfg.ssid,"SmartEmployee");
    strcpy(user_cfg.password,"TronX@$#");

    ret = set_user_config(user_cfg);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG, "set_user_config.....fail");
    } else
        ESP_LOGI(TAG, "set_user_config.....ok");

    user_config_t user_cfg_1 = user_config_initializer;
    user_cfg_1 = get_user_config();
    ESP_LOGI(TAG,"magic     : %s", user_cfg_1.magic);
    ESP_LOGI(TAG,"device_id : %s", user_cfg_1.device_id);
    ESP_LOGI(TAG,"ssid      : %s", user_cfg_1.ssid);
    ESP_LOGI(TAG,"password  : %s", user_cfg_1.password);

}
