ROOTDIR     =../..
TARGET      =libdemocmds.a
SRCS        =$(wildcard *.cpp)
OBJS        =$(SRCS:.cpp=.o)
INSTALL_DIR ?=$(ROOTDIR)/build

CORE_INC    =$(ROOTDIR)/../TouchVGCore/core/include

CPPFLAGS    += -Wall \
               -I$(CORE_INC) \
               -I$(CORE_INC)/geom \
               -I$(CORE_INC)/graph \
               -I$(CORE_INC)/shape \
               -I$(CORE_INC)/cmd \
               -I$(CORE_INC)/cmdbase \
               -I$(CORE_INC)/cmdobserver \
               -I../cmds

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
