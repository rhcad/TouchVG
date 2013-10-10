SUBDIRS         =$(subst /,,$(dir $(wildcard */)))
CLEANDIRS       =$(addsuffix .clean, $(SUBDIRS))
INSTALLDIRS     =$(addsuffix .install, $(SUBDIRS))
SWIGDIRS        =$(addsuffix .swig, $(SUBDIRS))

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
