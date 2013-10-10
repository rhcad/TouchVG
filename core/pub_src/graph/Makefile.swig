ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =graph

INCLUDES += -I$(ROOTDIR)/core/pub_inc \
            -I$(ROOTDIR)/core/pub_inc/geom \
            -I$(ROOTDIR)/core/pub_inc/graph \
            -I$(ROOTDIR)/core/pub_inc/canvas

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../geom/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
