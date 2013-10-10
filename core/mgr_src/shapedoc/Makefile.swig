ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =shapedoc

INCLUDES += -I$(ROOTDIR)/core/pub_inc \
            -I$(ROOTDIR)/core/pub_inc/geom \
            -I$(ROOTDIR)/core/pub_inc/graph \
            -I$(ROOTDIR)/core/pub_inc/canvas \
            -I$(ROOTDIR)/core/pub_inc/shape \
            -I$(ROOTDIR)/core/pub_inc/storage \
            -I$(ROOTDIR)/core/mgr_inc/shapedoc

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../../pub_src/geom/*.cpp) \
           $(wildcard ../../pub_src/graph/*.cpp) \
           $(wildcard ../../pub_src/shape/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
