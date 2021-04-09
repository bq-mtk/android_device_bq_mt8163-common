COMMON_PATH := device/bq/mt8163-common

# Call the Vendor Makefile
$(call inherit-product, vendor/bq/mt8163-common/mt8163-common-vendor.mk)

# Dalvik/HWUI
$(call inherit-product, frameworks/native/build/tablet-7in-hdpi-1024-dalvik-heap.mk)
$(call inherit-product-if-exists, frameworks/native/build/phone-xxhdpi-2048-hwui-memory.mk)

# GC
PRODUCT_TAGS += dalvik.gc.type-precise

# Overlays
DEVICE_PACKAGE_OVERLAYS += \
         $(COMMON_PATH)/overlay \
         $(COMMON_PATH)/overlay-lineage

# Camera
PRODUCT_PACKAGES += \
    android.hardware.camera.provider@2.4-impl-legacy \
    Snap \
    libcamera_parameters_mtk

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/init/mediaserver.rc:system/etc/init/mediaserver.rc

# Debugging tool
PRODUCT_PACKAGES += \
    debug_tool

# Remove Unused/Useless packages
PRODUCT_PACKAGES += \
    RemovePackages

# MediaTekParts
PRODUCT_PACKAGES += \
    MediaTekParts

# Shim symbols
PRODUCT_PACKAGES += \
    libshim_bionic \
    libshim_atomic \
    libshim_audio \
    libshim_omx \
    libshim_camera \
    libshim_xlog \
    libshim_gps \
    libshim_program_binary_service \
    libshim_wvm \
    libshim_drm \
    libshim_media \
    libshim_mute_log

# Power
PRODUCT_PACKAGES += \
    android.hardware.power@1.0-impl \
    power.mt8163

# GPS
PRODUCT_PACKAGES += \
    android.hardware.gnss@1.0-impl \
    libcurl \
    gps.mt8163

PRODUCT_PROPERTY_OVERRIDES += \
    persist.force.gps.mode=gnss

# MNLD
ifeq ($(TARGET_PROVIDES_MNLD_HAL),false) 
PRODUCT_PACKAGES += \
    libmnl \
    mnld \
    libDR \
    libgeofence \
    libhostill \
    libsupl
endif

# MTK WPA Supplicant Libraries
PRODUCT_PACKAGES += \
    lib_driver_cmd_mt66xx

# Network
PRODUCT_PACKAGES += \
    netd \
    wpa_supplicant \
    wpa_cli \
    hostapd \
    hostapd_cli \
    wificond \
    wifilogd

# HIDL
PRODUCT_PACKAGES += \
    android.hidl.base@1.0 \
    android.hidl.manager@1.0

# Graphic
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.mapper@2.0-impl \
    android.hardware.renderscript@1.0-impl \
    libGLES_android \
    libgralloc_extra \
    libgui_ext \
    libui_ext \
    libion

# Memtrack
PRODUCT_PACKAGES += \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service

# Vibrator
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service

# Bluetooth
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.bluetooth@1.0-service \
    libbt-vendor \
    libbluetooth_mtk

PRODUCT_PROPERTY_OVERRIDES += \
    ro.boot.btmacaddr=00:00:00:00:00:00

# DRM
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service.widevine \
    android.hardware.drm@1.0-service \
    libdrm \
    libdrmclearkeyplugin \
    libmockdrmcryptoplugin

# FM Radio
PRODUCT_PACKAGES += \
	FMRadio \
	libfmcust

# USB HAL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.0-service

# Keymaster
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service

# Gatekeeper
PRODUCT_PACKAGES += \
    android.hardware.gatekeeper@1.0-impl

# Lights
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-impl-mediatek \
    android.hardware.light@2.0-service-mediatek \
    lights.mt8163

# Health
PRODUCT_PACKAGES += \
    android.hardware.health@1.0-impl \
    android.hardware.health@1.0-service

# Tether Packages
PRODUCT_PACKAGES += \
    android.hardware.tetheroffload.config@1.0 \
    android.hardware.tetheroffload.control@1.0

