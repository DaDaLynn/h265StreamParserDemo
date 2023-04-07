LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := foo-prebuilt
ifneq ($(filter $(NDK_KNOWN_DEVICE_ABI64S), $(TARGET_ARCH_ABI)),)
LOCAL_SRC_FILES := ../build_android/libs/arm64-v8a/libH265StreamParser.so
else
LOCAL_SRC_FILES := ../build_android/libs/armeabi-v7a/libH265StreamParser.so
endif
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

ifneq ($(filter $(NDK_KNOWN_DEVICE_ABI64S), $(TARGET_ARCH_ABI)),)
LOCAL_MODULE    := H265StreamParserDemo64
TARGET_ARCH_ABI := arm64-v8a
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -o3 -march=armv8-a --signed-char -std=c++11 -w -mfpu=neon -ffast-math -mfloat-abi=softfp -pie -fPIE -fPIC
LOCAL_LDFLAGS := -pie -fPIE
LOCAL_ARM_NEON := true
else
LOCAL_MODULE    := H265StreamParserDemo32
TARGET_ARCH_ABI := armeabi-v7a
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -o3 -march=armv7-a --signed-char -std=c++11 -w -mfpu=neon -ffast-math -mfloat-abi=softfp -mtune=cortex-a8 -pie -fPIE -fPIC
LOCAL_LDFLAGS := -pie -fPIE
LOCAL_ARM_NEON := true
endif

LOCAL_SRC_FILES := ../StreamParser.cpp
LOCAL_SHARED_LIBRARIES := foo-prebuilt

include $(BUILD_EXECUTABLE)