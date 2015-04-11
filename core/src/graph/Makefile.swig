ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =graph

INCLUDES += -I$(ROOTDIR)/core/include/geom \
            -I$(ROOTDIR)/core/include/graph \
            -I$(ROOTDIR)/core/include/canvas

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../geom/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
