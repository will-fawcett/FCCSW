import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input="output_geantinos.root")
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections=[ "hits", "positionedHits", "trajectory", "trajectoryPoints"], OutputLevel=DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  ],
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
seed_tool.readoutName = "TrackerBarrelReadout"

combi_seeding = CombinatorialSeedingTest()
combi_seeding.TrackSeedingTool = seed_tool
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
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice],
                OutputLevel=DEBUG
 )
