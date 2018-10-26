#include <Arduino.h>
#include <FS.h>
#include "EspSpiffs.h"
#include "Config.h"


EspSpiffs::EspSpiffs() {
  Serial.println("start spiffs init");
}

void EspSpiffs::dumpStorageStats(void) {
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  Serial.println("-----------------------------");
  Serial.println("SPIFFS stat");
  Serial.println("-----------------------------");
  Serial.print("totalBytes    : ");
  Serial.println(fs_info.totalBytes);
  Serial.print("usedBytes     : ");
  Serial.println(fs_info.usedBytes);
  Serial.print("blockSize     : ");
  Serial.println(fs_info.blockSize);
  Serial.print("pageSize      : ");
  Serial.println(fs_info.pageSize);
  Serial.print("maxOpenFiles  : ");
  Serial.println(fs_info.maxOpenFiles);
  Serial.print("maxPathLength : ");
  Serial.println(fs_info.maxPathLength);
  Serial.println("-----------------------------");
}

/* spiffs format  */
bool EspSpiffs::eraseAndFormat() {
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
bool EspSpiffs::isFormatted() {
  if (!SPIFFS.exists(FMT_FOOT_PRINT)) {
    Serial.println("fs not found");
    return false;
  }
  Serial.println("SPIFFS found");
  return true;
}

/* fs init */
bool EspSpiffs::init() {
  // Initialize File System
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Initialization...failed");
    return false;
  }
  Serial.println("SPIFFS Initialize....ok");

  if (!isFormatted()){
    Serial.println("fs format required");
    if (!eraseAndFormat()) {
      Serial.println("fs init failed");
      return false;
    }
  }
  Serial.println("EspSpiffs.......ok");
  return true;
}
  
