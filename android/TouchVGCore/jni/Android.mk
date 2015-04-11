# Copyright (c) 2013, Zhang Yungui, https://github.com/rhcad/touchvg

LOCAL_PATH := $(call my-dir)
cflags     := -frtti -Wall -Wextra -Wno-unused-parameter

core_inc   := $(call my-dir)/../../../core/include
shape_incs := $(core_inc) \
              $(core_inc)/canvas \
              $(core_inc)/geom \
              $(core_inc)/graph \
              $(core_inc)/jsonstorage \
              $(core_inc)/gshape \
              $(core_inc)/shape \
              $(core_inc)/storage \
              $(core_inc)/shapedoc \
              $(core_inc)/test

cmd_incs   := $(core_inc)/cmd \
              $(core_inc)/cmdbase \
              $(core_inc)/cmdobserver \
              $(core_inc)/cmdbasic \
              $(core_inc)/cmdmgr
              
view_incs  := $(core_inc)/view \
              $(core_inc)/export \
              $(core_inc)/record

core_src   := ../../../core/src

geom_files := $(core_src)/geom/mgbase.cpp \
              $(core_src)/geom/mgbox.cpp \
              $(core_src)/geom/mgcurv.cpp \
              $(core_src)/geom/mglnrel.cpp \
              $(core_src)/geom/mgmat.cpp \
              $(core_src)/geom/mgnear.cpp \
              $(core_src)/geom/mgnearbz.cpp \
              $(core_src)/geom/fitcurves.cpp \
              $(core_src)/geom/mgvec.cpp \
              $(core_src)/geom/mgpnt.cpp \
              $(core_src)/geom/mgpath.cpp \
              $(core_src)/geom/nanosvg.cpp

graph_files := $(core_src)/graph/gigraph.cpp \
              $(core_src)/graph/gixform.cpp

json_files := $(core_src)/jsonstorage/mgjsonstorage.cpp

gshape_files := $(core_src)/gshape/mgarc.cpp \
              $(core_src)/gshape/mgbasesp.cpp \
              $(core_src)/gshape/mgcshapes.cpp \
              $(core_src)/gshape/mgdiamond.cpp \
              $(core_src)/gshape/mgdot.cpp \
              $(core_src)/gshape/mgellipse.cpp \
              $(core_src)/gshape/mggrid.cpp \
              $(core_src)/gshape/mgline.cpp \
              $(core_src)/gshape/mglines.cpp \
              $(core_src)/gshape/mgparallel.cpp \
              $(core_src)/gshape/mgpathsp.cpp \
              $(core_src)/gshape/mgrdrect.cpp \
              $(core_src)/gshape/mgrect.cpp \
              $(core_src)/gshape/mgsplines.cpp \
              $(core_src)/gshape/mgarccross.cpp

shape_files := $(core_src)/shape/mgcomposite.cpp \
              $(core_src)/shape/mgimagesp.cpp \
              $(core_src)/shape/mgshape.cpp \
              $(core_src)/shape/mgshapes.cpp \
              $(core_src)/shape/mgbasicspreg.cpp

doc_files  := $(core_src)/shapedoc/mgshapedoc.cpp \
              $(core_src)/shapedoc/mglayer.cpp \
              $(core_src)/shapedoc/spfactoryimpl.cpp

test_files := $(core_src)/test/testcanvas.cpp \
              $(core_src)/test/RandomShape.cpp

base_files := $(core_src)/cmdbase/mgcmddraw.cpp \
              $(core_src)/cmdbase/mgdrawarc.cpp \
              $(core_src)/cmdbase/mgdrawline.cpp \
              $(core_src)/cmdbase/mgdrawrect.cpp

basic_files := $(core_src)/cmdbasic/cmdbasic.cpp \
              $(core_src)/cmdbasic/mgcmderase.cpp \
              $(core_src)/cmdbasic/mgdrawcircle.cpp \
              $(core_src)/cmdbasic/mgdrawdiamond.cpp \
              $(core_src)/cmdbasic/mgdrawellipse.cpp \
              $(core_src)/cmdbasic/mgdrawfreelines.cpp \
              $(core_src)/cmdbasic/mgdrawgrid.cpp \
              $(core_src)/cmdbasic/mgdrawlines.cpp \
              $(core_src)/cmdbasic/mgdrawparallel.cpp \
              $(core_src)/cmdbasic/mgdrawpolygon.cpp \
              $(core_src)/cmdbasic/mgdrawsplines.cpp \
              $(core_src)/cmdbasic/mgdrawsquare.cpp \
              $(core_src)/cmdbasic/mgdrawtriang.cpp

mgr_files  := $(core_src)/cmdmgr/cmdsubject.cpp \
              $(core_src)/cmdmgr/mgactions.cpp \
              $(core_src)/cmdmgr/mgcmdmgr_.cpp \
              $(core_src)/cmdmgr/mgcmdmgr2.cpp \
              $(core_src)/cmdmgr/mgcmdselect.cpp \
              $(core_src)/cmdmgr/mgsnapimpl.cpp

view_files := $(core_src)/view/GcGraphView.cpp \
              $(core_src)/view/GcMagnifierView.cpp \
              $(core_src)/view/GcShapeDoc.cpp \
              $(core_src)/view/gicoreview.cpp \
              $(core_src)/view/gicorerecord.cpp \
              $(core_src)/export/svgcanvas.cpp \
              $(core_src)/export/girecordcanvas.cpp \
              $(core_src)/record/recordshapes.cpp

include $(CLEAR_VARS)
LOCAL_MODULE     := libTouchVGCore
LOCAL_CFLAGS     := $(cflags)

ifeq ($(TARGET_ARCH),x86)
# For SWIG, http://stackoverflow.com/questions/6753241
LOCAL_CFLAGS += -fno-strict-aliasing
endif

LOCAL_C_INCLUDES := $(shape_incs) $(cmd_incs) $(view_incs)
LOCAL_SRC_FILES  := $(geom_files) $(graph_files) $(gshape_files) $(shape_files) \
                    $(json_files) $(test_files) $(doc_files) \
                    $(base_files) $(basic_files) $(mgr_files) \
                    $(view_files)
include $(BUILD_STATIC_LIBRARY)
