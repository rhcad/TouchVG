# The simplest way to compile this project on MinGW, Cygwin, Linux or Mac OS X is:
#
# 1. `cd' to the directory containing the file of 'Makefile'.
#
# 2. Type `make` or `make all install` for C++ applications.
#    Type `make java`, `make python` or `make perl` for more language applications.
#    The program binaries files are outputed to '../build'.
# 
# 3. You can remove the program object files from the source code directory.
#    Type `make clean` to remove object files for C++ applications.
#    Type `make java.clean` to remove object files for Java applications.
#
# Readme about variables: https://github.com/rhcad/x3py/wiki/MakeVars
#
SUBDIRS         =$(subst /,,$(dir $(wildcard */)))
CLEANDIRS       =$(addsuffix .clean, $(SUBDIRS))
INSTALLDIRS     =$(addsuffix .install, $(SUBDIRS))
SWIGDIRS        =$(addsuffix .swig, $(SUBDIRS))
SWIGS           =python perl5 java csharp ruby php lua r
CLEANSWIGS      =$(addsuffix .clean, $(SWIGS))
CLEANALLSWIGS   =$(addsuffix .cleanall, $(SWIGS))

.PHONY:     $(SUBDIRS) clean install
all:        $(SUBDIRS)
clean:      $(CLEANDIRS)
install:    $(INSTALLDIRS)
swig:       $(SWIGDIRS)

ifdef SWIG_TYPE
makefile    =Makefile.swig
else
makefile    =Makefile
endif

$(SUBDIRS):
	@! test -e $@/Makefile || $(MAKE) -C $@

$(SWIGDIRS):
	@ ! test -e $(basename $@)/$(makefile) || \
	$(MAKE) -C $(basename $@) -f $(makefile) swig

$(CLEANDIRS) $(INSTALLDIRS):
	@ ! test -e $(basename $@)/$(makefile) || \
	$(MAKE) -C $(basename $@) -f $(makefile) $(subst .,,$(suffix $@))

$(SWIGS):
	@test -d ../build || mkdir ../build
	@export SWIG_TYPE=$@; $(MAKE) swig
	
$(CLEANSWIGS):
	@export SWIG_TYPE=$(basename $@); \
	export clean=1; $(MAKE) clean

$(CLEANALLSWIGS):
	@export SWIG_TYPE=$(basename $@); export cleanall=1; \
	export clean=1; $(MAKE) clean
