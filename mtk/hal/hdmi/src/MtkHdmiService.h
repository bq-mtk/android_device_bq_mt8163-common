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

#ifndef ANDROID_MTKHDMISERVICE_H
#define ANDROID_MTKHDMISERVICE_H

#include <stdint.h>
#include <sys/types.h>

#include <IMtkHdmiService.h>

#include <utils/Atomic.h>
#include <utils/Errors.h>
#include <utils/threads.h>
#include <binder/MemoryDealer.h>
#include <utils/KeyedVector.h>
#include <utils/SortedVector.h>
#include <utils/Vector.h>
#include <IMtkHdmiClient.h>
typedef unsigned long  u64;


namespace android
{

/*****************************************************************************
*                        C L A S S   D E F I N I T I O N
******************************************************************************
*/

class MtkHdmiService : public BnMtkHdmiService
{
public:
    static void initial();
    virtual int enable_hdcp(int enable);
    virtual int enable_hdmi(int enable);
    virtual int set_video_resolution(int resolution);
    virtual int* get_resolution_mask();
    virtual int get_capability();
    virtual int get_display_type();
    virtual int* get_audio_capability();
    static int setVideoConfig(int vformat);
    static bool setDeepColor(int color, int deep);
	static void refreshEdid();

    // register a current process for notifications
    virtual void registerClient(const sp<IMtkHdmiClient>& client);

   // IBinder::DeathRecipient
    virtual     void        binderDied(const wp<IBinder>& who);

    sp<IMtkHdmiClient>      mClient;

private:
    MtkHdmiService();
    virtual                 ~MtkHdmiService();
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif
