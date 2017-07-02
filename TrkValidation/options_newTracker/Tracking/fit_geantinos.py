import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from geantinoBaseConfig import *
pgunPt.phiMin = 0
pgunPt.phiMax =0
pgunPt.etaMin = 0
pgunPt.etaMax = 5
pgunPt.energyMin = 100000
pgunPt.energyMax = 100000
geantsim.eventProvider = pgunPt


from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")


from Configurables import TrkVolumeManagerSvc
trkvolmanservice = TrkVolumeManagerSvc("TrkVolMan")

from Configurables import TruthSeedingTool

truthseedtool = TruthSeedingTool()

from Configurables import SaveTrackStateTool

savetrackstatetool = SaveTrackStateTool()
savetrackstatetool.trackStates.Path = "trackStates"

from Configurables import TrackFit
trackFitAlg = TrackFit()
trackFitAlg.trackSeedingTool = truthseedtool
trackFitAlg.SaveTrackStateTool = savetrackstatetool
trackFitAlg.positionedTrackHits.Path = "positionedHits"
trackFitAlg.allGenParticles.Path = "GenParticles"

#trackFitAlg.trackHits.Path = "hits"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="trkFit_ExampleWithGeantinos.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, trackFitAlg, out],
                EvtSel = 'NONE',
                EvtMax   = 500,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice, trkvolmanservice,],
                OutputLevel=DEBUG
 )
