ROOTDIR  =../..
PKGNAME  =touchvg
PROJNAME =democmds
SWIGFILE =democmds.i

CORE_INC =$(ROOTDIR)/../TouchVGCore/core/include
CORE_SRC =$(ROOTDIR)/../TouchVGCore/core/src

INCLUDES += -I$(CORE_INC) \
            -I$(CORE_INC)/geom \
            -I$(CORE_INC)/graph \
            -I$(CORE_INC)/canvas \
            -I$(CORE_INC)/shape \
            -I$(CORE_INC)/storage \
            -I$(CORE_INC)/cmd \
            -I$(CORE_INC)/cmdbase \
            -I$(CORE_INC)/cmdobserver \
            -I../cmds \
            -I../gate

SOURCES   =$(wildcard *.cpp) \
           $(wildcard $(CORE_SRC)/geom/*.cpp) \
           $(wildcard $(CORE_SRC)/graph/*.cpp) \
           $(wildcard $(CORE_SRC)/shape/*.cpp) \
           $(wildcard $(CORE_SRC)/cmdbase/*.cpp) \
           $(wildcard ../cmds/*.cpp)

include $(ROOTDIR)/../TouchVGCore/core/mk/config.mk
include $(ROOTDIR)/../TouchVGCore/core/mk/Makefile.swig
