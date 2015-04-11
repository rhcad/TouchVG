# Included by Makefile of swig project.
# TouchVG framework: https://github.com/rhcad/touchvg

SOURCES ?=$(wildcard *.cpp)
OBJDIR0  =._$(SWIG_TYPE)o/
OBJDIR   =._$(SWIG_TYPE)o/o/o/
OBJS     =$(addprefix $(OBJDIR), $(SOURCES:.cpp=$(OBJEXT)))

# SWIG_TYPE == ?
USE_PYTHON   :=$(shell echo $(SWIG_TYPE)|grep -i python)
USE_PERL     :=$(shell echo $(SWIG_TYPE)|grep -i perl)
USE_JAVA     :=$(shell echo $(SWIG_TYPE)|grep -i java)
USE_CSHARP   :=$(shell echo $(SWIG_TYPE)|grep -i csharp)
USE_RUBY     :=$(shell echo $(SWIG_TYPE)|grep -i ruby)
USE_PHP      :=$(shell echo $(SWIG_TYPE)|grep -i php)

# The extension of the target file to compile.
ifdef IS_WIN
ifdef USE_PYTHON
SWIG_EXT      =.pyd
else
SWIG_EXT      =.dll
endif
else
SWIG_EXT      =.so
endif

# The prefix of the target file to compile.
ifdef USE_PYTHON
SWIG_PRE      =_
else
ifdef USE_CSHARP
SWIG_PRE      =_
else
ifndef IS_WIN
SWIG_PRE      =lib
endif
endif
endif

ifdef USE_PYTHON
SWIG_INCLUDE ?=$(PYTHON_INCLUDE)
SWIG_LIBPATH :=$(PYTHON_LIB)
SWIG_LIBFILE :=$(PYTHON_LIBFILE)
SWIG_TESTEXT  =.py
WRAPTARGET    =$(PROJNAME).py
CPPFLAGS     += -DUSE_PYTHON
endif

ifdef USE_PERL
SWIG_INCLUDE ?=$(PERL5_INCLUDE)
SWIG_LIBPATH :=$(PERL5_LIB)
SWIG_LIBFILE :=$(PERL5_LIBFILE)
SWIG_TESTEXT  =.pl
WRAPTARGET    =$(PROJNAME).pm
CPPFLAGS     += -DUSE_PERL
endif

ifdef USE_JAVA
SWIG_INCLUDE ?=$(JAVA_INCLUDE)
SWIG_TESTEXT  =.java
WRAPTARGET    =$(PROJNAME).jar
CPPFLAGS     += -DUSE_JAVA
endif

ifdef USE_CSHARP
SWIG_TESTEXT  =.cs
WRAPTARGET    =$(PROJNAME)$(SWIG_EXT)
CPPFLAGS     += -DUSE_CSHARP
endif

ifdef USE_RUBY
SWIG_INCLUDE ?=$(RUBY_INCLUDE)
SWIG_LIBPATH :=$(RUBY_LIB)
SWIG_LIBFILE :=$(RUBY_LIBFILE)
SWIG_TESTEXT  =.rb
WRAPTARGET    =$(PROJNAME).rb
CPPFLAGS     += -DUSE_RUBY
endif

ifdef USE_PHP
SWIG_INCLUDE ?=$(PHP_INCLUDE)
SWIG_LIBPATH :=$(PHP_LIB)
SWIG_LIBFILE :=$(PHP_LIBFILE)
SWIG_TESTEXT  =.rb
WRAPTARGET    =$(PROJNAME).php
CPPFLAGS     += -DUSE_PHP
endif

ifdef SWIG_INCLUDE
INCLUDES     += -I"$(SWIG_INCLUDE)"
ifdef IS_WIN
INCLUDES     += -I"$(SWIG_INCLUDE)\win32"
endif
ifdef USE_PHP
INCLUDES     += -I"$(SWIG_INCLUDE)/main" -I"$(SWIG_INCLUDE)/TSRM" -I"$(SWIG_INCLUDE)/Zend"
INCLUDES     += -I"$(INSTALL_DIR)/$(SWIG_TYPE)"
endif
endif
ifdef SWIG_LIBPATH
LIBS         += $(LIBPATHFLAG)"$(SWIG_LIBPATH)"
endif
ifdef SWIG_LIBFILE
LIBS         += $(LIBFLAG)$(SWIG_LIBFILE)$(LIBEND)
endif
LIBS         += $(LIBPATHFLAG)$(INSTALL_DIR)

ifdef USE_JAVA
SWIGTMP       =._$(SWIG_TYPE)
else
ifdef USE_CSHARP
SWIGTMP       =._$(SWIG_TYPE)
endif
endif

SWIG_TESTDIR ?=$(INSTALL_DIR)/$(SWIG_TYPE)
ifdef USE_PYTHON
SWIG_OUTDIR  ?=$(SWIG_TESTDIR)/$(PKGNAME)
else
SWIG_OUTDIR  ?=$(SWIG_TESTDIR)
endif

SWIG_CC       ="$(SWIG_BIN)swig" -c++ -$(SWIG_TYPE)
ifdef USE_CSHARP
SWIG_CC      += -namespace $(PKGNAME).$(PROJNAME) -dllimport _$(PROJNAME)
endif
ifdef USE_JAVA
SWIG_CC      += -package $(PKGNAME).$(PROJNAME)
endif

ifdef SWIGTMP
ifdef USE_JAVA
JAVATMPDIR    =$(SWIGTMP)/$(PKGNAME)/$(PROJNAME)
SWIG_CC      += -outdir $(JAVATMPDIR)
else
SWIG_CC      += -outdir $(SWIGTMP)
endif
else
SWIG_CC      += -outdir $(SWIG_OUTDIR)
endif

