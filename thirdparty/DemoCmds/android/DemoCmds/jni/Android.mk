LOCAL_PATH := $(call my-dir)
LCORE_PATH := ../../../../TouchVGCore/android/TouchVGCore/obj/local/armeabi
CORE_INC   := $(LOCAL_PATH)/../../../../TouchVGCore/core/include
CMDINC     := $(LOCAL_PATH)/../../../core
CMDSRC     := ../../../core

include $(CLEAR_VARS)
LOCAL_MODULE    := libTouchVGCore
LOCAL_SRC_FILES := $(LCORE_PATH)/libTouchVGCore.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := democmds
LOCAL_LDLIBS           := -L$(SYSROOT)/usr/lib -llog
LOCAL_PRELINK_MODULE   := false
LOCAL_CFLAGS           := -frtti -Wall -Wextra -Wno-unused-parameter
LOCAL_STATIC_LIBRARIES := libTouchVGCore

ifeq ($(TARGET_ARCH),arm)
# Ignore "note: the mangling of 'va_list' has changed in GCC 4.4"
LOCAL_CFLAGS += -Wno-psabi
endif

LOCAL_C_INCLUDES := $(CORE_INC) \
                    $(CORE_INC)/geom \
                    $(CORE_INC)/graph \
                    $(CORE_INC)/canvas \
                    $(CORE_INC)/shape \
                    $(CORE_INC)/storage \
                    $(CORE_INC)/cmd \
                    $(CORE_INC)/cmdbase \
                    $(CORE_INC)/cmdobserver \
                    $(CMDINC)/gate \
                    $(CMDINC)/cmds

LOCAL_SRC_FILES  := $(CMDSRC)/gate/DemoCmds.cpp \
                    $(CMDSRC)/cmds/cmds.cpp \
                    $(CMDSRC)/cmds/HitTestCmd.cpp \
                    democmds_java_wrap.cpp

include $(BUILD_SHARED_LIBRARY)
