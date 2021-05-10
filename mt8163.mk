COMMON_PATH := device/bq/mt8163-common

# Call the Vendor Makefile
$(call inherit-product, vendor/bq/mt8163-common/mt8163-common-vendor.mk)

# Dalvik/HWUI
$(call inherit-product, frameworks/native/build/tablet-7in-hdpi-1024-dalvik-heap.mk)
$(call inherit-product-if-exists, frameworks/native/build/phone-xxhdpi-2048-hwui-memory.mk)

# GC Tag
PRODUCT_TAGS += dalvik.gc.type-precise

# Overlays
DEVICE_PACKAGE_OVERLAYS += \
	$(COMMON_PATH)/overlay \
	$(COMMON_PATH)/overlay-lineage

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
	$(TOPDIR)frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
	$(COMMON_PATH)/configs/audio/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml \
	$(COMMON_PATH)/configs/audio/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
	$(COMMON_PATH)/configs/audio/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml \
	$(COMMON_PATH)/configs/audio/audio_device.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/audio_device.xml \
	$(COMMON_PATH)/configs/audio/audio_em.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/audio_em.xml \
	$(COMMON_PATH)/configs/audio/AudioParamOptions.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/audio_param/AudioParamOptions.xml \
	$(COMMON_PATH)/configs/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
	$(COMMON_PATH)/configs/init/audioserver.rc:$(TARGET_COPY_OUT_SYSTEM)/etc/init/audioserver.rc

# Bluetooth
PRODUCT_PACKAGES += \
	android.hardware.bluetooth@1.0-impl \
	android.hardware.bluetooth@1.0-service \
	libbt-vendor \
	libbluetooth_mtk

PRODUCT_PROPERTY_OVERRIDES += \
	ro.boot.btmacaddr=00:00:00:00:00:00

# Camera
PRODUCT_PACKAGES += \
	android.hardware.camera.provider@2.4-impl-legacy \
	Snap \
	libcamera_parameters_mtk

PRODUCT_COPY_FILES += \
	$(COMMON_PATH)/configs/init/mediaserver.rc:$(TARGET_COPY_OUT_SYSTEM)/etc/init/mediaserver.rc

# Configstore
PRODUCT_PACKAGES += \
    android.hardware.configstore@1.1-service

# DRM
PRODUCT_PACKAGES += \
	android.hardware.drm@1.0-impl \
	android.hardware.drm@1.0-service.widevine \
	android.hardware.drm@1.0-service \
	libdrm \
	libdrmclearkeyplugin \
	libmockdrmcryptoplugin

# Debugging tool
PRODUCT_PACKAGES += \
	debug_tool

# Enable StageFrightCodec 2.0
PRODUCT_PACKAGES += \
    libstagefright_ccodec

# FM Radio
PRODUCT_PACKAGES += \
	FMRadio \
	libfmcust

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

# Force Lockscreen Rotation
PRODUCT_PRODUCT_PROPERTIES += \
	log.tag.launcher_force_rotate=VERBOSE \
	lockscreen.rot_override=true

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

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.vulkan.level-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level.xml \
	frameworks/native/data/etc/android.hardware.vulkan.version-1_0_3.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version.xml

# GPS
PRODUCT_PACKAGES += \
	android.hardware.gnss@1.0-impl \
	libcurl \
	gps.mt8163

PRODUCT_PROPERTY_OVERRIDES += \
	persist.force.gps.mode=gnss

PRODUCT_COPY_FILES += \
	$(COMMON_PATH)/configs/gps/agps_profiles_conf2.xml:$(TARGET_COPY_OUT_VENDOR)/etc/agps_profiles_conf2.xml \
	$(COMMON_PATH)/configs/gps/gps.conf:$(TARGET_COPY_OUT_VENDOR)/etc/gps.conf

# Gatekeeper
PRODUCT_PACKAGES += \
	android.hardware.gatekeeper@1.0-impl

# HIDL
PRODUCT_PACKAGES += \
	android.hidl.base@1.0 \
	android.hidl.manager@1.0

# Health
PRODUCT_PACKAGES += \
	android.hardware.health@1.0-impl \
	android.hardware.health@1.0-service

# HDMI
PRODUCT_PACKAGES += \
	Overscan \
	hdmi \
	hdmimanager \
	libhdmiservice \
	libhdmi

PRODUCT_PROPERTY_OVERRIDES += \
	sys.service.hdmi.enable=0

# Keymaster
PRODUCT_PACKAGES += \
	android.hardware.keymaster@3.0-impl \
	android.hardware.keymaster@3.0-service

# Lights
PRODUCT_PACKAGES += \
	android.hardware.light@2.0-impl-mediatek \
	android.hardware.light@2.0-service-mediatek \
	lights.mt8163

# LiveDisplay
PRODUCT_PACKAGES += \
    vendor.lineage.livedisplay@2.0-service-mediatek

# Media
PRODUCT_PACKAGES += \
	android.hardware.media.omx@1.0

