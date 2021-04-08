package com.mediatek.hdmi;

import android.os.Binder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Parcel;
import android.os.RemoteException;
import android.util.Log;
import android.os.ServiceManager;
import android.os.SystemProperties;
import java.util.ArrayList;
import java.util.Arrays;

public class HdmiNative {
    static HdmiNative mHdmi;
    IBinder mHdmiNative;

    private static final boolean DEBUG = true;
    // ////Transaction code, don't change ////////
    private static final int ENABLE_HDCP = 1;
    private static final int SET_VIDEO_RESOLUTION = 2;
    private static final int GET_RESOLUTION_MASK = 3;
    private static final int GET_CAPABILITY = 4;
    private static final int GET_AUDIO_CAPABILITY = 5;
    private static final int REGISTER_CALLBACK = 6;
    private static final int ENABLE_HDMI = 7;
    private static final int GET_DISPLAY_TYPE = 8;

    private static final String TAG = "HdmiNative";
    private String HDMI_ENABLE = "sys.hdmi.enable";
    private String HDCP_ENABLE = "persist.sys.hdcp.enable";
    private String HDMI_VIDEO_RESOLUTION = "persist.sys.hdmi.resolution";
    private String HDMI_COLOR_SPACE = "persist.sys.hdmi.color_space";
    private String HDMI_DEEP_COLOR = "persist.sys.hdmi.deep_color";
    /**
     * HDMI capability definition: HDMI and call mutex
     *
     * @internal
     */
    public static final int CAPABILITY_MUTEX_CALL = 0x04;

    public static final int HDMI_MAX_CHANNEL = 0x78; // 1111000
    public static final int HDMI_MAX_SAMPLERATE= 0x380; // 1110000000
    public static final int HDMI_MAX_BITWIDTH = 0xc00; // 110000000000
    public static final int HDMI_MAX_CHANNEL_OFFSETS = 3;
    public static final int HDMI_MAX_SAMPLERATE_OFFSETS = 7;
    public static final int HDMI_MAX_BITWIDTH_OFFSETS = 10;

    public static final int AUDIO_OUTPUT_STEREO = 2;
    public static final int AUDIO_OUTPUT_MULTICHANNEL= 6;
    /**
     * HDMI resolution define AUTO mode offset value
     * Ex. 102, UI show AUTO, output resolution is 720p_60Hz
     *
     * @internal
     */
    public static final int AUTO = 100;

    /**
     * HDMI display type definition: HDMI
     */
    public static final int DISPLAY_TYPE_HDMI = 0;

    /**
     * HDMI display type definition: smartbook
     */
    public static final int DISPLAY_TYPE_SMB = 1;

    /**
     * HDMI display type definition: MHL
     */
    public static final int DISPLAY_TYPE_MHL = 2;

    /**
     * HDMI display type definition: Slim Port
     */
    public static final int DISPLAY_TYPE_SLIMPORT = 3;

    boolean mCablePlugged = false;

    // HDMI capbilities
    private int mCapabilities;
    private int mDisplayType;

    private int mHdmiState;
    private int mPrevHdmiState;

    private int[] mEdid;
    private int[] mPreEdid;

    private static final int MSG_HDMI = 1;

