
fccsw = ${PWD}
pwd = ${PWD}/TrkValidation/options
datadir = $(subst options,data,$(pwd))
$(info datadir  is $(datadir))
plotdir = $(subst options,data,$(pwd))
jobfiles = $(shell find $(pwd)/ -name  *.py)
datafilestemp = $(subst .py,.root,$(jobfiles))
datafiles = $(subst options,data,$(datafilestemp))

FILTER = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v)))


.PRECIOUS: $(datafiles)

#PileupStudy/data/min_bias_pool.root: PileupStudy/options/min_bias_pool.py
#	./run gaudirun.py $< > PileupStudy/data/min_bias_pool.log
$(datadir)%.root: $(pwd)%.py
	 		./run gaudirun.py $< ${ARGS} > $(subst .root,.log,$@)
.PHONY: all
all: $(datafiles) 

.PHONY: MaterialScan
MaterialScan: TrkValidation/data/MaterialScan/material_scan.root
	cd TrkValidation/data/MaterialScan && $(fccsw)/run python $(fccsw)/Examples/scripts/material_plots.py --fname material_scan.root 

.PHONY: GeantinoScan
GeantinoScan: $(call FILTER,eantino, $(datafiles))

	
	

