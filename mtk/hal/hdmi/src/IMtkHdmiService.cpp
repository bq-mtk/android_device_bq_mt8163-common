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

/*****************************************************************************
*                     C O M P I L E R   F L A G S
******************************************************************************
*/
/*****************************************************************************
*                E X T E R N A L   R E F E R E N C E S
******************************************************************************
*/
#define LOG_TAG "IMtkHdmiService"
#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <IMtkHdmiService.h>
#include <IMtkHdmiClient.h>
#include <binder/IPCThreadState.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/IMemory.h>


/*****************************************************************************
*                          C O N S T A N T S
******************************************************************************
*/

namespace android
{


/*****************************************************************************
*                         D A T A   T Y P E S
******************************************************************************
*/
enum
{
    ENABLE_HDCP = IBinder::FIRST_CALL_TRANSACTION,
    SET_VIDEO_RESOLUTION,
    GET_RESOLUTION_MASK,
    GET_CAPABILITY,
    GET_AUDIO_CAPABILITY,
    REGISTER_CLIENT,
    ENABLE_HDMI,
    GET_DISPLAY_TYPE
};


/*****************************************************************************
*                        C L A S S   D E F I N I T I O N
******************************************************************************
*/

class BpMtkHdmiService : public BpInterface<IMtkHdmiService>
{
public:
    BpMtkHdmiService(const sp<IBinder>& impl)
        : BpInterface<IMtkHdmiService>(impl)
    {
    }
    virtual int enable_hdcp(int enable)
    {
        Parcel data, reply;
        ALOGD("enableHdcp=%d", enable);
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());
        data.writeInt32(enable);
        remote()->transact(ENABLE_HDCP, data, &reply);
        return reply.readInt32();
    }
    virtual int enable_hdmi(int enable)
    {
        Parcel data, reply;
        ALOGD("enableHdmi=%d", enable);
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());
        data.writeInt32(enable);
        remote()->transact(ENABLE_HDMI, data, &reply);
        return reply.readInt32();
    }
    virtual int set_video_resolution(int resolution)
    {
        Parcel data, reply;
        ALOGD("setVideoResolution=%d", resolution);
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());
        data.writeInt32(resolution);
        remote()->transact(SET_VIDEO_RESOLUTION, data, &reply);
        return reply.readInt32();
    }

    virtual int* get_resolution_mask()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());

        remote()->transact(GET_RESOLUTION_MASK, data, &reply);
        int* resolution = new int[RES_MASK_NUM];
        resolution[0] = reply.readInt32();
        resolution[1] = reply.readInt32();
        resolution[2] = reply.readInt32();
        return resolution;
    }

    virtual int* get_audio_capability()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());

        remote()->transact(GET_AUDIO_CAPABILITY, data, &reply);
        int* audioValue = new int[RES_MASK_NUM];
        audioValue[0] = reply.readInt32();
        audioValue[1] = reply.readInt32();
        audioValue[2] = reply.readInt32();
        return audioValue;
    }
    virtual int get_capability()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());
        remote()->transact(GET_CAPABILITY, data, &reply);
        return reply.readInt32();
    }
    virtual int get_display_type()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());
        remote()->transact(GET_DISPLAY_TYPE, data, &reply);
        return reply.readInt32();
    }

    virtual void registerClient(const sp<IMtkHdmiClient>& client)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IMtkHdmiService::getInterfaceDescriptor());
        data.writeStrongBinder(client->asBinder(client));
        remote()->transact(REGISTER_CLIENT, data, &reply);
    }

};

IMPLEMENT_META_INTERFACE(MtkHdmiService, "hdmi.IMtkHdmiService");

// ----------------------------------------------------------------------

status_t BnMtkHdmiService::onTransact(
    uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    switch (code)
    {
        case ENABLE_HDCP:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int enable = data.readInt32();
            int ret =enable_hdcp(enable);
            reply->writeInt32(ret);
            return NO_ERROR;
        }

        case SET_VIDEO_RESOLUTION:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int resolution = data.readInt32();
            int ret = set_video_resolution(resolution);
            reply->writeInt32(ret);
            return NO_ERROR;
        }

        case GET_RESOLUTION_MASK:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int* res = get_resolution_mask();
            ALOGD("getResolutionMask=%d", *res);
            reply->writeInt32(*res);
            reply->writeInt32(*(res +1));
            reply->writeInt32(*(res +2));
            return NO_ERROR;
        }
        case GET_AUDIO_CAPABILITY:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int* res = get_audio_capability();
            ALOGD("getaudiocapability=%d", *res);
            reply->writeInt32(*res);
            reply->writeInt32(*(res +1));
            reply->writeInt32(*(res +2));
            return NO_ERROR;
        }
        case GET_CAPABILITY:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int res = get_capability();
            reply->writeInt32(res);
            return NO_ERROR;
        }

        case REGISTER_CLIENT:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            sp<IMtkHdmiClient> client = interface_cast<IMtkHdmiClient>(data.readStrongBinder());
            registerClient(client);
            return NO_ERROR;
        }
        case ENABLE_HDMI:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int enable = data.readInt32();
            int ret = enable_hdmi(enable);
            reply->writeInt32(ret);
            return NO_ERROR;
        }
        case GET_DISPLAY_TYPE:
        {
            CHECK_INTERFACE(IMtkHdmiService, data, reply);
            int res = get_display_type();
            reply->writeInt32(res);
            return NO_ERROR;
        }

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

// ----------------------------------------------------------------------------

}; // namespace android


