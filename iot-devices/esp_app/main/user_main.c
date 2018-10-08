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
    wifi_config_t hw_network = wifi_config_initializer;
    wifi_config_t read_back = wifi_config_initializer;

    spiffs_cfg.start_offset = FS1_FLASH_ADDR;
    spiffs_cfg.size = FS1_FLASH_SIZE;

    char * ssid = "test_ssid\0";
    char * pass = "test_password\0";

    strcpy(hw_network.ssid,ssid);
    strcpy(hw_network.password,pass);

    user_spiffs_fs_init(spiffs_cfg);
    set_wifi_config(hw_network);
    read_back = get_wifi_config();

    char ssid_n[20] = {0};
    char pass_n[20] = {0};

    strcpy(ssid_n,read_back.ssid);
    strcpy(pass_n,read_back.password);

    // ESP_LOGI(TAG, " ssid : %s  password : %s\n", ssid_n, pass_n);
    ESP_LOGI(TAG,"SDK version:%s\n", esp_get_idf_version());
}
