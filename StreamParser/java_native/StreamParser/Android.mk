LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := foo-prebuilt
ifneq ($(filter $(NDK_KNOWN_DEVICE_ABI64S), $(TARGET_ARCH_ABI)),)
LOCAL_SRC_FILES := ../../build_android/libs/arm64-v8a/libH265StreamParser.so
else
LOCAL_SRC_FILES := ../../build_android/libs/armeabi-v7a/libH265StreamParser.so
endif
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

ifneq ($(filter $(NDK_KNOWN_DEVICE_ABI64S), $(TARGET_ARCH_ABI)),)
LOCAL_ARM_MODE := arm
TARGET_ARCH_ABI := arm64-v8a
LOCAL_CFLAGS := -o3 -march=armv8-a --signed-char -std=c99
LOCAL_CFLAGS += -DARCH_ANDROID -DARM_64 -mfpu=neon -ffast-math -mfloat-abi=softfp

LOCAL_CXXFLAGS := -o3 -march=armv8-a --signed-char -std=c++11 -w
LOCAL_CXXFLAGS +=  -DARCH_ANDROID -D__ANDROID__ -DARM_64 -mfpu=neon -ffast-math -mfloat-abi=softfp -fexceptions

LOCAL_ARM_NEON := true
else
LOCAL_ARM_MODE := arm
TARGET_ARCH_ABI := armeabi-v7a
LOCAL_CFLAGS := -o3 -march=armv7-a --signed-char -std=c99
LOCAL_CFLAGS += -DARCH_ANDROID -DARM_32 -mfpu=neon -ffast-math -mfloat-abi=softfp -mtune=cortex-a8

LOCAL_CXXFLAGS := -o3 -march=armv7-a --signed-char -std=c++11 -w
LOCAL_CXXFLAGS +=  -DARCH_ANDROID -D__ANDROID__ -DARM_32 -mfpu=neon -ffast-math -mfloat-abi=softfp -mtune=cortex-a8 -fexceptions

LOCAL_ARM_NEON := true
endif

LOCAL_LDLIBS :=-llog


LOCAL_MODULE    := StreamParserJni_Android

LOCAL_C_INCLUDES := .

LOCAL_SRC_FILES := StreamParser.cpp
LOCAL_SHARED_LIBRARIES := foo-prebuilt

include $(BUILD_SHARED_LIBRARY)