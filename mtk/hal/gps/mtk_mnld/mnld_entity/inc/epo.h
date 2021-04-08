#ifndef __EPO_H__
#define __EPO_H__

#include "curl.h"
#ifdef __cplusplus
extern "C" {
#endif

#define EPO_FILE                    "/data/misc/gps/EPO.DAT"
#define EPO_UPDATE_FILE             "/data/misc/gps/EPOTMP.DAT"
#define EPO_UPDATE_HAL              "/data/misc/gps/EPOHAL.DAT"
#define MTK_EPO_ONE_SV_SIZE  72
#define SECONDS_PER_HOUR (60*60)
#define EPO_DL_MAX_RETRY_TIME 3  //The max retry time of curl_easy_download fail
#define EPO_INVALIDE_DL_MAX_RETRY_TIME 3  //The max retry time of whole download process
#define EPO_INVALIDE_DL_RETRY_SLEEP (100*1000)  //100 ms
#define MAX_EPO_PIECE 10
#define EPO_MERGE_FULL_FILE   -1
#define EPO_MD5_AVAILABLE_BIT   (1<<0)
#define EPO_DAT_AVAILABLE_BIT   (1<<1)
#define EPO_FILE_NAME_MAX_SIZE  60
#define EPO_MD5_FILE_MAX_SIZE   50
#define GPS_EPO_FILE_LEN  32
#define GPS_EPO_URL_LEN 256

typedef struct EPO_Status {
    unsigned int EPO_piece_flag[MAX_EPO_PIECE];
    int last_DL_Date;
    int today_retry_time;
} EPO_Status_T;

int epo_downloader_init();
int epo_read_cust_config();
int epo_downloader_is_file_invalid();
int epo_downloader_start();
void epo_update_epo_file();
int epo_is_wifi_trigger_enabled();
int epo_is_epo_download_enabled();
CURLcode curl_easy_download(char* url, char* filename);
int mtk_gps_sys_epo_period_start(int fd, unsigned int* u4GpsSecs, time_t* uSecond);
int mtk_gps_sys_epo_bd_period_start(int fd, unsigned int* u4GpsSecs, time_t* uSecond);
void GpsToUtcTime(int i2Wn, double dfTow, time_t* uSecond);
void getEpoUrl(char * filename, char * url);

#ifdef __cplusplus
}
#endif

#endif