# Audio
PRODUCT_PACKAGES += \
    android.hardware.audio@2.0-impl \
    android.hardware.audio@2.0-service \
    android.hardware.audio.effect@2.0-impl \
    android.hardware.broadcastradio@1.0-impl \
    android.hardware.soundtrigger@2.0-impl \
    android.hardware.soundtrigger@2.0-service \
    audio.a2dp.default \
    audio.r_submix.default \
    audio.r_submix.mt8163 \
    audio.usb.default \
    audio_policy.stub \
    libaudio-resampler \
    libtinyalsa \
    libtinyxml \
    tinymix

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/audio/audio_effects.conf:system/etc/audio_effects.conf \
    $(COMMON_PATH)/configs/audio/audio_device.xml:system/etc/audio_device.xml \
    $(COMMON_PATH)/configs/audio/audio_em.xml:system/etc/audio_em.xml \
    $(COMMON_PATH)/configs/audio/AudioParamOptions.xml:system/etc/audio_param/AudioParamOptions.xml \
    $(COMMON_PATH)/configs/init/audioserver.rc:system/etc/init/audioserver.rc

# XML Parser
PRODUCT_PACKAGES += libxml2

# USB
PRODUCT_PACKAGES += com.android.future.usb.accessory

# Ramdisk
PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(COMMON_PATH)/rootdir,root)

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml

# Media
PRODUCT_PACKAGES += \
    android.hardware.media.omx@1.0

PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:system/etc/media_codecs_google_video_le.xml \
    $(COMMON_PATH)/configs/media/media_profiles.xml:system/etc/media_profiles.xml \
    $(COMMON_PATH)/configs/media/media_codecs.xml:system/etc/media_codecs.xml \
    $(COMMON_PATH)/configs/media/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
    $(COMMON_PATH)/configs/media/media_codecs_mediatek_video.xml:system/etc/media_codecs_mediatek_video.xml \
    $(COMMON_PATH)/configs/media/media_codecs_mediatek_audio.xml:system/etc/media_codecs_mediatek_audio.xml

PRODUCT_PROPERTY_OVERRIDES += \
     persist.media.treble_omx=false

# Wifi
PRODUCT_PACKAGES += \
    android.hardware.wifi@1.0-service

PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/configs/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    $(COMMON_PATH)/configs/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf \
    $(COMMON_PATH)/configs/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
    $(COMMON_PATH)/configs/RIL/ecc_list.xml:system/etc/ecc_list.xml \
    $(COMMON_PATH)/configs/RIL/spn-conf.xml:system/etc/spn-conf.xml

# Vulkan
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.vulkan.level-0.xml:system/vendor/etc/permissions/android.hardware.vulkan.level.xml \
    frameworks/native/data/etc/android.hardware.vulkan.version-1_0_3.xml:system/vendor/etc/permissions/android.hardware.vulkan.version.xml

# Seccomp policy
PRODUCT_COPY_FILES += \
    $(COMMON_PATH)/seccomp/mediacodec.policy:system/vendor/etc/seccomp_policy/mediacodec.policy \
    $(COMMON_PATH)/seccomp/mediaextractor.policy:system/vendor/etc/seccomp_policy/mediaextractor.policy

# Sensors
PRODUCT_PACKAGES += \
    android.hardware.sensors@1.0-impl-mediatek \
    android.hardware.sensors@1.0-service-mediatek

# HDMI
PRODUCT_PACKAGES += \
    Overscan \
    hdmi \
    hdmimanager \
    libhdmiservice \
    libhdmi

PRODUCT_PROPERTY_OVERRIDES += \
    sys.service.hdmi.enable=0

# Filesystem management tools
PRODUCT_PACKAGES += \
	e2fsck \
	fibmap.f2fs \
	fsck.f2fs \
	mkfs.f2fs \
	make_ext4fs \
	resize2fs \
	setup_fs \
	ext4_resize \
	libext2_blkid \
	libext2_uuid_static \
	superumount \
	mount.exfat \
	fsck.exfat \
	mkfs.exfat \
	fsck.ntfs \
	mkfs.ntfs \
	mount.ntfs

# Property Overrides
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.allow.mock.location=1 \
    ro.zygote=zygote64_32 \
    ro.dalvik.vm.native.bridge=0 \
    persist.sys.usb.config=mtp,adb \
    persist.debug.xlog.enable=0
