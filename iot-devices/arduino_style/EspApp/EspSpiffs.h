#ifndef HEADER_USER_SPIFFS
#define HEADER_USER_SPIFFS

#define FMT_FOOT_PRINT          "fmt_done.txt"

class EspSpiffs {
  private:
    bool eraseAndFormat(void);
    bool isFormatted(void);

  public:
    EspSpiffs();
    bool init(void);
    void dumpStorageStats(void);
};
#endif
