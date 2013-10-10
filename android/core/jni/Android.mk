# Copyright (c) 2013, Zhang Yungui, https://github.com/rhcad/touchvg
#
LOCAL_PATH := $(call my-dir)
PUBINC     := $(LOCAL_PATH)/../../../core/pub_inc
MGRINC     := $(LOCAL_PATH)/../../../core/mgr_inc
PUBSRC     := ../../../core/pub_src
MGRSRC     := ../../../core/mgr_src

include $(CLEAR_VARS)

LOCAL_MODULE := touchvg
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
                    $(MGRINC)/test \
                    $(MGRINC)/view \
                    $(MGRINC)/cmdbasic \
                    $(MGRINC)/cmdmgr \
                    $(MGRINC)/json \
                    $(MGRINC)/shapedoc

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
                    $(MGRSRC)/cmdbasic/cmdbasic.cpp \
                    $(MGRSRC)/cmdbasic/mgcmderase.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawcircle.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawdiamond.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawellipse.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawfreelines.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawgrid.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawline.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawlines.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawparallel.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawpolygon.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawsplines.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawsquare.cpp \
                    $(MGRSRC)/cmdbasic/mgdrawtriang.cpp \
                    $(MGRSRC)/cmdbasic/mgbasicspreg.cpp \
                    $(MGRSRC)/cmdmgr/mgactions.cpp \
                    $(MGRSRC)/cmdmgr/mgcmdmgr_.cpp \
                    $(MGRSRC)/cmdmgr/mgcmdmgr2.cpp \
                    $(MGRSRC)/cmdmgr/mgsnapimpl.cpp \
                    $(MGRSRC)/cmdmgr/cmdsubject.cpp \
                    $(MGRSRC)/cmdmgr/mgcmdselect.cpp \
                    $(MGRSRC)/shapedoc/mgshapedoc.cpp \
                    $(MGRSRC)/shapedoc/mglayer.cpp \
                    $(MGRSRC)/shapedoc/mglockrw.cpp \
                    $(MGRSRC)/json/mgjsonstorage.cpp \
                    $(MGRSRC)/view/gicoreview.cpp \
                    $(MGRSRC)/view/GcShapeDoc.cpp \
                    $(MGRSRC)/view/GcGraphView.cpp \
                    $(MGRSRC)/view/GcMagnifierView.cpp \
                    $(MGRSRC)/test/testcanvas.cpp \
                    $(MGRSRC)/test/RandomShape.cpp \
                    touchvg_java_wrap.cpp

include $(BUILD_SHARED_LIBRARY)
