ROOTDIR     =../../..
TARGET      =libgview.a
SRCS        =$(wildcard *.cpp)
OBJS        =$(SRCS:.cpp=.o)
INSTALL_DIR ?=$(ROOTDIR)/build

CPPFLAGS    += -Wall \
               -I$(ROOTDIR)/core/pub_inc \
               -I$(ROOTDIR)/core/pub_inc/geom \
               -I$(ROOTDIR)/core/pub_inc/graph \
               -I$(ROOTDIR)/core/pub_inc/canvas \
               -I$(ROOTDIR)/core/pub_inc/shape \
               -I$(ROOTDIR)/core/pub_inc/storage \
               -I$(ROOTDIR)/core/pub_inc/cmd \
               -I$(ROOTDIR)/core/pub_inc/cmdobserver \
               -I$(ROOTDIR)/core/pub_inc/cmdbase \
               -I$(ROOTDIR)/core/mgr_inc/shapedoc \
               -I$(ROOTDIR)/core/mgr_inc/json \
               -I$(ROOTDIR)/core/mgr_inc/cmdbasic \
               -I$(ROOTDIR)/core/mgr_inc/cmdmgr \
               -I$(ROOTDIR)/core/mgr_inc/view \
               -I$(ROOTDIR)/core/mgr_inc/test

all:        $(TARGET)
$(TARGET):  $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	@rm -rfv *.o *.a
ifdef touch
	@touch -c *
endif

install:
	@test -d $(INSTALL_DIR) || mkdir $(INSTALL_DIR)
	@! test -e $(TARGET) || cp -v $(TARGET) $(INSTALL_DIR)
