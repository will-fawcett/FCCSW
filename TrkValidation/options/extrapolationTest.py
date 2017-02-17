import os
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")


from Configurables import TrkVolumeManagerSvc
trkvolmanservice = TrkVolumeManagerSvc("TrkVolMan")

from Configurables import TruthSeedingTool

truthseedtool = TruthSeedingTool()


from Configurables import TrackFit
trackFitAlg = TrackFit()
trackFitAlg.trackSeedingTool = truthseedtool
trackFitAlg.DataInputs.positionedTrackHits.Path = "positionedHits"
#trackFitAlg.DataInputs.trackHits.Path = "hits"

from Configurables import ExtrapolationTest
exTest = ExtrapolationTest()
exTest.DataOutputs.positionedTrackHits.Path="positionedHits"
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [exTest, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice, trkvolmanservice],
                OutputLevel=DEBUG
 )
