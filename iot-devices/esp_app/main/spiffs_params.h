/*
 * spiffs_test_params.h
 *
 */

#ifndef __SPIFFS_TEST_PARAMS_H__
#define __SPIFFS_TEST_PARAMS_H__



#define FS1_FLASH_SIZE      0x2000      // 8KB
#define FS1_FLASH_ADDR      0x8c000
#define FS1_ERASE_ADDR      0x8c

#define SECTOR_SIZE         (4*1024)
#define LOG_BLOCK           (SECTOR_SIZE)
#define LOG_PAGE            (128)

#define FD_BUF_SIZE         32*4
#define CACHE_BUF_SIZE      (LOG_PAGE + 32)*8

#endif /* __SPIFFS_TEST_PARAMS_H__ */
