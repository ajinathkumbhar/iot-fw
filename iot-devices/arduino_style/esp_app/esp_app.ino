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


// MQTT config
const char* mqtt_server = "iot.eclipse.org";
WiFiClient espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;


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

//MQTT message callback
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="esp8266/room/lamp"){
      Serial.print("Changing Room lamp to ");
      if(messageTemp == "on"){
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        Serial.print("Off");
      }
  }
  Serial.println();
}


// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("esp8266/room/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_setup() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

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
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
  static char payload[BUF_SIZE] = {0};
  long randNumber;
  randNumber = random(300);
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
  now = millis();

  if (now - lastMeasure > 1000) {
     lastMeasure = now;
    // Read temperature as Celsius (the default)
    float temp = randNumber;

    // Check if any reads failed and exit early (to try again).
    if (isnan(temp)) {
      Serial.println("Failed to read data from sensor!");
      return;
    }

    dtostrf(temp, 6, 2, payload);
    client.publish("esp8266/room/temperature", payload);

  }
}
