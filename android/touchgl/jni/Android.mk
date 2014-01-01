# Copyright (c) 2013, Zhang Yungui, https://github.com/rhcad/touchvg
#
LOCAL_PATH := $(call my-dir)
VGINC      := $(LOCAL_PATH)/../../../core/include
VGSRC      := ../../../core/src
CORE_PATH  := ../../../thirdparty/TouchVGCore/android/TouchVGCore/obj/local/armeabi
CORE_INC   := $(LOCAL_PATH)/../../../thirdparty/TouchVGCore/core/include
MONK_PATH  := ../../../thirdparty/MonkVG/projects/MonkVG-Android/obj/local/armeabi
MONK_INC   := $(LOCAL_PATH)/../../../thirdparty/MonkVG/include

include $(CLEAR_VARS)
LOCAL_MODULE    := libTouchVGCore
LOCAL_SRC_FILES := $(CORE_PATH)/libTouchVGCore.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenVG
LOCAL_SRC_FILES := $(MONK_PATH)/libOpenVG.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenVGU
LOCAL_SRC_FILES := $(MONK_PATH)/libOpenVGU.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE           := touchgl
LOCAL_LDLIBS           := -L$(SYSROOT)/usr/lib -llog -lGLESv1_CM -lGLESv2
LOCAL_PRELINK_MODULE   := false
LOCAL_CFLAGS           := -frtti -Wall -Wextra -Wno-unused-parameter
LOCAL_STATIC_LIBRARIES := libTouchVGCore libOpenVGU libOpenVG

ifeq ($(TARGET_ARCH),arm)
# Ignore "note: the mangling of 'va_list' has changed in GCC 4.4"
LOCAL_CFLAGS += -Wno-psabi
endif

LOCAL_C_INCLUDES := $(CORE_INC) \
                    $(CORE_INC)/graph \
                    $(CORE_INC)/canvas \
                    $(CORE_INC)/storage \
                    $(CORE_INC)/cmd \
                    $(CORE_INC)/view \
                    $(MONK_INC)/MonkVG \
                    $(VGINC)/touchgl

LOCAL_SRC_FILES  := $(VGSRC)/touchgl/GiOpenVGCanvas.cpp \
                    $(VGSRC)/touchgl/touchglview.cpp \
                    touchgl_java_wrap.cpp

include $(BUILD_SHARED_LIBRARY)
