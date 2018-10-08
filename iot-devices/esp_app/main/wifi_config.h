#define WIFI_CONF "wifi.conf"
#define wifi_config_initializer \
    {                         \
        {0}, { 0 }            \
    }

#define spiffs_config_initializer \
    {                         \
        0x0, 0                 \
    }

#define FS_MAGIC_NUM 0x8266


enum boolean {
    FALSE=0,
    TRUE,
};

struct wifi_config
{
    char ssid[20];
    char password[20];
};

typedef struct wifi_config wifi_config_t;

struct spiffs_config {
    int start_offset;
    int size;
};

typedef struct spiffs_config spiffs_config_t;


int user_spiffs_fs_init(spiffs_config_t fs_config);
int user_spiffs_fs_format(void);
int user_spiffs_write(wifi_config_t * wcfg);
int user_spiffs_read(wifi_config_t * wcfg);

int set_wifi_config(wifi_config_t cfg);
wifi_config_t get_wifi_config(void);