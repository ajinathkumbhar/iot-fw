#ifndef __WIFI_CONF_H__
#define __WIFI_CONF_H__

#define WIFI_CONF "wifi.conf"


#define FS_MAGIC_SIZE       4
#define FS_FDR_CHK_SIZE     4
#define DEV_ID_SIZE         16
#define SSID_SIZE           16
#define PASSWORD_SIZE       16
#define TOTAL_SIZE          (FS_MAGIC_SIZE + DEV_ID_SIZE + SSID_SIZE + PASSWORD_SIZE )
#define FS_MAGIC_OFFSET     FS1_FLASH_ADDR
#define FS_FDR_CHK_OFFSET   ( FS_MAGIC_OFFSET + FS_MAGIC_SIZE + 1 )
#define DEV_ID_OFFSET       ( FS_FDR_CHK_OFFSET + FS_FDR_CHK_SIZE + 1 )
#define SSID_OFFSET         ( DEV_ID_OFFSET + DEV_ID_SIZE + 1 )
#define PASSWORD_OFFSET     ( SSID_OFFSET + SSID_SIZE + 1 )


#define fs_magic_num_initializer    {'8', '2', '@', '\0'}
#define fs_fdr_check_initializer    {'F', '1', '\0'}
#define fs_fdr_check1_initializer   {'F', '0', '\0'}
#define user_config_initializer     { fs_magic_num_initializer, fs_fdr_check_initializer, {'A','B','9'}, {0}, {0} }
#define wifi_config_initializer     { {0}, {0} }
#define spiffs_config_initializer   { 0x0, 0 }

enum boolean {
    FALSE=0,
    TRUE,
};

struct spiffs_config {
    int start_offset;
    int size;
};

typedef struct spiffs_config spiffs_config_t;

struct user_config {
    char magic[FS_MAGIC_SIZE];
    char fdr_check[FS_FDR_CHK_SIZE];
    char device_id[DEV_ID_SIZE];
    char ssid[SSID_SIZE];
    char password[PASSWORD_SIZE];
};

typedef struct user_config user_config_t;


int user_spiffs_fs_init(spiffs_config_t fs_config);
int user_spiffs_fs_format(void);

int set_user_config(user_config_t * user_cfg);
user_config_t get_user_config();

#endif