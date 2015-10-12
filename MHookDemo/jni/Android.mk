LOCAL_PATH := $(call my-dir)
# substrate 环境
include $(CLEAR_VARS)
LOCAL_MODULE:= substrate-dvm
LOCAL_SRC_FILES := libsubstrate-dvm.so
include $(PREBUILT_SHARED_LIBRARY)

# substrate 环境
include $(CLEAR_VARS)
LOCAL_MODULE:= substrate
LOCAL_SRC_FILES := libsubstrate.so
include $(PREBUILT_SHARED_LIBRARY)

# 通用静态库
include $(CLEAR_VARS)
LOCAL_MODULE    := lib-Common
LOCAL_SRC_FILES := Common/Common.cpp
LOCAL_CFLAGS  	:= -Wpointer-arith
include $(BUILD_STATIC_LIBRARY)

#DexFile
include $(CLEAR_VARS)
LOCAL_MODULE    := lib-DexFile
LOCAL_SRC_FILES := DexFile/LDexFile.cpp
LOCAL_CFLAGS  	:= -Wpointer-arith
APP_STL:= gnustl_static			
include $(BUILD_STATIC_LIBRARY)

# MAIN
include $(CLEAR_VARS)
LOCAL_MODULE    := SubstrateHook.cy
LOCAL_SRC_FILES := SubstrateHook.cy.cpp 
LOCAL_CFLAGS  	:= -Wpointer-arith
LOCAL_LDLIBS := -llog
LOCAL_ARM_MODE := arm
LOCAL_LDLIBS += -L$(LOCAL_PATH) -lsubstrate-dvm -lsubstrate
LOCAL_STATIC_LIBRARIES := lib-Common lib-Hook_JNI lib-DexFile
include $(BUILD_SHARED_LIBRARY)
#end
