

pwd = ${PWD}/TrkValidation/options
datadir = $(subst options,data,$(pwd))
$(info datadir  is $(datadir))
plotdir = $(subst options,data,$(pwd))
jobfiles = $(shell find "$(pwd)/"*.py)
datafilestemp = $(subst .py,.root,$(jobfiles))
datafiles = $(subst options,data,$(datafilestemp))

.PRECIOUS: $(datafiles)

#PileupStudy/data/min_bias_pool.root: PileupStudy/options/min_bias_pool.py
#	./run gaudirun.py $< > PileupStudy/data/min_bias_pool.log
$(datadir)%.root: $(pwd)%.py
			./run gaudirun.py $< > $(subst .root,.log,$@)
.PHONY: all
all: $(datafiles) 

jobfilesfit = $(shell find "$(pwd)/"single_particle*.py)
datafilestempfit = $(subst .py,.root,$(jobfilesfit))
datafilesfit = $(subst options,data,$(datafilestempfit))
.PHONY: fits
fits: $(datafilesfit)
