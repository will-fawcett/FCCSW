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
                                      "allGenParticles",
                                      "allGenVertices",
                                      "simParticles", 
                                      "simVertices", 
                                      "hits", 
                                      "positionedHits", 
                                      "trajectory", 
                                      "trajectoryPoints",
                                      ], 
                          OutputLevel=DEBUG,
                          )
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/triplet/FCCtriplet_1barrel30mm.xml'],
                    OutputLevel = INFO)





# TrickTrack Seeding Configuration
from Configurables import FastHitFilterTool
hitfiltertool = FastHitFilterTool("FastHitFilterTool")

from Configurables import BarrelLayerGraphTool4
layergraphtool = BarrelLayerGraphTool4()

from Configurables import DoubletCreationTool
doublet_tool = DoubletCreationTool()

from Configurables import TrickTrackSeedingTool
tricktrack_seed_tool = TrickTrackSeedingTool()
tricktrack_seed_tool.LayerGraphTool = layergraphtool
tricktrack_seed_tool.deltaZ=360
tricktrack_seed_tool.deltaT=1e12
tricktrack_seed_tool.deltaPhi=0.1

#tricktrack_seed_tool.ptMin = 0.0
#tricktrack_seed_tool.phiCut = 0.2
#tricktrack_seed_tool.thetaCut = 0.000002
#tricktrack_seed_tool.hardPtCut= 0.0
#tricktrack_seed_tool.regionOriginRadius= 0.00001
from Configurables import CombinatorialSeedingTool

seed_tool = CombinatorialSeedingTool()
seed_tool.seedingLayerIndices0=(0,0)
seed_tool.seedingLayerIndices1=(0,1)
seed_tool.seedingLayerIndices2=(0,2)
seed_tool.trackSeeds.Path = "trackSeeds"
seed_tool.readoutName = "TrackerBarrelReadout"

# Alternative: TruthSeeding
from Configurables import TruthSeedingTool
truth_seeds = TruthSeedingTool()

from Configurables import RecTrackAlg
RecTrackAlg = RecTrackAlg()
RecTrackAlg.Errors = False
RecTrackAlg.hitRes = 5*1e-9
RecTrackAlg.TrackSeedingTool = seed_tool#tricktrack_seed_tool
RecTrackAlg.positionedTrackHits.Path = "positionedHits"


# PODIO algorithm
outputfile = "triplet_tracks.root"
if args.outputfile:
  outputfile = args.outputfile
from Configurables import PodioOutput
out = PodioOutput("out",
                   filename=outputfile,
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]


# get number of events from arguments
nEvents = 2
if args.nevents:
    nEvents = args.nevents

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, RecTrackAlg, out],
                EvtSel = 'NONE',
                EvtMax   = nEvents,
                ExtSvc = [podioevent, geoservice],
                OutputLevel=DEBUG,
 )
