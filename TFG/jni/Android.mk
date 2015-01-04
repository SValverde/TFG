LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include D:/OpenCV-2.4.9-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := NativeLib
LOCAL_SRC_FILES := NativeLib.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
