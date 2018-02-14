import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

inputfilename2 = "muons_for_seeding.root"


from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input=inputfilename2)
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=["simParticles", "simVertices", "hits", "positionedHits", "trajectory", "trajectoryPoints"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  ],
  OutputLevel=INFO,
  )


from Configurables import CombinatorialSeedingTest, TrickTrackSeedingTool, BarrelLayerGraphTool

from Configurables import DoubletCreationTool


layergraphtool = BarrelLayerGraphTool()

doublet_tool = DoubletCreationTool()

seed_tool = TrickTrackSeedingTool()
seed_tool.LayerGraphTool = layergraphtool
seed_tool.seedingLayerIndices0=(0,0)
seed_tool.seedingLayerIndices1=(0,1)
seed_tool.seedingLayerIndices2=(0,2)
seed_tool.seedingLayerIndices3=(0,3)
seed_tool.readoutName = "TrackerBarrelReadout"

from Configurables import TruthSeedingTool
truth_seeds = TruthSeedingTool()

combi_seeding = CombinatorialSeedingTest()

combi_seeding.TrackSeedingTool = truth_seeds
combi_seeding.positionedTrackHits.Path = "positionedHits"


# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="tricktrackSeeding_Example.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, combi_seeding, out],
                EvtSel = 'NONE',
                EvtMax   = 3,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice],
                OutputLevel=DEBUG
 )
