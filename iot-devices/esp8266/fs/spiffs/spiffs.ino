/*
   ESP8266 SPIFFS Basic Reading and Writing File

*/

#include <ESP8266WiFi.h>
#include <FS.h>   //Include File System Headers

const char* fmt_foot_print = "fmt_done.txt";
const char* wificonf = "/wifi_conf.txt";

// format and create foot print
bool fs_fmt() {
  if (!SPIFFS.format()) {
    Serial.println("File System Formatting Error");
    return false;
  }
  Serial.println("Spiffs formatted");
  File foot_print = SPIFFS.open(fmt_foot_print, "w");
  if (!foot_print) {
    Serial.println("file open failed");
    return false;
  } 
  foot_print.println("Format Complete");
  foot_print.close();
  return true;
}

// Check file mount and fs format
bool is_fs_fmt() {
  if (!SPIFFS.exists(fmt_foot_print)) {
    Serial.println("fs not found");
    return false;
  } 
  Serial.println("SPIFFS found");
  return true;
}

// fs init 
bool fs_init() {
  // Initialize File System
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Initialization...failed");
    return false;
  }
  Serial.println("SPIFFS Initialize....ok");

  if (!is_fs_fmt()){
    Serial.println("fs format required");
    if (!fs_fmt()) {
      Serial.println("fs init failed");
      return false;
    }
  }
  Serial.println("fs init done"); 
  return true;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  if (!fs_init()) {
    Serial.println("failed to setup file system");
  }
  
  File f = SPIFFS.open(wificonf, "w");
  if (!f) 
    Serial.println("file open failed");
  else
  {
    //Write data to file
    Serial.println("storing wifi configuration");
    f.print("ssid:TX2");
    f.print("password:TX2XXXX");
    f.close();  
  }
}

bool setup_wifi_conf() {

}

void loop() {
  int i;

  
  //Read File data
  File f = SPIFFS.open(wificonf, "r");
  
  if (!f) {
    Serial.println("file open failed");
  }
  else
  {
    Serial.println("Reading Data from File:");
    //Data from file
    for (i = 0; i < f.size(); i++) //Read upto complete file size
    {
      Serial.print((char)f.read());
    }
    f.close();  //Close file
    Serial.println("File Closed");
  }
  delay(5000);
}
