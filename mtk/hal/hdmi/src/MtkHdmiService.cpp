/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "MtkHdmiService"
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <utils/String16.h>
#include <utils/threads.h>
#include <cutils/properties.h>
#include "MtkHdmiService.h"
#include "event/hdmi_event.h"

#include <errno.h>
#include <utils/misc.h>

#include "linux/hdmitx.h"

#if defined (MTK_DRM_KEY_MNG_SUPPORT)
#include "keyblock.h"
#endif

#define HDMI_ENABLE "sys.hdmi.enable"
#define HDCP_ENABLE "persist.sys.hdcp.enable"
#define HDMI_VIDEO_AUTO "persist.sys.hdmi.auto"
#define HDMI_VIDEO_RESOLUTION "persist.sys.hdmi.resolution"
#define HDMI_COLOR_SPACE "persist.sys.hdmi.color_space"
#define HDMI_DEEP_COLOR "persist.sys.hdmi.deep_color"
#define HDMI_edid "persist.sys.hdmi.edid"
#define HDMI_SERVICE_START "service.hdmiservice.start"


#define EDIDNUM 4
#define AUTO_RESOLUTION 100
/**
 * HDMI resolution definition
 */
#define HDMI_VIDEO_720x480i_60Hz 0
#define HDMI_VIDEO_720x576i_50Hz 1
#define RESOLUTION_720X480P_60HZ 2
#define RESOLUTION_720X576P_50HZ 3
#define RESOLUTION_1280X720P_60HZ 4
#define RESOLUTION_1280X720P_50HZ 5
#define RESOLUTION_1920X1080I_60HZ 6
#define RESOLUTION_1920X1080I_50HZ 7
#define RESOLUTION_1920X1080P_30HZ 8
#define RESOLUTION_1920X1080P_25HZ 9
#define RESOLUTION_1920X1080P_24HZ 10
#define RESOLUTION_1920X1080P_23HZ 11
#define RESOLUTION_1920X1080P_29HZ 12
#define RESOLUTION_1920X1080P_60HZ 13
#define RESOLUTION_1920X1080P_50HZ 14
#define RESOLUTION_1280X720P3D_60HZ 15
#define RESOLUTION_1280X720P3D_50HZ 16
#define RESOLUTION_1920X1080I3D_60HZ 17
#define RESOLUTION_1920X1080I3D_50HZ 18
#define RESOLUTION_1920X1080P3D_24HZ 19
#define RESOLUTION_1920X1080P3D_23HZ 20

#define RESOLUTION_3840X2160P23_976HZ 21
#define RESOLUTION_3840X2160P_24HZ 22
#define RESOLUTION_3840X2160P_25HZ 23
#define RESOLUTION_3840X2160P19_97HZ 24
#define RESOLUTION_3840X2160P_30HZ 25
#define RESOLUTION_4096X2161P_24HZ 26

#define RESOLUTION_3840X2160P_60HZ 27
#define RESOLUTION_3840X2160P_50HZ 28
#define RESOLUTION_4096X2161P_60HZ 29
#define RESOLUTION_4096X2161P_50HZ 30

#define SINK_YCBCR_420 1<<11
#define SINK_YCBCR_420_CAPABILITY 1<<12

static Mutex mLock;
sp<HdmiUEventThread> event_thread;

int getValue(char* key, char* defValue) {
    char buf[PROPERTY_VALUE_MAX];
    int len = property_get(key,buf,defValue);
    ALOGD("getValue: %s, %s" , key, buf);
    return (atoi(buf));
}
int setValue(char* key, int value) {
    char buf[PROPERTY_VALUE_MAX];
    sprintf(buf,"%d",value);
    int ret = property_set(key,buf);
    ALOGD("setValue: %s, %s" , key, buf);
    return ret;
}

