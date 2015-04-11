ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =gshape

INCLUDES += -I$(ROOTDIR)/core/include \
            -I$(ROOTDIR)/core/include/geom \
            -I$(ROOTDIR)/core/include/gshape \
            -I$(ROOTDIR)/core/include/storage

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../geom/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