    class HdmiEventCallback extends Binder {
        protected boolean onTransact(int code, Parcel data, Parcel reply,
                int flags) throws RemoteException {
            Log.d(TAG,"HdmiEventCallback: " + code);
            Log.d(TAG,"HdmiEventCallback11 data.readInt(): " + data.readInt());
            if (code == 1) {
                data.enforceInterface("hdmi.IMtkHdmiClient");
                int event = data.readInt();
                Log.d(TAG,"HdmiEventCallback event =  " + event);
                return true;
            }
            return false;
        }
    }
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Log.i(TAG, "handleMessage: " + msg.what);
            switch (msg.what) {
            case MSG_HDMI:
                int state = (Integer) msg.arg1;
                hdmiCableStateChanged(state);
                break;
            default:
                super.handleMessage(msg);
            }
        }
    };

    private HdmiObserver mHdmiObserver;

    // //////////////////////public APIS//////////////////////////

    public static HdmiNative getInstance() {
        if (mHdmi == null){
           mHdmi = new HdmiNative();
        }
        return mHdmi;
    }

    private HdmiNative() {
        // this name is defined in native, don't change it
        mHdmiNative = ServiceManager.getService("hdmi.MtkHdmiService");
        log(TAG, "GET HDMI SERVICE : " + mHdmiNative);

        mHdmiObserver = HdmiObserver.getHdmiObserver();
        mHdmiObserver.setHandler(mHandler);
        mHdmiObserver.startObserve();
    }

    public void deInitHdmi() {
        mHdmiObserver.stopObserve();
    }

    public int[] getSupportedResolutions() {
        return getSupportedResolutionsImpl();
    }

    public boolean enableHDCP(boolean enable) {
        if (mHdmiNative != null) {
            log(TAG, "enableHdcp: " + enable);
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            data.writeInt(enable ? 1 : 0);
            try {
                mHdmiNative.transact(ENABLE_HDCP, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            int res = reply.readInt();

            reply.recycle();
            data.recycle();
            return (res > 0 ? true : false);
        }
        return false;
    }

    public boolean enableHDMIImpl(boolean enable) {
        if (mHdmiNative != null) {
            log(TAG, "enableHDMI: " + enable);
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            data.writeInt(enable ? 1 : 0);
            try {
                mHdmiNative.transact(ENABLE_HDMI, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            int res = reply.readInt();

            reply.recycle();
            data.recycle();
            return (res > 0 ? true : false);
        }
        return false;
    }

    public boolean enableHdmi(boolean enable) {
        log(TAG, "enableHdmi: " + enable);
        boolean ret = false;
        ret = enableHDMIImpl(enable);
        log(TAG, "enableHdmi ret:" + ret);
        return ret;

    }

    public void registerHdmiClient() {
        if (mHdmiNative != null) {
            log(TAG, "registerHdmiClient: ");
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            data.writeStrongBinder(new HdmiEventCallback());
            try {
                mHdmiNative.transact(REGISTER_CALLBACK, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    public boolean setVideoResolutionImpl(int resolution) {
        if (mHdmiNative != null) {
            log(TAG, "setVideoResolutionImpl: " + resolution);
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            data.writeInt(resolution);
            try {
                mHdmiNative.transact(SET_VIDEO_RESOLUTION, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            int res = reply.readInt();

            reply.recycle();
            data.recycle();
            return (res > 0 ? true : false);
        }
        return false;
    }

    public boolean setVideoResolution(int resolution) {
        log(TAG, "setVideoResolution: " + resolution);
        boolean ret = false;
        int suitableResolution = resolution;
        if (resolution >= HdmiDef.AUTO) {
            suitableResolution = getSuitableResolution(resolution);
        }
        int finalResolution = suitableResolution >= HdmiDef.AUTO ? (suitableResolution - HdmiDef.AUTO)
                : suitableResolution;
        log(TAG, "final video resolution: " + finalResolution);
        ret = setVideoResolutionImpl(finalResolution);
        log(TAG, "setVideoResolution ret:" + ret);
        return ret;
    }

    public boolean hasCapability(int mask) {
        log(TAG, "hasCapability: " + mask);
        return (mCapabilities & mask) != 0;
    }

    public int getCapabilities() {
        if (mHdmiNative != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            try {
                mHdmiNative.transact(GET_CAPABILITY, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            mCapabilities = reply.readInt();
            log(TAG, "getCapabilities: 0x" + Integer.toHexString(mCapabilities));
            reply.recycle();
            data.recycle();
        }
        return mCapabilities;
    }

    public int getDisplayTypeImpl() {
        if (mHdmiNative != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            try {
                mHdmiNative.transact(GET_DISPLAY_TYPE, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            mDisplayType = reply.readInt();
            log(TAG, "getDisplayTypeImpl: 0x" + Integer.toHexString(mDisplayType));
            reply.recycle();
            data.recycle();
        }
        return mDisplayType;
    }

    public int getDisplayType() {
        return getDisplayTypeImpl();
    }

    public int[] getResolutionMask() {
        if (mHdmiNative != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            try {
                mHdmiNative.transact(GET_RESOLUTION_MASK, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            int[] res = new int[3];
            res[0] = reply.readInt();
            res[1] = reply.readInt();
            res[2] = reply.readInt();
            log(TAG, "getResolutionMask[0]: " + res[0]);
            reply.recycle();
            data.recycle();
            return res;
        }
        return null;
    }

    public int[] getAudioCapability() {
        if (mHdmiNative != null) {
            Parcel data = Parcel.obtain();
            Parcel reply = Parcel.obtain();
            data.writeInterfaceToken("hdmi.IMtkHdmiService");
            try {
                mHdmiNative.transact(GET_AUDIO_CAPABILITY, data, reply, 0);
            } catch (RemoteException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            int[] res = new int[3];
            res[0] = reply.readInt();
            res[1] = reply.readInt();
            res[2] = reply.readInt();
            log(TAG, "getAudioCapability[0]: " + res[0]);
            reply.recycle();
            data.recycle();
            return res;
        }
        return null;
    }
    // ////////////////implements internal/////////////////////////////////////

    private int getSuitableResolution(int videoResolution) {
    log(TAG, "getSuitableResolution videoResolution: " + videoResolution);
        int[] supportedResolutions = getSupportedResolutions();
        ArrayList<Integer> resolutionList = new ArrayList<Integer>();
        for (int res : supportedResolutions) {
            resolutionList.add(res);
        }
        if (needUpdate(videoResolution)) {
            if (mEdid[0] != 0 || mEdid[1]!= 0 || mEdid[2]!= 0) {
                int edidTemp = mEdid[0] | mEdid[1];
                int edidTemp_4k2k = mEdid[2];
                log(TAG, "getSuitableResolution edidTemp:" + edidTemp);
                log(TAG, "getSuitableResolution edidTemp_4k2k:" + edidTemp_4k2k);
                int index = 0;
                if (FeatureOption.MTK_INTERNAL_HDMI_SUPPORT
                        || FeatureOption.MTK_INTERNAL_MHL_SUPPORT) {
                    index = 1;
                } else if (FeatureOption.MTK_MT8193_HDMI_SUPPORT
                    || FeatureOption.MTK_ITE66121_HDMI_SUPPORT) {
                    index = 2;
                } else if (FeatureOption.MTK_TB6582_HDMI_SUPPORT) {
                    index = 2;
                }else {
                    index = 3;
                }
                log(TAG, "getSuitableResolution index:" + index);
                int[] prefered = HdmiDef.getPreferedResolutions(index);
                for (int res : prefered) {
                    log(TAG, "getSuitableResolution res:" + res);
                    int act = res;
                    if (res >= HdmiDef.AUTO) {
                        act = res - HdmiDef.AUTO;
                    }
                    log(TAG, "getSuitableResolution act:" + act);
                    if(act<HdmiDef.RESOLUTION_3840X2160P23_976HZ){
                        if (0 != (edidTemp & HdmiDef.sResolutionMask[act])
                              && resolutionList.contains(act)) {
                            videoResolution = res;
                            log(TAG, "getSuitableResolution videoResolution 1:" + videoResolution);
                            break;
                        }
                    }else{
                        if (0 != (edidTemp_4k2k & HdmiDef.sResolutionMask_4k2k[act - HdmiDef.RESOLUTION_3840X2160P23_976HZ])
                              && resolutionList.contains(act)) {
                            videoResolution = res;
                            log(TAG, "getSuitableResolution videoResolution 2:" + videoResolution);
                            break;
                        }
                    }
                }
            }
        }
        log(TAG, "suiteable video resolution: " + videoResolution);
        return videoResolution;
    }

    private static class FeatureOption {

        public static final boolean MTK_DRM_KEY_MNG_SUPPORT = getValue("ro.mtk_drm_key_mng_support");
        public static final boolean MTK_HDMI_HDCP_SUPPORT = getValue("ro.mtk_hdmi_hdcp_support");
        public static final boolean MTK_MT8193_HDCP_SUPPORT = getValue("ro.mtk_mt8193_hdcp_support");
        public static final boolean MTK_CLEARMOTION_SUPPORT = getValue("ro.mtk_clearmotion_support");
        public static final boolean MTK_INTERNAL_MHL_SUPPORT = getValue("ro.mtk_internal_mhl_support");
        public static final boolean MTK_INTERNAL_HDMI_SUPPORT = getValue("ro.mtk_internal_hdmi_support");
        public static final boolean MTK_MT8193_HDMI_SUPPORT = getValue("ro.mtk_mt8193_hdmi_support");
        public static final boolean MTK_ENABLE_HDMI_MULTI_CHANNEL = true;
        public static final boolean MTK_TB6582_HDMI_SUPPORT = getValue("ro.hdmi.1080p60.disable");
        public static final boolean MTK_ALPS_BOX_SUPPORT = getValue("ro.mtk_alps_box_support");
        public static final boolean MTK_HDMI_4K_SUPPORT = getValue("ro.mtk_hdmi_4k_support");
        public static final boolean MTK_ITE66121_HDMI_SUPPORT = getValue("ro.mtk_ite66121_hdmi_support");

        private static boolean getValue(String key) {
            return SystemProperties.get(key).equals("1");
        }
    }

    private int[] getSupportedResolutionsImpl() {
        log(TAG, "getSupportedResolutionsImpl");
        mEdid = getResolutionMask();
        /*if(mEdid == null){
            return HdmiDef.getDefaultResolutions(3);
        }
        if (0 == mEdid[0] && 0 == mEdid[1] && 0 == mEdid[2]) {
            return HdmiDef.getDefaultResolutions(3);
        }*/
        int[] resolutions;
        if (FeatureOption.MTK_INTERNAL_HDMI_SUPPORT
                || FeatureOption.MTK_INTERNAL_MHL_SUPPORT) {
                log(TAG, "MTK_INTERNAL_HDMI_SUPPORT");
            if(FeatureOption.MTK_HDMI_4K_SUPPORT){
                return HdmiDef.getDefaultResolutions(1);
            } else {
                return HdmiDef.getDefaultResolutions(4);
            }
        } else if (FeatureOption.MTK_MT8193_HDMI_SUPPORT
            || FeatureOption.MTK_ITE66121_HDMI_SUPPORT){
            resolutions = HdmiDef.getDefaultResolutions(2);
        } else {
            resolutions = HdmiDef.getAllResolutions();
        }
        int edidTemp = mEdid[0] | mEdid[1];
        //add for 4k2k resolution
        int edidTemp_4k2k = mEdid[2];
        log(TAG, "getSupportedResolutionsImpl edidTemp:" + edidTemp);
        log(TAG, "getSupportedResolutionsImpl edidTemp_4k2k:" + edidTemp_4k2k);
        ArrayList<Integer> list = new ArrayList<Integer>();
        for (int res : resolutions) {
            try {
                if(res >= HdmiDef.RESOLUTION_3840X2160P23_976HZ){
                    int mask_4k2k = HdmiDef.sResolutionMask_4k2k[res - HdmiDef.RESOLUTION_3840X2160P23_976HZ];
                              if((edidTemp_4k2k & mask_4k2k) != 0){
                        if(!list.contains(res)){
                            list.add(res);
                        }
                    }
                }else if(res < HdmiDef.RESOLUTION_3840X2160P23_976HZ){
                int mask = HdmiDef.sResolutionMask[res];
                    if ((edidTemp & mask) != 0) {
                        if (!list.contains(res)) {
                            list.add(res);
                        }
                    }
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                Log.w(TAG, e.getMessage());
            }
        }
        resolutions = new int[list.size()];
        for (int i = 0; i < list.size(); i++) {
            resolutions[i] = list.get(i);
            log(TAG, "getSupportedResolutionsImpl i: " + i + ", " + resolutions[i]);
        }
        return resolutions;
    }

    // //////////////////////////////////////////////////////////////////////////////
    // when hdmi cable plugged state changed, refresh hdmi edit
    private void hdmiCableStateChanged(int state) {
        mCablePlugged = state == 1;
        Log.d(TAG,"hdmiCableStateChanged: [old state]"  + mHdmiState + "[new state]" + state);
        Log.d(TAG,"hdmiCableStateChanged: mCablePlugged "  + mCablePlugged);
        int hdmiState = state;
        int newOrOld = hdmiState | mHdmiState;
        int delay = 0;

        if (mHdmiState == hdmiState || ((newOrOld & (newOrOld - 1)) != 0)) {
            return;
        }

        mPrevHdmiState = mHdmiState;
        mHdmiState = hdmiState;

        getCapabilities(); // Add for multi channel, when state
                           // changed,capability maybe change.
        refreshEdid(mCablePlugged);
    }

    private void refreshEdid(boolean plugged) {
        Log.d(TAG,"refreshEdid: hdmi plugged is " + plugged);
        if (plugged) {
            mEdid = getResolutionMask();
            if (mEdid != null) {
                for (int i = 0; i < mEdid.length; i++) {
                    log(TAG, String.format("mEdid[%d] = %d", i, mEdid[i]));
                }
            } else {
                log(TAG, "mEdid is null!");
            }

            if (mPreEdid != null) {
                for (int i = 0; i < mPreEdid.length; i++) {
                    log(TAG, String.format("mPreEdid[%d] = %d", i, mPreEdid[i]));
                }
            } else {
                log(TAG, "mPreEdid is null!");
            }
        } else {
            mPreEdid = mEdid;
            // mEdid = null;
        }
    }

    private boolean needUpdate(int videoResolution) {
        log(TAG, "needUpdate: " + videoResolution);
        boolean needUpdate = true;
        if (videoResolution >= HdmiDef.AUTO) {
            needUpdate = true;
        }
        if (mPreEdid != null && Arrays.equals(mEdid, mPreEdid)) {
            needUpdate = false;
        }
        log(TAG, "needUpdate needUpdate: " + needUpdate);
        return needUpdate;
    }
    public boolean isSignalOutputting() {
        log(TAG, "isSignalOutputting, mCablePlugged = " + mCablePlugged);
        return mCablePlugged ;//&& FeatureOption.getValue(HDMI_ENABLE);
    }

    public int getAudioParameter(int mask, int offsets) {
        int param = (mCapabilities & mask) >> offsets;
        log(TAG, "getAudioParameter() mask: " + mask + " ,offsets: " + offsets
                + " ,param: " + param + " ,mCapabilities: 0x"
                + Integer.toHexString(mCapabilities));
        return param;
    }

    // ///////////////////////////////////////////////////

    private void log(String tag, String info) {
        if (DEBUG) {
            Log.i(tag, info);
        }
    }

}
