LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := com.byteandahalf.betterdrops
LOCAL_SRC_FILES := main.cpp

LOCAL_LDLIBS    := -L$(LOCAL_PATH) -llog -ldl -lmcpelauncher_tinysubstrate

TARGET_NO_UNDEFINED_LDFLAGS :=

include $(BUILD_SHARED_LIBRARY)