USE_TDMGCC   :=$(shell g++ --version|grep -i tdm)
ifdef USE_TDMGCC
SWIG_CC      += -D__MINGW32__
endif

#------------------------------------------------

TARGET              =$(SWIG_PRE)$(PROJNAME)$(SWIG_EXT)
WRAPFILE            =$(PROJNAME)_$(SWIG_TYPE)_wrap
OBJS               += $(OBJDIR)$(WRAPFILE)$(OBJEXT)
TESTFILE           :=test$(PROJNAME)$(SWIG_TESTEXT)
REMOTE_TARGET       =$(SWIG_OUTDIR)/$(TARGET)
REMOTE_WRAPTARGET   =$(SWIG_OUTDIR)/$(WRAPTARGET)
REMOTE_TESTFILE    :=$(SWIG_TESTDIR)/$(TESTFILE)
HASTESTFILE         =(test -e $(TESTFILE))

.PHONY: swig clean swigonly $(TESTFILE)

all:
swig: $(REMOTE_TARGET) $(REMOTE_WRAPTARGET) $(REMOTE_TESTFILE)
swigonly: $(REMOTE_WRAPTARGET)

$(REMOTE_TARGET): $(OBJS)
	-@mkdir -vp $(SWIG_TESTDIR)
	-@mkdir -vp $(SWIG_OUTDIR)
	$(LINK) $(LDFLAGS) $(OUTFLAG)$@ $(OBJS) $(LIBS)

$(OBJDIR)%$(OBJEXT): %.cxx
	-@mkdir -vp $(OBJDIR)
	$(CPP) $(subst -Wall ,,$(subst -W4 ,,$(CPPFLAGS))) $(INCLUDES) -c $< $(OFLAG)$@

HAS_PUBSRC      :=$(shell echo $(SOURCES)|grep -i pub_src)

$(OBJDIR)%.d: %.cpp
	-@mkdir -vp $(OBJDIR)
ifdef HAS_PUBSRC
	-@mkdir -vp $(OBJDIR0)pub_src
endif
	-@mkdir -vp $(OBJDIR)$(dir $<)
	@echo Generating $@ from $<; $(DEPEND_CC) $@ $(INCLUDES) $<

$(OBJDIR)%$(OBJEXT): %.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< $(OFLAG)$@

ifndef clean
include $(addprefix $(OBJDIR), $(SOURCES:.cpp=.d))
endif

#------------------------------------------------
SWIGFILE       ?=$(PROJNAME).i
$(WRAPFILE).cxx: $(SWIGFILE)
	@echo --Making $(SWIG_TYPE) project $(PROJNAME)...
	-@mkdir -vp $(SWIG_TESTDIR)
	-@mkdir -vp $(SWIG_OUTDIR)
ifdef SWIGTMP
	-@test -d $(SWIGTMP) || mkdir -v $(SWIGTMP); rm -rf $(SWIGTMP)/*.*
ifdef USE_JAVA
	-@mkdir -vp $(SWIGTMP)/$(PKGNAME)/$(PROJNAME)
	-@rm -rf $(JAVATMPDIR)/*.*
endif
endif
	$(SWIG_CC) -o $(WRAPFILE).cxx $(INCLUDES) $<
ifdef USE_PYTHON
	@test -e $(SWIG_OUTDIR)/__init__.py || echo "#empty" > $(SWIG_OUTDIR)/__init__.py
endif

#------------------------------------------------

$(REMOTE_WRAPTARGET): $(WRAPFILE).cxx
ifdef SWIGTMP
	@test -d $(SWIGTMP) || (rm -rf $(WRAPFILE).cxx; $(MAKE) $(WRAPFILE).cxx)
endif
ifdef USE_JAVA
	@cd $(JAVATMPDIR); "$(JAVA_BIN)javac" *.java;
	@cd $(SWIGTMP); "$(JAVA_BIN)jar" cfv $(WRAPTARGET) $(PKGNAME)/$(PROJNAME)/*.class; \
	    cp -v $(WRAPTARGET) ../$(SWIG_OUTDIR)
endif
ifdef USE_CSHARP
	@cd $(SWIGTMP); "$(CSC_BIN)csc" -nologo -t:library -out:$(WRAPTARGET) *.cs; \
	    cp -v $(WRAPTARGET) ../$(SWIG_OUTDIR)
endif

#------------------------------------------------

$(TESTFILE):
ifdef SWIG_TESTEXT
	-@!$(HASTESTFILE) || cp -v $(TESTFILE) $(SWIG_TESTDIR)
endif

$(REMOTE_TESTFILE): $(REMOTE_WRAPTARGET) $(TESTFILE)
ifdef SWIG_TESTEXT
ifdef USE_JAVA
	@!$(HASTESTFILE) || (cd $(SWIG_TESTDIR); \
	    "$(JAVA_BIN)javac" -cp $(WRAPTARGET) $(TESTFILE))
endif
ifdef USE_CSHARP
	@!$(HASTESTFILE) || (cd $(SWIG_TESTDIR); \
	    "$(CSC_BIN)csc" -nologo -t:exe -r:$(WRAPTARGET) $(TESTFILE))
endif
endif #SWIG_TESTEXT

#------------------------------------------------

clean:
ifndef SWIG_TYPE
	@rm -rfv *.d *$(OBJEXT)  *_wrap.*  ._*
ifdef touch
	@touch -c *
endif
else
	@rm -rfv *.d $(WRAPFILE).cxx $(WRAPFILE).h  ._$(SWIG_TYPE)*
ifdef cleanall
	@rm -rfv *.cc $(REMOTE_TARGET) $(REMOTE_WRAPTARGET) $(REMOTE_TESTFILE)
endif
endif
