LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SDK_VERSION := current
LOCAL_PACKAGE_NAME := MediaTekParts
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_PROGUARD_FLAG_FILES := proguard.flags
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true

LOCAL_STATIC_JAVA_LIBRARIES := \
    android-common

LOCAL_AAPT_FLAGS := \
    --auto-add-overlay

LOCAL_RESOURCE_DIR := \
    $(addprefix $(LOCAL_PATH)/, res)

include frameworks/base/packages/SettingsLib/common.mk

include $(BUILD_PACKAGE)
