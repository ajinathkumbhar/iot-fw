/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "wifi_config.h"

void smartconfig_example_task(void * parm);
esp_err_t event_handler(void *ctx, system_event_t *event);
user_config_t initialise_wifi(EventGroupHandle_t * wifi_conn_group);
void sc_callback(smartconfig_status_t status, void *pdata);
void smartconfig_example_task(void * parm);

