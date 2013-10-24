ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =export

INCLUDES += -I$(ROOTDIR)/core/pub_inc \
            -I$(ROOTDIR)/core/pub_inc/canvas \
            -I$(ROOTDIR)/core/mgr_inc/export

SOURCES   =$(wildcard *.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
