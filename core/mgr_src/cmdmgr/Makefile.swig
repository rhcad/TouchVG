ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =cmdmgr

INCLUDES += -I$(ROOTDIR)/core/pub_inc \
            -I$(ROOTDIR)/core/pub_inc/geom \
            -I$(ROOTDIR)/core/pub_inc/graph \
            -I$(ROOTDIR)/core/pub_inc/canvas \
            -I$(ROOTDIR)/core/pub_inc/shape \
            -I$(ROOTDIR)/core/pub_inc/storage \
            -I$(ROOTDIR)/core/pub_inc/cmd \
            -I$(ROOTDIR)/core/pub_inc/cmdobserver \
            -I$(ROOTDIR)/core/mgr_inc/cmdmgr

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../../pub_src/geom/*.cpp) \
           $(wildcard ../../pub_src/graph/*.cpp) \
           $(wildcard ../../pub_src/shape/*.cpp) \
                      ../../pub_src/cmdbase/mgsplock.cpp

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
