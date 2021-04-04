LOCAL_PATH := $(call my-dir)
ifneq ($(filter aquaris_m8 freezerfhd,$(TARGET_DEVICE)),)
include $(call all-makefiles-under,$(LOCAL_PATH))
include $(CLEAR_VARS)
endif
