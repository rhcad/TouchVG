ROOTDIR  =../../..
PKGNAME  =touchvg
PROJNAME =gview
SWIGFILE =touchvg.swig

INCLUDES += -I$(ROOTDIR)/core/include \
            -I$(ROOTDIR)/core/include/geom \
            -I$(ROOTDIR)/core/include/graph \
            -I$(ROOTDIR)/core/include/canvas \
            -I$(ROOTDIR)/core/include/gshape \
            -I$(ROOTDIR)/core/include/shape \
            -I$(ROOTDIR)/core/include/storage \
            -I$(ROOTDIR)/core/include/cmd \
            -I$(ROOTDIR)/core/include/cmdobserver \
            -I$(ROOTDIR)/core/include/cmdbase \
            -I$(ROOTDIR)/core/include/cmdbasic \
            -I$(ROOTDIR)/core/include/shapedoc \
            -I$(ROOTDIR)/core/include/jsonstorage \
            -I$(ROOTDIR)/core/include/cmdmgr \
            -I$(ROOTDIR)/core/include/view \
            -I$(ROOTDIR)/core/include/export \
            -I$(ROOTDIR)/core/include/record \
            -I$(ROOTDIR)/core/include/test

SOURCES   =$(wildcard *.cpp) \
           $(wildcard ../geom/*.cpp) \
           $(wildcard ../graph/*.cpp) \
           $(wildcard ../gshape/*.cpp) \
           $(wildcard ../shape/*.cpp) \
           $(wildcard ../cmdbase/*.cpp) \
           $(wildcard ../shapedoc/*.cpp) \
           $(wildcard ../cmdbasic/*.cpp) \
           $(wildcard ../cmdmgr/*.cpp) \
           $(wildcard ../jsonstorage/*.cpp) \
           $(wildcard ../export/*.cpp) \
           $(wildcard ../record/*.cpp) \
           $(wildcard ../test/*.cpp)

include $(ROOTDIR)/core/mk/config.mk
include $(ROOTDIR)/core/mk/Makefile.swig
