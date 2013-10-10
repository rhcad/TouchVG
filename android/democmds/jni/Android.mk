LOCAL_PATH := $(call my-dir)
PUBINC     := $(LOCAL_PATH)/../../../core/pub_inc
CMDINC     := $(LOCAL_PATH)/../../../core/democmds
PUBSRC     := ../../../core/pub_src
CMDSRC     := ../../../core/democmds

include $(CLEAR_VARS)

LOCAL_MODULE := democmds
LOCAL_LDLIBS           := -L$(SYSROOT)/usr/lib -llog
LOCAL_PRELINK_MODULE   := false
LOCAL_CFLAGS           := -frtti -Wall -Wextra

ifeq ($(TARGET_ARCH),arm)
# Ignore "note: the mangling of 'va_list' has changed in GCC 4.4"
LOCAL_CFLAGS += -Wno-psabi
endif

LOCAL_C_INCLUDES := $(PUBINC) \
                    $(PUBINC)/geom \
                    $(PUBINC)/graph \
                    $(PUBINC)/canvas \
                    $(PUBINC)/shape \
                    $(PUBINC)/storage \
                    $(PUBINC)/cmd \
                    $(PUBINC)/cmdbase \
                    $(PUBINC)/cmdobserver \
                    $(CMDINC)/gate \
                    $(CMDINC)/cmds

LOCAL_SRC_FILES  := $(PUBSRC)/geom/mgmat.cpp \
                    $(PUBSRC)/geom/mgbase.cpp \
                    $(PUBSRC)/geom/mgbnd.cpp \
                    $(PUBSRC)/geom/mgbox.cpp \
                    $(PUBSRC)/geom/mgcurv.cpp \
                    $(PUBSRC)/geom/mglnrel.cpp \
                    $(PUBSRC)/geom/mgnear.cpp \
                    $(PUBSRC)/geom/mgnearbz.cpp \
                    $(PUBSRC)/geom/mgvec.cpp \
                    $(PUBSRC)/graph/gipath.cpp \
                    $(PUBSRC)/graph/gixform.cpp \
                    $(PUBSRC)/graph/gigraph.cpp \
                    $(PUBSRC)/shape/mgcomposite.cpp \
                    $(PUBSRC)/shape/mgellipse.cpp \
                    $(PUBSRC)/shape/mggrid.cpp \
                    $(PUBSRC)/shape/mgline.cpp \
                    $(PUBSRC)/shape/mglines.cpp \
                    $(PUBSRC)/shape/mgrdrect.cpp \
                    $(PUBSRC)/shape/mgrect.cpp \
                    $(PUBSRC)/shape/mgshape.cpp \
                    $(PUBSRC)/shape/mgshapes.cpp \
                    $(PUBSRC)/shape/mgsplines.cpp \
                    $(PUBSRC)/cmdbase/mgcmddraw.cpp \
                    $(PUBSRC)/cmdbase/mgdrawarc.cpp \
                    $(PUBSRC)/cmdbase/mgdrawrect.cpp \
                    $(PUBSRC)/cmdbase/mgsplock.cpp \
                    $(CMDSRC)/gate/DemoCmds.cpp \
                    $(CMDSRC)/cmds/cmds.cpp \
                    $(CMDSRC)/cmds/HitTestCmd.cpp \
                    democmds_java_wrap.cpp

include $(BUILD_SHARED_LIBRARY)