PRODUCT_COPY_FILES += \
	frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_google_audio.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_google_telephony.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_google_video_le.xml \
	$(COMMON_PATH)/configs/media/media_profiles.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_profiles_V1_0.xml \
	$(COMMON_PATH)/configs/media/media_codecs.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs.xml \
	$(COMMON_PATH)/configs/media/media_codecs_performance.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_performance.xml \
	$(COMMON_PATH)/configs/media/media_codecs_mediatek_video.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_mediatek_video.xml \
	$(COMMON_PATH)/configs/media/media_codecs_mediatek_audio.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/media_codecs_mediatek_audio.xml

PRODUCT_PROPERTY_OVERRIDES += \
	media.stagefright.legacyencoder=true \
	media.stagefright.less-secure=true \
	persist.media.treble_omx=false

# Memory optimization
PRODUCT_PRODUCT_PROPERTIES += \
	ro.sys.fw.bservice_enable=true \
	ro.sys.fw.bservice_limit=5 \
	ro.sys.fw.bservice_age=5000 \
	ro.am.reschedule_service=true \
	ro.sys.fw.bg_apps_limit=24

# MediaTekParts
PRODUCT_PACKAGES += \
	MediaTekParts

# Memtrack
PRODUCT_PACKAGES += \
	android.hardware.memtrack@1.0-impl \
	android.hardware.memtrack@1.0-service \
	memtrack.mt8163

# Power
PRODUCT_PACKAGES += \
	android.hardware.power@1.0-impl \
	power.mt8163

# Permissions
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.bluetooth_le.xml \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.bluetooth.xml \
	frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.camera.front.xml \
	frameworks/native/data/etc/android.hardware.camera.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.camera.xml \
	frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.sensor.accelerometer.xml \
	frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.wifi.direct.xml \
	frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.wifi.xml \
	frameworks/native/data/etc/android.software.sip.voip.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.software.sip.voip.xml \
	frameworks/native/data/etc/tablet_core_hardware.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/tablet_core_hardware.xml \
	frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.hardware.location.gps.xml

# Property Overrides
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
	ro.allow.mock.location=1 \
	ro.zygote=zygote64_32 \
	camera.disable_zsl_mode=1 \
	ro.dalvik.vm.native.bridge=0 \
	persist.sys.usb.config=mtp,adb \
	persist.debug.xlog.enable=0 \
	ro.adb.secure=0 \
	ro.secure=0

# Ramdisk
PRODUCT_COPY_FILES += \
	$(call find-copy-subdir-files,*,$(COMMON_PATH)/rootdir,root)

# Remove Unused/Useless packages
PRODUCT_PACKAGES += \
	RemovePackages

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
	libshim_mute_log \
	libshim_gui

# Seccomp policy
PRODUCT_COPY_FILES += \
	$(COMMON_PATH)/seccomp/mediacodec.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy \
	$(COMMON_PATH)/seccomp/mediaextractor.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediaextractor.policy

# Sensors
PRODUCT_PACKAGES += \
	android.hardware.sensors@1.0-impl-mediatek \
	android.hardware.sensors@1.0-service-mediatek

# Tether Packages
PRODUCT_PACKAGES += \
	android.hardware.tetheroffload.config@1.0 \
	android.hardware.tetheroffload.control@1.0

# Thermal
PRODUCT_COPY_FILES += \
	$(COMMON_PATH)/configs/thermal/thermal.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/.tp/thermal.conf \
	$(COMMON_PATH)/configs/thermal/thermal.off.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/.tp/thermal.off.conf \
	$(COMMON_PATH)/configs/thermal/.ht120.mtc:$(TARGET_COPY_OUT_SYSTEM)/etc/.tp/.ht120.mtc

# USB
PRODUCT_PACKAGES += \
	android.hardware.usb@1.0-service \
	com.android.future.usb.accessory

# Vendor security patch level
PRODUCT_PROPERTY_OVERRIDES += \
    ro.lineage.build.vendor_security_patch=2018-08-05

# Wifi
PRODUCT_PACKAGES += \
	android.hardware.wifi@1.0-service \
	lib_driver_cmd_mt66xx \
	netd \
	wpa_supplicant \
	wpa_cli \
	hostapd \
	hostapd_cli \
	wificond \
	wifilogd

PRODUCT_COPY_FILES += \
	$(COMMON_PATH)/configs/wifi/wpa_supplicant.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/wifi/wpa_supplicant.conf \
	$(COMMON_PATH)/configs/wifi/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/wifi/wpa_supplicant_overlay.conf \
	$(COMMON_PATH)/configs/wifi/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/wifi/p2p_supplicant_overlay.conf \
	$(COMMON_PATH)/configs/RIL/ecc_list.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/ecc_list.xml \
	$(COMMON_PATH)/configs/RIL/spn-conf.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/spn-conf.xml

# XML Parser
PRODUCT_PACKAGES += \
	libxml2
