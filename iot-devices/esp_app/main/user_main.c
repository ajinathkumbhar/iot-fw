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
#include "smartconfig.h"
#include "spi_flash.h"
#include "esp8266/rom_functions.h"

static const char *TAG = "esp_app";

static EventGroupHandle_t wifi_conn_group;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;

int is_user_cfg_reset() {
    char fdr_check[FS_FDR_CHK_SIZE] = {0};
    esp_err_t ret = spi_flash_read(FS_FDR_CHK_OFFSET,fdr_check,FS_FDR_CHK_SIZE);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG, "fdr check.....fail");
        return FALSE;
    } else
        ESP_LOGI(TAG, "fdr check : %s.....ok",fdr_check);

    if ( strncmp(fdr_check,"F1",2) == 0 ) {
        ESP_LOGE(TAG, "Device resetted before");
        return TRUE;
    }

    return FALSE;
}

int load_user_cfg_to_flash(user_config_t * user_cfg) {
    ESP_LOGI(TAG,"-------load_user_cfg_to_flash------- ");
    ESP_LOGI(TAG,"magic     : %s", user_cfg->magic);
    ESP_LOGI(TAG,"fdr       : %s", user_cfg->fdr_check);
    ESP_LOGI(TAG,"device_id : %s", user_cfg->device_id);
    ESP_LOGI(TAG,"ssid      : %s", user_cfg->ssid);
    ESP_LOGI(TAG,"password  : %s", user_cfg->password);
    ESP_LOGI(TAG,"----------------------------------- ");

    esp_err_t ret = set_user_config(user_cfg);
    if ( ret != ESP_OK ) {
        ESP_LOGE(TAG, "set_user_config.....fail");
    } else
        ESP_LOGI(TAG, "set_user_config.....ok");

    return ret;

}

int load_user_cfg_from_flash(user_config_t * user_cfg) {
    user_config_t usr_cfg = user_config_initializer;
    if ( is_user_cfg_reset() != TRUE ) {
        usr_cfg = get_user_config();

        strncpy(user_cfg->magic, usr_cfg.magic, sizeof(usr_cfg.magic));
        strncpy(user_cfg->fdr_check, usr_cfg.fdr_check, sizeof(usr_cfg.fdr_check));
        strncpy(user_cfg->device_id, usr_cfg.device_id, sizeof(usr_cfg.device_id));
        strncpy(user_cfg->ssid, usr_cfg.ssid, sizeof(usr_cfg.ssid));
        strncpy(user_cfg->password, usr_cfg.password, sizeof(usr_cfg.password));

        ESP_LOGI(TAG,"-------load_user_cfg_from_flash------- ");
        ESP_LOGI(TAG,"magic     : %s", user_cfg->magic);
        ESP_LOGI(TAG,"fdr       : %s", user_cfg->fdr_check);
        ESP_LOGI(TAG,"device_id : %s", user_cfg->device_id);
        ESP_LOGI(TAG,"ssid      : %s", user_cfg->ssid);
        ESP_LOGI(TAG,"password  : %s", user_cfg->password);
        ESP_LOGI(TAG,"----------------------------------- ");
        return TRUE;
    }

    ESP_LOGI(TAG,"--------------------------------------");
    ESP_LOGI(TAG,"User config not found, need to reconfigure");
    ESP_LOGI(TAG,"-");
    ESP_LOGI(TAG,"1. Take wifi enabled android phone");
    ESP_LOGI(TAG,"2. Connect to wifi network");
    ESP_LOGI(TAG,"3. Open EspTouch app");
    ESP_LOGI(TAG,"4. Enter password of wifi network and confirm");
    ESP_LOGI(TAG,"5. If configuration take long time then reboot iot device");
    ESP_LOGI(TAG,"   and try again from step-1");
    ESP_LOGI(TAG,"--------------------------------------");
    return FALSE;


}


esp_err_t spiffs_init() {
    spiffs_config_t spiffs_cfg = spiffs_config_initializer;
    esp_err_t ret;

    spiffs_cfg.start_offset = FS1_FLASH_ADDR;
    spiffs_cfg.size = FS1_FLASH_SIZE;

    ret = user_spiffs_fs_init(spiffs_cfg);
    if ( ret != ESP_OK )
        ESP_LOGE(TAG, "Failed to user_spiffs_fs_init ");
    else
        ESP_LOGI(TAG, "spiffs init.....ok");

    return ret;
}

/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main(void)
{
    ESP_LOGI(TAG,"--------------------------------------");
    ESP_LOGI(TAG,"SDK version :%s\n", esp_get_idf_version());
    ESP_LOGI(TAG,"--------------------------------------");

    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK( spiffs_init() );

    wifi_conn_group = xEventGroupCreate();


    user_config_t user_cfg = user_config_initializer;
    char fdr_check[FS_FDR_CHK_SIZE] = fs_fdr_check1_initializer;
    esp_err_t ret;
    EventBits_t uxBits;

    xEventGroupCreate();
    if ( load_user_cfg_from_flash(&user_cfg) != TRUE ) {
        user_cfg = initialise_wifi(&wifi_conn_group);
        while (1) {
                ESP_LOGI(TAG, "Waiting....");
                uxBits = xEventGroupWaitBits(wifi_conn_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 
                if(uxBits & ESPTOUCH_DONE_BIT) {
                    ESP_LOGI(TAG, "smartconfig done");
                    break;
                }
        }

        ESP_LOGI(TAG,"------ wait -user_cfg form initialise_wifi ------- ");
        ESP_LOGI(TAG,"magic     : %s", user_cfg.magic);
        ESP_LOGI(TAG,"fdr       : %s", user_cfg.fdr_check);
        ESP_LOGI(TAG,"device_id : %s", user_cfg.device_id);
        ESP_LOGI(TAG,"ssid      : %s", user_cfg.ssid);
        ESP_LOGI(TAG,"password  : %s", user_cfg.password);
        ESP_LOGI(TAG,"----------------------------------- ");

        strncpy(user_cfg.fdr_check,fdr_check,3);
        strncpy(user_cfg.ssid,"oiweQQQwureouiqwer",15);
        strncpy(user_cfg.password, "1234asfd@#",15);


        ret = load_user_cfg_to_flash(&user_cfg);
        if ( ret != ESP_OK ) {
            ESP_LOGE(TAG, "load_user_cfg_to_flash......fail");
        }
    }

    if ( load_user_cfg_from_flash(&user_cfg) != TRUE ) {
        ESP_LOGE(TAG, "load_user_cfg_from_flash......fail");
    }

    ESP_LOGI(TAG,"---Done---");
}
