# Copyright (C) 2020 MediaTek Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# 2020/05/09
# Initial port of mnld to Lineage.

LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_PROVIDES_MNLD_HAL),false)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/utility/inc \
  $(LOCAL_PATH)/mnl_agps_interface/inc \
  $(LOCAL_PATH)/mnl_at_cmd_interface/inc \
  $(LOCAL_PATH)/mnl_flp_interface/inc \
  $(LOCAL_PATH)/mnl_mpe_interface/inc \
  $(LOCAL_PATH)/mnl_nlp_interface/inc \
  $(LOCAL_PATH)/mnl_geofence_interface/inc \
  $(LOCAL_PATH)/mnld_entity/inc \
  $(LOCAL_PATH)/mnl/inc \
  $(LOCAL_PATH)/curl/inc \
  $(LOCAL_PATH)/nvram \
  external/libxml2/include \
  $(TOP)/system/core/libcutils/include_vndk \
  $(TOP)/frameworks/native/include \
  $(TOP)/frameworks/hardware/interfaces/sensorservice/libsensorndkbridge \
  $(TOP)/hardware/interfaces/sensors/1.0/default/include

LOCAL_SRC_FILES := \
	mnld_entity/src/mnl2hal_interface.c \
	utility/src/data_coder.c \
	utility/src/mtk_lbs_utility.c \
	utility/src/mtk_socket_data_coder.c \
	utility/src/mtk_socket_utils.c \
	mnl_agps_interface/src/mnl_agps_interface.c \
	mnl_agps_interface/src/mnl2agps_interface.c \
	mnl_agps_interface/src/agps2mnl_interface.c \
	mnl_flp_interface/src/mtk_flp_controller.c \
	mnl_flp_interface/src/mtk_flp_main.c \
	mnl_flp_interface/src/mtk_flp_mnl_interface.c \
	mnl_flp_interface/src/mnl_flp_test_interface.c \
	mnl_geofence_interface/src/mtk_geofence_controller.c \
	mnl_geofence_interface/src/mtk_geofence_main.c \
	mnl_nlp_interface/src/Mnld2NlpUtilsInterface.c \
	mnl_mpe_interface/src/mpe_main.c \
	mnl_mpe_interface/src/mpe_logger.c \
	mnl_mpe_interface/src/mpe_sensor.cpp \
	mnl_at_cmd_interface/src/mnl_at_interface.c \
	mnld_entity/src/mnld.c \
	mnld_entity/src/mnld_uti.c \
	mnld_entity/src/gps_controller.c \
	mnld_entity/src/nmea_parser.c \
	mnld_entity/src/epo.c \
	mnld_entity/src/qepo.c \
	mnld_entity/src/mtknav.c \
	mnld_entity/src/mnl_common.c \
	mnld_entity/src/op01_log.c \
	mnld_entity/src/gps_dbg_log.c \
	mnld_entity/src/mpe.c \
	mnl/src/pseudo_mnl.c \
	utility/src/mtk_mnld_dump.cpp \

LOCAL_MODULE := mnld
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := 32
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)

LOCAL_CFLAGS += -DMTK_GPS_CO_CLOCK_DATA_IN_MD -DCONFIG_GPS_USER_DBG_LOAD -DMTK_AGPS_SUPPORT

LOCAL_STATIC_LIBRARIES +=  libsupl
LOCAL_SHARED_LIBRARIES += \
    libmnl \
    libgeofence\
    libcurl \
    libcutils \
    libc \
    libm \
    libnvram \
    libcrypto \
    libssl \
    libz \
    liblog \
    libhardware \
    libutils \
    libDR \
    libutils \
    android.frameworks.sensorservice@1.0 \
    android.hardware.sensors@1.0 \
    libhidlbase \
    libhidltransport \
    libsensorndkbridge

LOCAL_HEADER_LIBRARIES +=  libcutils_headers
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS += libhardware_headers
LOCAL_REQUIRED_MODULES := libmnl.so libgeofence.so

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
include vendor/bq/mt8163/proprietary/hardware/gps/mtk_mnld/mnl/libs/Android.mk
endif