namespace android
{
static int hdmi_ioctl(int code, unsigned long value);
int enableHDMI(int enable);
int enableHDMIInit(int enable);
int enableHDCP(int value);
int setHdcpKey(char* key);
bool setDrmKey();
int getCapabilities();
int getDisplayType();
int* getResolutionMask();
int setVideoResolution(int res);

//////////////hdmi auto resolution///////////
static int mEdid[3];
static int DEFAULT_RESOLUTIONS[4] = {RESOLUTION_1920X1080P_30HZ,RESOLUTION_1920X1080P_60HZ,RESOLUTION_1280X720P_60HZ,RESOLUTION_720X480P_60HZ};
static int DEFAULT_ALL_RESOLUTIONS[24] = { RESOLUTION_4096X2161P_24HZ,
                RESOLUTION_3840X2160P_30HZ,RESOLUTION_3840X2160P19_97HZ,
                RESOLUTION_3840X2160P_25HZ,RESOLUTION_3840X2160P_24HZ,
                RESOLUTION_3840X2160P23_976HZ,RESOLUTION_1920X1080P_60HZ,
                RESOLUTION_1920X1080P_50HZ, RESOLUTION_1920X1080P_30HZ,
                RESOLUTION_1920X1080P_25HZ, RESOLUTION_1920X1080P_24HZ,
                RESOLUTION_1920X1080P_23HZ, RESOLUTION_1920X1080I_60HZ,
                RESOLUTION_1920X1080I_50HZ, RESOLUTION_1280X720P_60HZ,
                RESOLUTION_1280X720P_50HZ, RESOLUTION_720X480P_60HZ,
                RESOLUTION_720X576P_50HZ };

static int PREFERED_RESOLUTIONS[22]{
                    RESOLUTION_4096X2161P_50HZ,
                    RESOLUTION_4096X2161P_60HZ,
                    RESOLUTION_3840X2160P_50HZ,
                    RESOLUTION_3840X2160P_60HZ,
                    RESOLUTION_4096X2161P_24HZ,
                    RESOLUTION_3840X2160P_30HZ,
                    RESOLUTION_3840X2160P19_97HZ,
                    RESOLUTION_3840X2160P_25HZ,
                    RESOLUTION_3840X2160P_24HZ,
                    RESOLUTION_3840X2160P23_976HZ,
                    RESOLUTION_1920X1080P_60HZ,
                    RESOLUTION_1920X1080P_50HZ,
                    RESOLUTION_1920X1080P_30HZ,
                    RESOLUTION_1920X1080P_25HZ,
                    RESOLUTION_1920X1080P_24HZ,
                    RESOLUTION_1920X1080P_23HZ,
                    RESOLUTION_1920X1080I_60HZ,
                    RESOLUTION_1920X1080I_50HZ,
                    RESOLUTION_1280X720P_60HZ,
                    RESOLUTION_1280X720P_50HZ,
                    RESOLUTION_720X480P_60HZ,
                    RESOLUTION_720X576P_50HZ };

static int sResolutionMask[15] = {0,0,SINK_480P, SINK_576P,
        SINK_720P60, SINK_720P50, SINK_1080I60, SINK_1080I50, SINK_1080P30,
        SINK_1080P25, SINK_1080P24, SINK_1080P23976, SINK_1080P2997,
        SINK_1080P60, SINK_1080P50 };

#if defined (MTK_INTERNAL_HDMI_SUPPORT)
static int sResolutionMask_4k2k[10] = { SINK_2160P_23_976HZ, SINK_2160P_24HZ,
    SINK_2160P_25HZ, SINK_2160P_29_97HZ, SINK_2160P_30HZ, SINK_2161P_24HZ,
    SINK_2160P_60HZ, SINK_2160P_50HZ, SINK_2161P_60HZ, SINK_2161P_50HZ};
#endif

bool mHdmiStateChanged= false;

void MtkHdmiService::refreshEdid() {
    int* edid = getResolutionMask();
    int edidTemp;
    //int preEdid = getValue(HDMI_edid,"0")
    ALOGI("refresh edid[0] %d" , *edid);
    ALOGI("refresh edid[1] %d" , *(edid +1));

    //ALOGI("refresh preEdid %d" , preEdid);
    mEdid[0] = *edid;
    mEdid[1] = *(edid +1);
    mEdid[2] = *(edid +2);
    mHdmiStateChanged = true;
    setVideoResolution(getValue(HDMI_VIDEO_RESOLUTION,"100"));
    setValue(HDMI_SERVICE_START, 1);
}

int* getSupportedResolutions() {
    if (0 == mEdid[0] && 0 == mEdid[1] && 0 == mEdid[2]) {
        return DEFAULT_RESOLUTIONS;
    }
    return DEFAULT_ALL_RESOLUTIONS;
}

bool isResolutionSupport(int resolution) {
    ALOGI("isResolutionSupported: %d ", resolution);
    if (resolution >= AUTO_RESOLUTION) {
        return false;
    }
    int* supportedResolutions = getSupportedResolutions();
    for (int i =0 ;i < sizeof(supportedResolutions)/sizeof(int); i++) {
        if (*(supportedResolutions +i) == resolution) {
            return true;
        }
    }
    return false;
}

int getSuitableResolution(int resolution) {
    ALOGI("getSuitableResolution: %d ",resolution);
    int SuitableResolution = resolution;
    if (mEdid[0]!= 0 || mEdid[1]!= 0 || mEdid[2]!= 0 ) {
        int edidTemp = mEdid[0] | mEdid[1];
        int edidTemp_4k2k = mEdid[2];
        ALOGI("getSuitableResolution edidTemp: %d ",edidTemp);
        ALOGI("getSuitableResolution edidTemp_4k2k: %d ",edidTemp_4k2k);
        int* prefered = PREFERED_RESOLUTIONS;
        for (int i = 0; i < 22; i++) {
            int act = *(prefered + i);
            ALOGI("getSuitableResolution act: %d ",act);
            if(act < RESOLUTION_3840X2160P23_976HZ){
               if (0 != (edidTemp & sResolutionMask[act])) {
                    SuitableResolution = act;
                    ALOGI("getSuitableResolution resolution: %d ",SuitableResolution);
                    break;
              }
            }
        #if defined (MTK_INTERNAL_HDMI_SUPPORT)
            else{
                if (0 != (edidTemp_4k2k & sResolutionMask_4k2k[act - RESOLUTION_3840X2160P23_976HZ])) {
                    ALOGI("getSuitableResolution resolution 4k: %d ",SuitableResolution);
                    SuitableResolution = act;
                    break;
                }
            }
        #endif
        }
    } else {
        SuitableResolution = 2;
        ALOGI("getSuitableResolution edid==null,set solution to 480P60");
    }
    ALOGI("getSuitableResolution resolution final: %d ",SuitableResolution);
    return SuitableResolution;
}

////////////////////////////////////////////

bool setDrmKey() {
    bool ret = false;
#if defined (MTK_HDMI_SUPPORT)
#if defined (MTK_DRM_KEY_MNG_SUPPORT)
    ALOGI("[MtkHdmiService]setDrmKey\n");
    hdmi_hdcp_drmkey hKey;
    int i;
    int ret_temp = 0;
    unsigned char* enckbdrm = NULL;
    unsigned int inlength = 0;

    ret_temp = get_encrypt_drmkey(HDCP_1X_TX_ID,&enckbdrm,&inlength);
    if(ret_temp !=0 ) {
        ALOGI("[MtkHdmiService] setHDMIDRMKey get_encrypt_drmkey failed %d", ret_temp);
        return ret;
    }

    memcpy(hKey.u1Hdcpkey, (unsigned char*)enckbdrm, sizeof(hKey.u1Hdcpkey));
    ret = hdmi_ioctl(MTK_HDMI_HDCP_KEY, (long)&hKey);
    ALOGI("[MtkHdmiService]setHDMIDRMKey ret = %d\n",ret);
    free_encrypt_drmkey(enckbdrm);
#endif
#endif
    return ret;
}

void startObserving()
{
    event_thread = new HdmiUEventThread();
    if (event_thread == NULL)
    {
        ALOGE("Failed to initialize UEvent thread!!");
        abort();
    }
    int ret = event_thread->run("HdmiUEventThread");
    ALOGE("HdmiUEventThread run: %d", ret);
}

int initialize(void)
{
#if defined (MTK_MT8193_HDCP_SUPPORT)||defined (MTK_HDMI_HDCP_SUPPORT)

    ALOGI("[MtkHdmiService]hdmi initial()");
    #if defined (MTK_DRM_KEY_MNG_SUPPORT)
        setDrmKey();
        //ALOGI("[MtkHdmiService]setDrmKey: " + setDrmKey());
    #endif
    getCapabilities();
#endif
    startObserving();
    enableHDMIInit(0);
    enableHDMIInit(1);
    return 1;
}

int getCapabilities() {
    int result = 0;
#if defined (MTK_HDMI_SUPPORT)
    //if (hdmi_ioctl(MTK_HDMI_GET_CAPABILITY, (long)&result) == false) {
    //    result = 0;
    //}
#endif
    ALOGI("[MtkHdmiService]getCapabilities(%d)\n", result);
    return result;
}
int getDisplayType() {
    int result = 0;
#if defined (MTK_HDMI_SUPPORT)
   /* bool ret = false;
    mtk_dispif_info_t hdmi_info;
    memset((void *)&hdmi_info,0,sizeof(mtk_dispif_info_t));
    ret = hdmi_ioctl(MTK_HDMI_GET_DEV_INFO, (long)&hdmi_info);
    if (ret) {
        if (hdmi_info.displayType == HDMI_SMARTBOOK) {
            result = 1;
        } else if (hdmi_info.displayType == MHL) {
            result = 2;
        } else if (hdmi_info.displayType == SLIMPORT) {
            result = 3;
        }
    }*/
#endif
    ALOGI("[MtkHdmiService]getDisplayType(%d)\n", result);
    return result;
}

static int setDeepColor(int color, int deep) {
    int ret = -1;
#if defined (MTK_MT8193_HDMI_SUPPORT)||defined (MTK_INTERNAL_HDMI_SUPPORT)
    hdmi_para_setting h;
      h.u4Data1 = color;
      h.u4Data2 = deep;
      ret = hdmi_ioctl(MTK_HDMI_COLOR_DEEP, (long)&h);
      if (ret >= 0) {
          ALOGI("[MtkHdmiService]setDeepColor(%d,%d)\n", color, deep);
      }
#endif
    return ret;
}

//---------------------------hdmi implementation start-----------------------

static int hdmi_ioctl(int code, unsigned long value)
{
    int fd = open("/dev/hdmitx", O_RDONLY, 0);//O_RDONLY;O_RDWR
    int ret = -1;
    if (fd >= 0) {
        ret = ioctl(fd, code, value);
        if (ret < 0) {
            ALOGE("[MtkHdmiService] [%s] failed. ioctlCode: %d, errno: %d",
                 __func__, code, errno);
        }
        close(fd);
    } else {
        ALOGE("[MtkHdmiService] [%s] open hdmitx failed. errno: %d", __func__, errno);
    }
	ALOGI("[MtkHdmiService] [%s] lv ret: %d", __func__,ret);
    return ret;
}

int enableHDMIInit(int value) {
    ALOGI("[MtkHdmiService] enableHDMIInit = %d", value);
    //bool enable = (value) > 0 ? true : false;
    int ret = -1;
#if defined (MTK_HDMI_SUPPORT)
    ret = hdmi_ioctl(MTK_HDMI_AUDIO_VIDEO_ENABLE, value);
#endif
    if(ret >= 0 && value == 1)
    {
        setValue(HDMI_ENABLE, value);
    }

    return ret;
}


int enableHDMI(int value) {
    ALOGI("[MtkHdmiService] enableHDMI = %d", value);
    //bool enable = (value) > 0 ? true : false;
    int ret = -1;
#if defined (MTK_HDMI_SUPPORT)
    ret = hdmi_ioctl(MTK_HDMI_AUDIO_VIDEO_ENABLE, value);
#endif
    if(ret >= 0)
    {
        setValue(HDMI_ENABLE, value);
    }

    return ret;
}

int enableHDCP(int value) {
    ALOGI("[MtkHdmiService] enableHDCP = %d", value);
    bool enable = (value) > 0 ? true : false;
    int ret = -1;
#if defined (MTK_MT8193_HDCP_SUPPORT)||defined (MTK_HDMI_HDCP_SUPPORT)
    ret = hdmi_ioctl(MTK_HDMI_ENABLE_HDCP, (unsigned long)&enable);
#endif
    if(ret >= 0)
    {
        setValue(HDCP_ENABLE, value);
    }

    return ret;
}

#define HDMI_RES_MASK 0x80
#define LOGO_DEV_NAME "/dev/block/mmcblk0p9"

int MtkHdmiService::setVideoConfig(int vformat) {
    ALOGI("[MtkHdmiService] setVideoConfig = %d", vformat);
    int ret = -1;

#if defined (MTK_HDMI_SUPPORT)
    ret = hdmi_ioctl(MTK_HDMI_VIDEO_CONFIG, vformat);
#endif

#if defined (MTK_ALPS_BOX_SUPPORT)
    int fd = open(LOGO_DEV_NAME, O_RDWR);
    if (fd >= 0) {
        int hdmi_res = vformat|HDMI_RES_MASK;

        lseek(fd, -512 ,SEEK_END);

        ret = write(fd, (void*)&hdmi_res, sizeof(hdmi_res));
        ALOGI("[MtkHdmiService]setVideoConfig hdmi_res 0x%x",hdmi_res);
        if (ret < 0) {
            ALOGE("[MtkHdmiService] [%s] failed. ioctlCode: %d, errno: %d",
                __func__, vformat, errno);
        }
        close(fd);
    } else {
        ALOGE("[MtkHdmiService] [%s] open %s failed. errno:%d %s", __func__, LOGO_DEV_NAME, errno, strerror(errno));
    }
#endif
    return ret;
}

//-------------------------hdmi implementation end---------------------------------

MtkHdmiService::MtkHdmiService()
{
    ALOGD("[MtkHdmiService]MtkHdmiService()");
}

MtkHdmiService::~MtkHdmiService()
{
    ALOGD("[MtkHdmiService]~MtkHdmiService()");
    if (event_thread != NULL) {
        ALOGE("~MtkHdmiService requestExit");
        event_thread->requestExit();
        ALOGE("~MtkHdmiService requestExitAndWait");
        event_thread->requestExitAndWait();
        ALOGE("MtkHdmiService clear");
        event_thread = NULL;
        ALOGE("~uevent_thread done");
    }
}

void MtkHdmiService::initial()
{
    ALOGD("[MtkHdmiService]initial()");
    setValue(HDMI_SERVICE_START, 0);
    MtkHdmiService *mHdmiService = new MtkHdmiService();
    if(mHdmiService != NULL)
    {
        defaultServiceManager()->addService(
            String16("hdmi.MtkHdmiService"), mHdmiService);
    } else {
      ALOGD("[MtkHdmiService]service is null");
    }
    initialize();
}

void MtkHdmiService::registerClient(const sp<IMtkHdmiClient>& client)
{
    ALOGD("registerClient() %p, tid %d, calling tid %d", client.get(), gettid(), IPCThreadState::self()->getCallingPid());
    Mutex::Autolock _l(mLock);

    mClient = client;

    sp<IBinder> binder = client->asBinder(mClient);
    mClient->onHdmiEvent(100);
}


void MtkHdmiService::binderDied(const wp<IBinder>& who)
{
    ALOGD("[MtkHdmiService]binderDied() %p, tid %d, calling tid %d", who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());
    Mutex::Autolock _l(mLock);

    IBinder *binder = who.unsafe_get();

    if (binder != NULL)
    {

    }

  //  sp<IBinder> binder_hdmi = mClient->asBinder();
  //  if (binder == binder_hdmi.get())

}

int MtkHdmiService::enable_hdcp(int enable)
{
    Mutex::Autolock _l(mLock);

    ALOGD("[MtkHdmiService]enable_hdcp = %d" , enable);
    return enableHDCP(enable);
}

int MtkHdmiService::enable_hdmi(int enable)
{
    Mutex::Autolock _l(mLock);

    ALOGD("[MtkHdmiService]enable_hdcp = %d" , enable);
    return enableHDMI(enable);
}

int MtkHdmiService::set_video_resolution(int resolution)
{
    Mutex::Autolock _l(mLock);

    return setVideoResolution(resolution);
}
int setVideoResolution(int resolution) {
    if(getValue(HDMI_VIDEO_AUTO,"1") == 1)
        setValue(HDMI_VIDEO_AUTO, 1);

    ALOGI("[MtkHdmiService]set_video_resolution = %d" , resolution);
    int suitableResolution = resolution;
    if (resolution >= AUTO_RESOLUTION || getValue(HDMI_VIDEO_AUTO,"1") == 1) {
        suitableResolution = getSuitableResolution(resolution);
    }
    ALOGI("[MtkHdmiService]suitableResolution = %d" , suitableResolution);
    if (suitableResolution == getValue(HDMI_VIDEO_RESOLUTION,"100")) {
        ALOGI("setVideoResolution is the same");
        if (!mHdmiStateChanged) {
            ALOGI("setVideoResolution is the same return");
            return true;
        }
    }
    int finalResolution = suitableResolution >= AUTO_RESOLUTION ? (suitableResolution - AUTO_RESOLUTION)
                : suitableResolution;
    ALOGI("[MtkHdmiService]final video resolution = %d ", finalResolution);

    /*if (finalResolution == 27) {
        int* edid_temp = getResolutionMask();
        if (edid_temp[3] & SINK_YCBCR_420) {
            setDeepColor(4, 1);
        } else if (edid_temp[3] & SINK_YCBCR_420_CAPABILITY) {
            setDeepColor(3, 1);
        }
    } else {
        setDeepColor(3, 1);
    }*/
    setValue(HDMI_VIDEO_RESOLUTION, finalResolution);
    mHdmiStateChanged = false;
    int param = (finalResolution & 0xff);
    return MtkHdmiService::setVideoConfig(param);
}

int* MtkHdmiService::get_resolution_mask()
{
    Mutex::Autolock _l(mLock);
    return getResolutionMask();
}
int* getResolutionMask(){
    ALOGI("[MtkHdmiService]get_resolution_mask in");
    int ret = -1;
    int* cResult = new int[EDIDNUM];
#if defined (MTK_HDMI_SUPPORT)
    HDMI_EDID_T edid;
    ret = hdmi_ioctl(MTK_HDMI_GET_EDID, (long)&edid);
    ALOGI("[MtkHdmiService]hdmi_ioctl,ret = %d",ret);
    if (ret >= 0) {
        ALOGI("[MtkHdmiService] edid.ui4_ntsc_resolution %4X\n", edid.ui4_ntsc_resolution);
        ALOGI("[MtkHdmiService] edid.ui4_pal_resolution %4X\n", edid.ui4_pal_resolution);
        #if defined (MTK_INTERNAL_HDMI_SUPPORT)
        ALOGI("[MtkHdmiService] edid.ui4_sink_hdmi_4k2kvic %4X\n", edid.ui4_sink_hdmi_4k2kvic);
        #endif
        ALOGI("[MtkHdmiService] edid.ui2_sink_colorimetry %4X\n", edid.ui2_sink_colorimetry);

        cResult[0] = edid.ui4_ntsc_resolution;
        cResult[1] = edid.ui4_pal_resolution;
        #if defined (MTK_INTERNAL_HDMI_SUPPORT)
        cResult[2] = edid.ui4_sink_hdmi_4k2kvic;
        cResult[3] = edid.ui2_sink_colorimetry;
        #else
        cResult[2] = 0;
        cResult[3] = edid.ui2_sink_colorimetry;
        #endif
    }
#endif
    ALOGI("[MtkHdmiService]getEDID\n");
    return cResult;
}
int* MtkHdmiService::get_audio_capability()
{
    ALOGI("[MtkHdmiService]get_audio_capability in");
    int ret = -1;
    int* cResult = new int[EDIDNUM];
#if defined (MTK_HDMI_SUPPORT)
    HDMI_EDID_T edid;
    ret = hdmi_ioctl(MTK_HDMI_GET_EDID, (long)&edid);
    ALOGI("[MtkHdmiService]hdmi_ioctl,ret = %d",ret);
    if (ret >= 0) {
        ALOGI("[MtkHdmiService] edid.ui2_sink_aud_dec %4X\n", edid.ui2_sink_aud_dec);
        ALOGI("[MtkHdmiService] edid.ui4_hdmi_pcm_ch_type %4X\n", edid.ui4_hdmi_pcm_ch_type);
        ALOGI("[MtkHdmiService] edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type %4X\n", edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type);

        cResult[0] = edid.ui2_sink_aud_dec;
        cResult[1] = edid.ui4_hdmi_pcm_ch_type;
        cResult[2] = edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type;
    }
#endif
    ALOGI("[MtkHdmiService]getEDID\n");
    return cResult;
}
int MtkHdmiService::get_capability()
{
    Mutex::Autolock _l(mLock);
    return getCapabilities();
}
int MtkHdmiService::get_display_type()
{
    Mutex::Autolock _l(mLock);
    return getDisplayType();
}

/*
void MtkHdmiService::hdmi_register_callback()
{
    ALOGD("hdmi_register_callback");
}
*/

}; // namespace android

