ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =cmdbase

INCLUDES += -I$(ROOTDIR)/core/pub_inc \
            -I$(ROOTDIR)/core/pub_inc/geom \
            -I$(ROOTDIR)/core/pub_inc/graph \
            -I$(ROOTDIR)/core/pub_inc/canvas \
            -I$(ROOTDIR)/core/pub_inc/shape \
            -I$(ROOTDIR)/core/pub_inc/storage \
            -I$(ROOTDIR)/core/pub_inc/cmd \
            -I$(ROOTDIR)/core/pub_inc/cmdbase \
            -I$(ROOTDIR)/core/pub_inc/cmdobserver

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../geom/*.cpp) \
           $(wildcard ../graph/*.cpp) \
           $(wildcard ../shape/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
