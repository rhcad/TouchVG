# The simplest way to compile TouchVG on MinGW, Cygwin, Linux or Mac OS X is:
#
# 1. `cd' to the directory containing the file of 'Makefile'.
#
# 2. Type `make java' to compile the swig projects for java.
#    The program binaries files are outputed to './build/java'.
#
# 3. Type `make all install' for C++ applications.
#    Type `make python' or `make perl' for more language applications.
# 
# 4. You can remove the program object files from the source code
#    directory by typing `make java.clean'.
#    Type `make clean' to remove object files for C++ applications.
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

$(SUBDIRS):
	@! test -e $@/Makefile || $(MAKE) -C $@

$(SWIGDIRS):
	@ ! test -e $(basename $@)/Makefile || \
	$(MAKE) -C $(basename $@) swig

$(CLEANDIRS) $(INSTALLDIRS):
	@ ! test -e $(basename $@)/Makefile || \
	$(MAKE) -C $(basename $@) $(subst .,,$(suffix $@))

$(SWIGS):
	@test -d ../build || mkdir ../build
	@export SWIG_TYPE=$@; $(MAKE) swig
	
$(CLEANSWIGS):
	@export SWIG_TYPE=$(basename $@); \
	export clean=1; $(MAKE) clean

$(CLEANALLSWIGS):
	@export SWIG_TYPE=$(basename $@); export cleanall=1; \
	export clean=1; $(MAKE) clean