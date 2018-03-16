import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from FWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()

inputfile = "triplet_muons_for_seeding.root"
if args.inputfile:
  inputfile = args.inputfile

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input=inputfile)

from Configurables import PodioInput
podioinput = PodioInput("PodioReader", 
                        collections=[
                                      "GenParticles",
                                      "GenVertices",
                                      #"simParticles", 
                                      "SimParticles", 
                                      #"simVertices", 
                                      "SimVertices", 
                                      #"hits", 
                                      "TrackerHits",
                                      #"positionedHits", 
                                      "TrackerPositionedHits",
                                      #"trajectory", 
                                      #"trajectoryPoints",
                                      ], 
                          OutputLevel=INFO,
                          )

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", 
                    detectors=[
                                'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                              ],
                    OutputLevel=INFO,
                   )




# TrickTrack Seeding Configuration
from Configurables import FastHitFilterTool
hitfiltertool = FastHitFilterTool("FastHitFilterTool")


# Alternative: TruthSeeding
from Configurables import TruthSeedingTool
truth_seeds = TruthSeedingTool(
        OutputLevel=DEBUG
        )

from Configurables import TripletSeedingTool
triplet_seeds = TripletSeedingTool(
        OutputLevel=INFO)
triplet_seeds.someParameter = 4.

from Configurables import RecTrackAlg
RecTrackAlg = RecTrackAlg(
        OutputLevel=INFO)
RecTrackAlg.doFit=True
RecTrackAlg.TrackSeedingTool = triplet_seeds
RecTrackAlg.positionedTrackHits.Path = "TrackerPositionedHits"
RecTrackAlg.simParticles.Path = "SimParticles"


# PODIO algorithm
outputfile = "triplet_test.root"
if args.outputfile:
  outputfile = args.outputfile
from Configurables import PodioOutput
out = PodioOutput("out",
                   filename=outputfile,
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]


# get number of events from arguments
nEvents = 2
if args.nevents:
    nEvents = args.nevents

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, RecTrackAlg, out],
                EvtSel = 'NONE',
                EvtMax   = nEvents,
                ExtSvc = [podioevent],
                #OutputLevel=INFO,
 )
