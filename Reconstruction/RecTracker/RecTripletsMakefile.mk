############################
# FCC Track Reco Plots Makefile
# run from FCCSW directory
# with 
#
# make -f Reconstruction/RecTracker/RecTripletsMakefile.mk plots
#
#
#
###########################
.PHONY: all
all: plots

plots: triplet_tracks.root
	./run python Reconstruction/RecTracker/scripts/plotTrackSeeds.py triplet_tracks.root
tracks: triplets_muons_for_seeding.root
  ./run fccrun.py Reconstruction/RecTracker/options/TrickTrackSeedingTest.py --nevents 100 --inputfile triplet_muons_for_seeding.root --outputfile triplet_tracks.root

muons: 
	./run gaudirun.py Reconstruction/RecTracker/options/triplets_muons_for_seeding.py
