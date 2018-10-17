#include <FS.h>   //Include File System Headers
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define BUF_SIZE                16
#define user_config_initilizer  { {0}, {0}, {0}, {0} }
#define FMT_FOOT_PRINT          "fmt_done.txt"
#define WIFI_CONF               "/wifi_conf.txt"



// MQTT config
/*
 * Registration : 
 * Pub - esp8266/regRes/<dev_id>
 * Sub - esp8266/regReq  "<dev_id>" 
 *  
 * 
 * 
 * 
 * 
 */

#define TOPIC_REG_REQ     "esp8266/regReq"
#define TOPIC_REG_RES     "esp8266/regRes/"

// topic for ac controller
#define TOPIC_AC_TEMP_STATUS     "esp8266/ac/temperature/status"
#define TOPIC_AC_TEMP_ACTION     "esp8266/ac/temperature/action"
#define TOPIC_AC_POWER           "esp8266/ac/power/action"

const char* mqtt_broker_host = "iot.eclipse.org";
const uint16_t mqtt_broker_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
bool is_registered = false;
String mDevice_id;
String client_id;
// User configuration 
struct user_config {
  char status[BUF_SIZE];
  char dev_id[BUF_SIZE];
  char ssid[BUF_SIZE];
  char password[BUF_SIZE];
};

typedef user_config user_config_t;


/*
 * Logging
 */

/* spiffs format  */
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

/* Check file mount and fs format */
bool is_user_spiffs_fmt() {
  if (!SPIFFS.exists(FMT_FOOT_PRINT)) {
    Serial.println("fs not found");
    return false;
  }
  Serial.println("SPIFFS found");
  return true;
}

/* fs init */
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

/*
 * smart config setup 
 */
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

/* Get user config from flash */
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

/* Set user config in flash */
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


/* Wifi connection setup */
void wifi_setup() {
  user_config_t user_cfg = user_config_initilizer;
  String dev_id;
  if (load_user_config_from_flash(&user_cfg) != true ) {
      Serial.println("Err : load_user_wifi_config.......fail");
  }

  if (user_cfg.status[0] != 0xff) {
    Serial.println("Err : user config corroupted, need to reconfigure.......fail");
    start_smart_config_setup(&user_cfg);
    user_cfg.status[0] = 0xff;
    dev_id = get_device_id();
    strcpy(user_cfg.dev_id,dev_id.c_str());
    load_user_config_to_flash(&user_cfg);
    return;
  }
  mDevice_id = String(user_cfg.dev_id);
  Serial.println("User config found...");
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

/* Generate device id */
String get_device_id() {
  String dev_id = "ESP8266C-";   // Create a random MQTT client ID
  dev_id += String(random(0xfff), HEX);
  Serial.println("Dev id generated : ");
  Serial.print(dev_id);
  return dev_id;
}

/*
 * Device registration
 */

 void device_setup() {
  user_config_t user_cfg = user_config_initilizer;
  char payload[BUF_SIZE] = {0};
  String topic_res(TOPIC_REG_RES);
  topic_res += mDevice_id;

  if (load_user_config_from_flash(&user_cfg) != true ) {
      Serial.println("Err : device_setup .......fail");
  }

  if (user_cfg.status[1] == 0xff) {
      Serial.println("Device registerd...");
      return;
  }
  Serial.println("Device not registerd...");
  strncpy(payload,user_cfg.dev_id,sizeof(user_cfg.dev_id));

  client.publish(TOPIC_REG_REQ,payload);
  Serial.println("Device registration req sent...");
  Serial.print("Topic   : ");
  Serial.println(TOPIC_REG_REQ);
  Serial.print("payload : ");
  Serial.println(payload);

  Serial.print("Subscribe to : ");
  Serial.println(topic_res.c_str());
  client.subscribe(topic_res.c_str());

 }

/* MQTT message callback */
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  String reg_res_topic(TOPIC_REG_RES);  //"esp8266/regRes/"
  reg_res_topic += mDevice_id;
  String res_topic = topic.substring(0,reg_res_topic.length()); 
    
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  
  Serial.println();
  if (topic == reg_res_topic){
      Serial.println("Registration response received... ");
      if(messageTemp == "success"){
        Serial.println("Registration response success ... ");
        is_registered = true;
        
      }
  }
  
 
  /* If a message is received on the topic room/lamp, 
   you check if the message is either on or off. 
  Turns the lamp GPIO according to the message */
  
  if(topic==TOPIC_AC_TEMP_ACTION){
      Serial.print("Changing AC temperature to ");
      Serial.println(messageTemp);
  }

  if(topic==TOPIC_AC_POWER){
      if(messageTemp == "on"){
        Serial.print(" Turning AC On");
      }
      else if(messageTemp == "off"){
        Serial.print(" Turning AC Off");
      }
  }
   
  Serial.println();
}


/* This functions reconnects your ESP8266 to your MQTT broker
 * Change the function below if you want to subscribe to more topics with your ESP8266  
 */
 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (!client.connect(mDevice_id.c_str())) {
      Serial.println("connected");
      mqtt_subscribe_setup();
      break;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
   
}

/*
 * Publish message 
 */
void publish_message() {
  
  // Timers auxiliar variables
  long now = millis();
  long lastMeasure = 0;

  char payload[BUF_SIZE] = {0};
  long randNumber = random(300);
  bool need_to_pub = (now - lastMeasure) > 1000;
  
  if (!need_to_pub) 
    return;
  
  lastMeasure = now;
  
  /* Read temperature from sensor i.e gpio */
  float temp = randNumber;
  if (isnan(temp)) {
    Serial.println("Failed to read data from sensor!");
    return;
  }
  dtostrf(temp, 6, 2, payload);
  Serial.print("Publish : ");
  Serial.print(TOPIC_AC_TEMP_STATUS);
  Serial.print("   ");
  Serial.println(payload);
  client.publish(TOPIC_AC_TEMP_STATUS, payload);
}



/*
 *  Subcribe required topics
 */

void mqtt_subscribe_setup(){
  client.subscribe(TOPIC_AC_TEMP_ACTION);
  client.subscribe(TOPIC_AC_POWER);
}
/*
 * Mqtt setup
 */
void mqtt_setup() {
  client.setServer(mqtt_broker_host, mqtt_broker_port);
  client.setCallback(callback);
  if (!client.connected()) {
    client.connect(mDevice_id.c_str());
  }
  mqtt_subscribe_setup();
}

/*
 * Board setup 
 */
void setup() {
  Serial.begin(115200);
  // spiffs setup
  if (!user_spiffs_init()) {
      Serial.println("failed to setup file system");
  }
  Serial.println("user_spiffs_init.......ok");
  wifi_setup();
  Serial.println("wifi_setup.......ok");
  mqtt_setup();
  Serial.println("mqtt_setup.......ok");
  device_setup();
  Serial.println("device_setup.......ok");

  randomSeed(analogRead(0));
}

/*
 * Board loop function
 */
void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect(mDevice_id.c_str());

  delay(1000);
  if (!is_registered) {
      Serial.println("Waiting for device registration....");
      return;
  }
  
  publish_message();
  
 
}
