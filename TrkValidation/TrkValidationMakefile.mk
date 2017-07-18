
fccsw = ${PWD}
pwd = ${PWD}/TrkValidation/options/
datadir = $(subst options,data,$(pwd))
#$(info datadir  is $(datadir))
plotdir = $(subst options,data,$(pwd))
jobfiles = $(shell find $(pwd)/ -name  *.py)
datafilestemp = $(subst .py,.root,$(jobfiles))
datafiles = $(subst options,data,$(datafilestemp))

FILTER = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v)))

publication_dir = ${HOME}/www/pileup

.PRECIOUS: $(datafiles)

#PileupStudy/data/min_bias_pool.root: PileupStudy/options/min_bias_pool.py
#	./run gaudirun.py $< > PileupStudy/data/min_bias_pool.log


.SECONDEXPANSION: 
%.root: OBJECTNAME=$(subst .root,.py,$@) 
%.root: OBJPREREQ=$(subst data,options,$(OBJECTNAME))


%.root: $(OBJPREREQ) 
	./run gaudirun.py $(OBJPREREQ) ${ARGS} > $(subst .root,.log,$@)

.PHONY: all
all: $(datafiles) 

TrkValidation/data/MaterialScan/material_scan.root: Examples/options/material_scan.py
	./run gaudirun.py $< ${ARGS} > $(subst .root,.log,$@) && mv DD4hep_material_scan.root $@

.PHONY: MaterialScan
MaterialScan: TrkValidation/data/MaterialScan/material_scan.root
	cd TrkValidation/data/MaterialScan && $(fccsw)/run python $(fccsw)/Examples/scripts/material_plots.py --fname material_scan.root 
	cd TrkValidation/data/MaterialScan && mkdir -p $(publication_dir)/TrkValidation/data/MaterialScan && cp *.pdf $(publication_dir)/TrkValidation/data/MaterialScan

.PHONY: GeantinoScan
GeantinoScan: $(call FILTER,GeantinoScan, $(datafiles))
	python  TrkValidation/scripts/plot_pos.py TrkValidation/data/GeantinoScan/chargedGeantinoScan-etaScan.root

.PHONY: ExtrapolationScan
ExtrapolationScan: $(call FILTER,extrapolationTests, $(datafiles))
	python  TrkValidation/scripts/plot_pos.py TrkValidation/data/extrapolationTest.py

.PHONY: SingleParticleMomentumResolution
SingleParticleMomentumResolution: $(call FILTER,fit_gean, $(datafiles))


	
	

