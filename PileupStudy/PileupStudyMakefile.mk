



pwd = $(CURDIR)/options
datadir = $(subst options,data,$(pwd))
plotdir = $(subst options,data,$(pwd))
jobfiles = $(shell find "$(pwd)/"*.py)
datafilestemp = $(subst .py,.root,$(jobfiles))
datafiles = $(subst options,data,$(datafilestemp))

.PRECIOUS: $(datafiles)

#PileupStudy/data/min_bias_pool.root: PileupStudy/options/min_bias_pool.py
#	./run gaudirun.py $< > PileupStudy/data/min_bias_pool.log
$(datadir)%.root: $(pwd)%.py
		./run gaudirun.py $< > $(subst .py,.log,$<)
.PHONY: all
all:  $(datafiles) 

#.PHONY: clean
#clean:
#	rm RUN*.py RUN*.conf $(datadir)/*.pic $(datadir)/*.dat $(plotdir)/*.dat $(plotdir)/*.png $(plotdir)/*.pdf

