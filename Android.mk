LOCAL_PATH := $(call my-dir)
ifneq ($(filter karin freezerfhd,$(TARGET_DEVICE)),)
include $(call all-makefiles-under,$(LOCAL_PATH))
include $(CLEAR_VARS)
endif
