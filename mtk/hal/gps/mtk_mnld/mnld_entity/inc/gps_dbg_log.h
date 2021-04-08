#ifndef __GPS_DBG_LOG_H__
#define __GPS_DBG_LOG_H__

#include "mtk_gps_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX
#define MAX(A,B) ((A)>(B)?(A):(B))
#endif
#ifndef MIN
#define MIN(A,B) ((A)<(B)?(A):(B))
#endif

#define LOG_FILE            "/data/misc/gps/gpsdebug.log"
#define LOG_FILE_PATH       "/data/misc/gps/"
#define PATH_SUFFIX         "mtklog/gpsdbglog/"
#define LOG_FILE_EXTEN_NAME ".nma"
#define LOG_FILE_WRITING_EXTEN_NAME ".nmac"
#define GPS_LOG_PERSIST_STATE "debug.gpsdbglog.enable"
#define GPS_LOG_PERSIST_PATH "debug.gpsdbglog.path"
#define GPS_LOG_PERSIST_FLNM "debug.gpsdbglog.file"

#define GPS_LOG_PERSIST_VALUE_ENABLE "1"
#define GPS_LOG_PERSIST_VALUE_DISABLE "0"
#define GPS_LOG_PERSIST_VALUE_NONE "none"

#define GPS_DBG_LOG_FILE_NUM_LIMIT 1000
#define MAX_DBG_LOG_FILE_SIZE      MIN(48*1024*1024,(g_dbglog_file_size_in_config<1024*1024?25*1024*1024:g_dbglog_file_size_in_config))
#define MAX_DBG_LOG_DIR_SIZE       MIN(512*1024*1024,MAX(g_dbglog_folder_size_in_config,MAX_DBG_LOG_FILE_SIZE*12))

enum {
    MTK_GPS_DISABLE_DEBUG_MSG_WR_BY_MNL = 0x00,
    MTK_GPS_ENABLE_DEBUG_MSG_WR_BY_MNL = 0x01,
    MTK_GPS_DISABLE_DEBUG_MSG_WR_BY_MNLD = 0x10,
    MTK_GPS_ENABLE_DEBUG_MSG_WR_BY_MNLD = 0x11,
};
// Task synchronization related type
typedef enum{
  MNLD_MUTEX_PINGPANG_WRITE = 0,
  MNLD_MUTEX_MAX
} mnld_mutex_enum;

int gps_dbg_log_thread_init();

int create_mtklogger2mnl_fd();

int mtklogger2mnl_hdlr(int fd);

INT32 gps_log_dir_check(char * dirname);

void gps_stop_dbglog_release_condition(void);

void mtklogger_mped_reboot_message_update(void);

//Rename gpsdebug name, .nmac to .nma
void gps_log_file_rename(char *filename_cur);

void gps_dbg_log_property_load(void);

void gps_dbg_log_exit_flush(int force_exit) ;

#ifdef __cplusplus
}
#endif

#endif



