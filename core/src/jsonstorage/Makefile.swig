ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =jsonstorage

INCLUDES += -I$(ROOTDIR)/core/include \
            -I$(ROOTDIR)/core/include/storage \
            -I$(ROOTDIR)/core/include/jsonstorage

SOURCES   =$(wildcard *.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
