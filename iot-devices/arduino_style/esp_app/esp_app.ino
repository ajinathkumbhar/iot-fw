#include <FS.h>   //Include File System Headers
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define BUF_SIZE                16
#define user_config_initilizer  { {0}, {'E','S','P','8','2','6','6','\0'}, {0}, {0} }
#define FMT_FOOT_PRINT          "fmt_done.txt"
#define WIFI_CONF               "/wifi_conf.txt"

struct user_config {
  char status[BUF_SIZE];
  char dev_id[BUF_SIZE];
  char ssid[BUF_SIZE];
  char password[BUF_SIZE];
};

typedef user_config user_config_t;

//enum local_boolean {
//  true=1,
//  false=0
//}
bool user_spiffs_fmt() {
  if (!SPIFFS.format()) {
    Serial.println("File System Formatting Error");
    return false;
  }
  Serial.println("Spiffs formatted");
  File foot_print = SPIFFS.open(FMT_FOOT_PRINT, "w");
  if (!foot_print) {
    Serial.println("file open failed");
    return false;
  } 
  foot_print.println("Format Complete");
  foot_print.close();
  return true;
}

// Check file mount and fs format
bool is_user_spiffs_fmt() {
  if (!SPIFFS.exists(FMT_FOOT_PRINT)) {
    Serial.println("fs not found");
    return false;
  } 
  Serial.println("SPIFFS found");
  return true;
}

// fs init 
bool user_spiffs_init() {
  // Initialize File System
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Initialization...failed");
    return false;
  }
  Serial.println("SPIFFS Initialize....ok");

  if (!is_user_spiffs_fmt()){
    Serial.println("fs format required");
    if (!user_spiffs_fmt()) {
      Serial.println("fs init failed");
      return false;
    }
  }
  Serial.println("fs init done"); 
  return true;
}


void start_smart_config_setup(user_config_t * user_cfg) {
  char ssid[BUF_SIZE] = {0};
  char password[BUF_SIZE] = {0};

  Serial.println("");
  WiFi.mode(WIFI_STA);
  delay(500);

  Serial.print("smart config...");
  WiFi.beginSmartConfig();
  while (!WiFi.smartConfigDone()) {
    delay(1000);
    Serial.print(".");
  }

  strncpy(ssid,WiFi.SSID().c_str(),sizeof(ssid));
  strncpy(password,WiFi.psk().c_str(),sizeof(password));

  Serial.println("done!");
  Serial.print("ssid:");
  Serial.println(ssid);
  Serial.print("password:");
  Serial.println(password);

  Serial.print("connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("done!");
  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  strncpy(user_cfg->ssid,ssid,sizeof(ssid));
  strncpy(user_cfg->password,password,sizeof(password));
  
}

int load_user_config_from_flash(user_config_t * user_cfg) {
  //Read File data
  File f = SPIFFS.open(WIFI_CONF, "r");
  if (!f) {
    Serial.println("file open failed");
    return false;
  }
  
  f.readBytes(user_cfg->status,BUF_SIZE);
  f.readBytes(user_cfg->dev_id,BUF_SIZE);
  f.readBytes(user_cfg->ssid,BUF_SIZE);
  f.readBytes(user_cfg->password,BUF_SIZE);
  f.close();  //Close file

  Serial.print("status: ");
  Serial.println(user_cfg->status[0]);
  Serial.print("device ID: ");
  Serial.println(user_cfg->dev_id);
  Serial.print("SSID     : ");
  Serial.println(user_cfg->ssid);
  Serial.print("Password : ");
  Serial.println(user_cfg->password);
  Serial.println(".........");
  delay(1000);
  return true;
}


int load_user_config_to_flash(user_config_t * user_cfg) {
  File f = SPIFFS.open(WIFI_CONF, "w");
  if (!f) {
    Serial.println("file open failed");
    return false;
  }
  //Write data to file
  Serial.println("storing wifi configuration");
  f.write((uint8_t *)user_cfg->status,(size_t)BUF_SIZE);
  f.write((uint8_t *)user_cfg->dev_id,(size_t)BUF_SIZE);
  f.write((uint8_t *)user_cfg->ssid,(size_t)BUF_SIZE);
  f.write((uint8_t *)user_cfg->password,(size_t)BUF_SIZE);
  f.close();  
  delay(1000);
  return true;
}

void wifi_setup() {
  user_config_t user_cfg = user_config_initilizer;
  if (load_user_config_from_flash(&user_cfg) != true ) {
      Serial.println("Err : load_user_wifi_config.......fail");
  }

  if (user_cfg.status[0] != 0xff) {
    Serial.println("Err : user config corroupted, need to reconfigure.......fail");
    start_smart_config_setup(&user_cfg);
    user_cfg.status[0] = 0xff;
    load_user_config_to_flash(&user_cfg);
    return;
  } 
  
  WiFi.begin(user_cfg.ssid, user_cfg.password);
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  
}

void setup() {
  Serial.begin(115200);
  // spiffs setup
  if (!user_spiffs_init()) {
      Serial.println("failed to setup file system");
  }
  wifi_setup();
  Serial.println("setup.......ok");
}

void loop() {
  // put your main code here, to run repeatedly:

}
