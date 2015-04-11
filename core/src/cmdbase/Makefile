ROOTDIR     =../../..
TARGET      =libcmdbase.a
SRCS        =$(wildcard *.cpp)
OBJS        =$(SRCS:.cpp=.o)
INSTALL_DIR ?=$(ROOTDIR)/build

CPPFLAGS    += -Wall \
               -I$(ROOTDIR)/core/include \
               -I$(ROOTDIR)/core/include/geom \
               -I$(ROOTDIR)/core/include/graph \
               -I$(ROOTDIR)/core/include/gshape \
               -I$(ROOTDIR)/core/include/shape \
               -I$(ROOTDIR)/core/include/storage \
               -I$(ROOTDIR)/core/include/cmd \
               -I$(ROOTDIR)/core/include/cmdbase \
               -I$(ROOTDIR)/core/include/cmdobserver

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
