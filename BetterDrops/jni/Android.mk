LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := com.byteandahalf.betterdrops # should match the name in the AndroidManifest.xml
LOCAL_SRC_FILES_RAW := $(shell find $(LOCAL_PATH) -name '*.cpp') $(shell find $(LOCAL_PATH) -name '*.c')
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES_RAW:$(LOCAL_PATH)/%=%)
# search for libraries in the jni/ folder of our project,
# link to the libraries for printing to logcat, getting methods from libminecraftpe,
# and for accessing BlockLauncher functions
LOCAL_LDLIBS    := -L$(LOCAL_PATH) -llog -ldl -lminecraftpe -lmcpelauncher_tinysubstrate

include $(BUILD_SHARED_LIBRARY)
