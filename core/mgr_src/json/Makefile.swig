ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =json

INCLUDES += -I$(ROOTDIR)/core/pub_inc \
            -I$(ROOTDIR)/core/pub_inc/storage \
            -I$(ROOTDIR)/core/mgr_inc/json

SOURCES   =$(wildcard *.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
