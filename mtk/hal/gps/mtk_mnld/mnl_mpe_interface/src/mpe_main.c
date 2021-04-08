/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
***************************************************************/
#ifndef MPE_MAIN_C
#define MPE_MAIN_C

#ifdef __cplusplus
  extern "C" {
#endif

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <utils/Log.h> // For Debug
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <math.h>
#include <cutils/properties.h>
#include "mpe_common.h"
#include "mpe_DR.h"
#include "mpe_sensor.h"
#include "mpe.h"
#include "data_coder.h"
#include "mtk_lbs_utility.h"
#include "mnld.h"

/*****************************************************************************
 * Define
 *****************************************************************************/
#ifdef LOGD
#undef LOGD
#endif
#ifdef LOGW
#undef LOGW
#endif
#ifdef LOGE
#undef LOGE
#endif
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "MPE_MA"
#include <cutils/sockets.h>
#include <log/log.h>     /*logging in logcat*/
#define LOGD(fmt, arg ...) ALOGD("%s: " fmt, __FUNCTION__ , ##arg)
#define LOGW(fmt, arg ...) ALOGW("%s: " fmt, __FUNCTION__ , ##arg)
#define LOGE(fmt, arg ...) ALOGE("%s: " fmt, __FUNCTION__ , ##arg)

#ifndef UNUSED
#define UNUSED(x) (x)=(x)
#endif

/*****************************************************************************
 * GLobal Variable
 *****************************************************************************/
static const char pMPEVersion[] = {MPE_VER_INFO,0x00};
unsigned char isUninit_SE = FALSE;
static MNL2MPE_AIDING_DATA mnl_latest_in;
static MNL2MPE_AIDING_DATA mnl_glo_in;
static MPE2MNL_AIDING_DATA mnl_glo_out;
static UINT32 gMPEConfFlag = 0;

//Mtklogger function
char mpe_debuglog_file_name[MPE_LOG_NAME_MAX_LEN];

static int64_t last_ttick = 0;

//check for baro validity
extern unsigned char isfirst_baro;

// se log
extern FILE *data_file_local;

static void mpe_sensor_stop_notify(void);

UINT32 mpe_sys_get_mpe_conf_flag(void) {
    return gMPEConfFlag;
}

void mpe_sys_read_mpe_conf_flag(void) {
    const char mpe_prop_path[] = "/system/vendor/etc/mpe.conf";
    char propbuf[512];
    UINT32 flag = 0;
    FILE *fp = fopen(mpe_prop_path, "rb");
    LOGD("%s\n", pMPEVersion);
    if(!fp) {
        LOGD("mpe config flag - can't open");
        gMPEConfFlag = 0;
        return;
    }
    while(fgets(propbuf, sizeof(propbuf), fp)) {
        if(strstr(propbuf, "mpe_enable=1")) {
            flag |= MPE_CONF_MPE_ENABLE;
        } else if(strstr(propbuf, "print_rawdata=1")) {
            flag |= MPE_CONF_PRT_RAWDATA;
        } else if(strstr(propbuf, "auto_calib=1")) {
            flag |= MPE_CONF_AUTO_CALIB;
        }
    }
    LOGD("mpe config flag %u", flag);
    fclose(fp);
    gMPEConfFlag = flag;
    return;
}

INT32 mpe_sys_gps_to_sys_time( UINT32 gps_sec ) {
    return (INT32) ( gps_sec + DIFF_GPS_C_TIME );// difference between GPS and
}

void mpe_sys_get_time_stamp(double* timetag, UINT32 leap_sec) {
    struct tm tm_pt;
    struct timeval tv;
    //get second and usec
    gettimeofday(&tv, NULL);
    //convert to local time
    localtime_r(&tv.tv_sec, &tm_pt);
    (*timetag) = mktime(&tm_pt);
    (*timetag)= (*timetag) + leap_sec +((float)tv.tv_usec)/1000000;
}

void mnl2mpe_hdlr_init(void) {
    memset(&mnl_latest_in, 0 , sizeof(MNL2MPE_AIDING_DATA));
    memset(&mnl_glo_in, 0 , sizeof(MNL2MPE_AIDING_DATA));
    memset(&mnl_glo_out, 0 , sizeof(MPE2MNL_AIDING_DATA));
}

int mnl2mpe_hdlr(int fd) {
    char mnl2mpe_buff[MNL_MPE_MAX_BUFF_SIZE] = {0};
    char mpe2mnl_buff[MNL_MPE_MAX_BUFF_SIZE] = {0};
    int mnl2mpe_offset = 0;
    int mpe2mnl_offset = 0;
    int read_len;
    int log_rec = 0, rec_loc =0;
    UINT32 gps_sec = 0;
    UINT32 leap_sec = 18;
    MPE2MNL_AIDING_DATA mnl_out;
    INT8 accuracy[SENSOR_TYPE_MAX] = {-1};
    UINT32 type, length;

    read_len = safe_recvfrom(fd, mnl2mpe_buff, sizeof(mnl2mpe_buff));
    if (read_len <= 0) {
        LOGE("safe_recvfrom() failed read_len=%d", read_len);
        return -1;
    }

    type = get_int(mnl2mpe_buff, &mnl2mpe_offset);
    length = get_int(mnl2mpe_buff, &mnl2mpe_offset);
    LOGD("mpe recv, type = %u, len = %u", type, length);

    switch (type) {
        case CMD_START_MPE_REQ: {
            LOGD("%s\n", pMPEVersion);
            get_int(mnl2mpe_buff, &mnl2mpe_offset); //no_used
            get_int(mnl2mpe_buff, &mnl2mpe_offset); //no_used
            gps_sec = get_int(mnl2mpe_buff, &mnl2mpe_offset);
            leap_sec = get_int(mnl2mpe_buff, &mnl2mpe_offset);
            LOGD("gps_sec =%u, leap_sec=%u\n", gps_sec, leap_sec);
            put_int(mpe2mnl_buff, &mpe2mnl_offset, CMD_START_MPE_RES);
            put_int(mpe2mnl_buff, &mpe2mnl_offset, 0);
            mpe2mnl_hdlr((char*)&mpe2mnl_buff);
            mpe_sensor_update_mnl_sec(gps_sec, leap_sec);

            if(mpe_sensor_get_listen_mode() == MPE_IDLE_MODE) {
                mpe_sensor_run();
                LOGD("SE run = %d", mpe_sensor_get_listen_mode());
            } else {
                LOGD("SE already run");
            }
            break;
        }
        case CMD_STOP_MPE_REQ: {
            LOGD("trigger uninit se");
            last_ttick = 0;
            memset(&mnl_glo_in, 0 , sizeof(MNL2MPE_AIDING_DATA));
            memset(&mnl_latest_in, 0 , sizeof(MNL2MPE_AIDING_DATA));
            mpe_sensor_set_listen_mode(MPE_IDLE_MODE);
            if(!isUninit_SE) {
                LOGD("uninit se type, source");
                put_int(mpe2mnl_buff, &mpe2mnl_offset, CMD_STOP_MPE_RES);
                put_int(mpe2mnl_buff, &mpe2mnl_offset, 0);
                mpe2mnl_hdlr((char*)&mpe2mnl_buff);
            } else {
                isUninit_SE = FALSE;
                LOGD("uninit se type automatically \n");
            }
            break;
        }
        case CMD_GET_ADR_STATUS_REQ: {
            if(mpe_sensor_get_listen_mode() == MPE_START_MODE) {
                mpe_sensor_get_accuracy(accuracy);
                memcpy(&mnl_out, &mnl_glo_out, sizeof(MPE2MNL_AIDING_DATA));
                LOGD("mpe_flag, %d, %d, %d, %d, %d, %d, %d\n",
                    mnl_out.valid_flag[0], mnl_out.valid_flag[1], mnl_out.valid_flag[2], mnl_out.valid_flag[3], accuracy[0], accuracy[1], accuracy[2]);
                if(mnl_out.valid_flag[MTK_MPE_KER_FLAG] && mnl_out.valid_flag[MTK_MPE_RAW_FLAG] && accuracy[SENSOR_TYPE_GYR] >= 2) {
                    LOGD("mnl_out, %lf, %lf, %f, %f, %d, %f, %f\n",
                        mnl_out.latitude, mnl_out.longitude, mnl_out.velocity[0], mnl_out.bearing, mnl_out.staticIndex, mnl_out.barometerHeight, mnl_out.HACC);
                    put_int(mpe2mnl_buff, &mpe2mnl_offset, CMD_SEND_ADR_STATUS_RES);
                    put_binary(mpe2mnl_buff, &mpe2mnl_offset, (const char*)&mnl_out, sizeof(MPE2MNL_AIDING_DATA));
                    mpe2mnl_hdlr((char*)&mpe2mnl_buff);
                }
                if(mnl_out.valid_flag[MTK_MPE_RAW_FLAG] == 0) {
                    LOGD("MPE_DBG: No sensor raw data report. Init MPE kernel\n");
                    mpe_kernel_initialize();
                }
                mnl_glo_out.valid_flag[MTK_MPE_RAW_FLAG] = 0;
            }
            break;
        }
        case CMD_SET_GPS_AIDING_REQ: {
            if(length != sizeof(MNL2MPE_AIDING_DATA)) {
                LOGD("MPE_DBG: mnl_in except len = %d, receive len = %d", sizeof(MNL2MPE_AIDING_DATA), length);
                CRASH_TO_DEBUG();
            }
            memcpy(&mnl_latest_in,((INT8*)mnl2mpe_buff)+sizeof(MPE_MSG), length);
            LOGD("mnl_in, %lf, %lf, %lf %u %u\n", mnl_latest_in.latitude[0], mnl_latest_in.longitude[0], mnl_latest_in.altitude[0],
                mnl_latest_in.gps_sec, mnl_latest_in.leap_sec);

            gps_sec = mnl_latest_in.gps_sec;
            leap_sec = mnl_latest_in.leap_sec;

            if(data_file_local != NULL) {
                fprintf(data_file_local,"& %lf %lf %lf %lf %lf %lf %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %u %u\n",
                    mnl_latest_in.latitude[0], mnl_latest_in.longitude[0], mnl_latest_in.altitude[0],
                    mnl_latest_in.latitude[1], mnl_latest_in.longitude[1], mnl_latest_in.altitude[1],
                    mnl_latest_in.LS_velocity[0], mnl_latest_in.LS_velocity[1], mnl_latest_in.LS_velocity[2],
                    mnl_latest_in.KF_velocity[0], mnl_latest_in.KF_velocity[1], mnl_latest_in.KF_velocity[2],
                    mnl_latest_in.LS_velocitySigma[0], mnl_latest_in.LS_velocitySigma[1], mnl_latest_in.LS_velocitySigma[2],
                    mnl_latest_in.KF_velocitySigma[0], mnl_latest_in.KF_velocitySigma[1], mnl_latest_in.KF_velocitySigma[2],
                    mnl_latest_in.HACC, mnl_latest_in.VACC, mnl_latest_in.HDOP,
                    mnl_latest_in.confidenceIndex[0], mnl_latest_in.confidenceIndex[1], mnl_latest_in.confidenceIndex[2],
                    mnl_latest_in.gps_sec, mnl_latest_in.leap_sec);
            }

            if(mpe_sensor_get_listen_mode() == MPE_START_MODE) {
                mpe_sensor_update_mnl_sec(gps_sec, leap_sec);
                mpe_sensor_check_time();
                mpe_log_check_file();
                put_int(mpe2mnl_buff, &mpe2mnl_offset, CMD_SEND_GPS_AIDING_RES);
                put_int(mpe2mnl_buff, &mpe2mnl_offset, 0);
                mpe2mnl_hdlr((char*)&mpe2mnl_buff);
            } else {
                mpe_sensor_update_mnl_sec(gps_sec,leap_sec);
                mpe_sensor_run();
                LOGD("SE run = %d", mpe_sensor_get_listen_mode());
            }
            break;
        }
        case CMD_SEND_FROM_MNLD: {
            log_rec = get_int(mnl2mpe_buff, &mnl2mpe_offset);
            rec_loc = get_int(mnl2mpe_buff, &mnl2mpe_offset);
            get_binary(mnl2mpe_buff, &mnl2mpe_offset, mpe_debuglog_file_name);
            LOGD("log_rec =%d, rec_loc=%d, mpelog_path:%s", log_rec, rec_loc,mpe_debuglog_file_name );
            mpe_log_mtklogger_check((INT16)log_rec, mpe_debuglog_file_name, (INT8)rec_loc);
            break;
        }
        default: {
            LOGD("MPE_DBG: invalid msg type = %d", type);
        }
    }
    return 0;
}

void *mpe_calib_thread(void * arg) {
    int timeout_cnt = 0;
    INT8 accuracy[SENSOR_TYPE_MAX] = {0};

    UNUSED(arg);
    LOGD("MPE_calib_thread, create\n");

    sleep(10);
    mpe_sensor_init(SENSOR_USER_ID_CALIB);
    mpe_sensor_start(SENSOR_USER_ID_CALIB, SENSOR_TYPE_GYR, 5);
    while (1) {
        mpe_sensor_get_calib_accuracy(accuracy);
        LOGD("MPE_calib_thread, %d %d\n", timeout_cnt, accuracy[1]);
        if (accuracy[SENSOR_TYPE_GYR] == 3) {
            LOGD("MPE_calib_thread, acc ok\n");
            break;
        } else if (timeout_cnt > 60) {
            LOGD("MPE_calib_thread, timeout\n");
            break;
        } else if (mpe_sensor_get_calib_gyr_data() == false && timeout_cnt > 15) {
            LOGD("MPE_calib_thread, no gyro data report\n");
            break;
        }
        sleep(1);
        timeout_cnt++;
    }
    mpe_sensor_stop(SENSOR_USER_ID_CALIB, SENSOR_TYPE_GYR);
    mpe_sensor_deinit(SENSOR_USER_ID_CALIB);
    LOGD("MPE_calib_thread, exit\n");

    pthread_exit(NULL);
    return NULL;
}

void *mpe_sensor_thread(void * arg) {
    UNUSED(arg);
    LOGD("MPE_sensor_thread, create\n");

    mpe_sensor_init(SENSOR_USER_ID_MPE);
    mpe_sensor_start(SENSOR_USER_ID_MPE, SENSOR_TYPE_GYR, 5);
    mpe_sensor_start(SENSOR_USER_ID_MPE, SENSOR_TYPE_ACC, 5);
    mpe_sensor_start(SENSOR_USER_ID_MPE, SENSOR_TYPE_MAG, 200);
    mpe_sensor_start(SENSOR_USER_ID_MPE, SENSOR_TYPE_BAR, 20);

    while (1) {
        if (!mpe_sensor_check_mnl_response()) {
            break;
        }
        sleep(1);
    }
    mpe_sensor_stop(SENSOR_USER_ID_MPE, SENSOR_TYPE_ACC);
    mpe_sensor_stop(SENSOR_USER_ID_MPE, SENSOR_TYPE_GYR);
    mpe_sensor_stop(SENSOR_USER_ID_MPE, SENSOR_TYPE_MAG);
    mpe_sensor_stop(SENSOR_USER_ID_MPE, SENSOR_TYPE_BAR);
    mpe_sensor_deinit(SENSOR_USER_ID_MPE);
    mpe_sensor_stop_notify();
    LOGD("MPE_sensor_thread, exit\n");
    pthread_exit(NULL);
    return NULL;
}


int mpe_kernel_inject(IMU* data, UINT16 len, MPE2MNL_AIDING_DATA *mnl_out) {
    int i=0;
    int delta_t = 0;
    static float dr_compute_gyro_bias[3]={0.0f};

    if(data == NULL) {
        LOGD("allocate sensor cb error\n");
        return MTK_GPS_ERROR;
    }

    for(i = 0; i < len; i++ ) {
        if(data + i != NULL) {
            if(last_ttick == 0) {
                last_ttick = data[i].input_time_gyro;
                continue;
            }
            mpe_set_fusion_mode(SENSOR_FUSION_MODE_PDR);

            delta_t = (int)((float)(data[i].input_time_gyro - last_ttick)/1000.f);
            last_ttick = data[i].input_time_gyro;

            if (mpe_sys_get_mpe_conf_flag() & MPE_CONF_PRT_RAWDATA) {
                LOGD("[%d] MPErpy %lld %f %f %f %f %f %f %f %f %f %f %lf %lf %lf %lf %lf %lf %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %u %u",
                    i, (data+i)->input_time_gyro,
                    (data+i)->acceleration[0], (data+i)->acceleration[1], (data+i)->acceleration[2],
                    (data+i)->angularVelocity[0], (data+i)->angularVelocity[1], (data+i)->angularVelocity[2],
                    (data+i)->magnetic[0], (data+i)->magnetic[1], (data+i)->magnetic[2],
                    (data+i)->pressure,
                    mnl_glo_in.latitude[0], mnl_glo_in.longitude[0], mnl_glo_in.altitude[0],
                    mnl_glo_in.latitude[1], mnl_glo_in.longitude[1], mnl_glo_in.altitude[1],
                    mnl_glo_in.LS_velocity[0], mnl_glo_in.LS_velocity[1], mnl_glo_in.LS_velocity[2],
                    mnl_glo_in.KF_velocity[0], mnl_glo_in.KF_velocity[1], mnl_glo_in.KF_velocity[2],
                    mnl_glo_in.LS_velocitySigma[0], mnl_glo_in.LS_velocitySigma[1], mnl_glo_in.LS_velocitySigma[2],
                    mnl_glo_in.KF_velocitySigma[0], mnl_glo_in.KF_velocitySigma[1], mnl_glo_in.KF_velocitySigma[2],
                    mnl_glo_in.HACC, mnl_glo_in.VACC, mnl_glo_in.HDOP,
                    mnl_glo_in.confidenceIndex[0], mnl_glo_in.confidenceIndex[1], mnl_glo_in.confidenceIndex[2],
                    mnl_glo_in.gps_sec, mnl_glo_in.leap_sec);
            }
            (data+i)->angularVelocity[0] -= dr_compute_gyro_bias[0];
            (data+i)->angularVelocity[1] -= dr_compute_gyro_bias[1];
            (data+i)->angularVelocity[2] -= dr_compute_gyro_bias[2];
            if(!mpe_update_posture(data+i, delta_t)) {
                mpe_update_dead_reckoning(data+i, delta_t, &mnl_glo_in);
                mpe_get_dr_position(mnl_out); //absolute position
                mpe_get_dr_gyro_bias(dr_compute_gyro_bias);
            } else {
                if (mpe_sys_get_mpe_conf_flag() & MPE_CONF_PRT_RAWDATA) {
                    LOGD("MPE_DBG: MPE mpe_update_posture return false");
                }
            }
        } else {
            LOGD("accept null data\n");
        }
    }
    return MTK_GPS_SUCCESS;
}

void mpe_set_mnl_out_flag(MPE2MNL_AIDING_DATA *mnl_out) {
    if (mpe_sensor_get_listen_mode() == MPE_START_MODE) {
        mnl_out->valid_flag[MTK_MPE_SYS_FLAG] = 1;
    } else {
        mnl_out->valid_flag[MTK_MPE_SYS_FLAG] = 0;
    }
    mnl_out->valid_flag[MTK_MPE_RAW_FLAG] = 1;
    if(isfirst_baro == 1) {
        mnl_out->barometerHeight= -16000;
    }
}

void mpe_sensor_stop_notify( void ) {
    char mpe2mpe_buff[MNL_MPE_MAX_BUFF_SIZE] = {0};
    int offset = 0;
    put_int(mpe2mpe_buff, &offset, CMD_STOP_MPE_REQ);
    put_int(mpe2mpe_buff, &offset, sizeof(INT32));
    put_int(mpe2mpe_buff, &offset, 2);
    safe_sendto(MNLD_MPE_SOCKET, mpe2mpe_buff, MNL_MPE_MAX_BUFF_SIZE);
    isUninit_SE = TRUE;
    LOGD("send uninit request from listener \n");
}

void mpe_run_algo( void ) {
    int data_ret = 0;
    IMU SE_data[MAX_NUM_SAMPLES];
    UINT16 data_cnt = 0;
    MPE2MNL_AIDING_DATA mnl_out;

    memset(SE_data, 0 ,MAX_NUM_SAMPLES*sizeof(IMU));
    memset(&mnl_out, 0, sizeof(MPE2MNL_AIDING_DATA));

    data_cnt = mpe_sensor_acquire_Data(SE_data);
    if(data_cnt) {
        data_ret = mpe_kernel_inject(SE_data, data_cnt, &mnl_out);
        mpe_set_mnl_out_flag(&mnl_out);
        memcpy(&mnl_glo_out, &mnl_out, sizeof(MPE2MNL_AIDING_DATA));
    }
    memcpy(&mnl_glo_in, &mnl_latest_in, sizeof(MNL2MPE_AIDING_DATA));
}

void mpe_kernel_initialize(void) {
    mpe_re_initialize();
    mpe_dr_re_initialize();
}

#ifdef __cplusplus
  extern "C" }
#endif

#endif //#ifndef MPE_FUSION_MAIN_C
