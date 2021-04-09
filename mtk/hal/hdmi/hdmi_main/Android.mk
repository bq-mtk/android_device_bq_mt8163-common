LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DMTK_HDMI_SUPPORT

LOCAL_SRC_FILES:= \
    main_hdmi.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libbinder \
    liblog \
    libhdmi \
    libhdmiservice

LOCAL_C_INCLUDES += \
    $(TOP)/device/bq/mt8163-common/mtk/hal/hdmi/include

LOCAL_MODULE := hdmi
LOCAL_MODULE_OWNER := mtk
LOCAL_INIT_RC := hdmi.rc

include $(BUILD_EXECUTABLE)
