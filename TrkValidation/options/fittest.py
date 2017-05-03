import os
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:TrkValidation/compact/Tracker_Small.xml'])
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc",
                        detector='SimG4DD4hepDetector',
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions")


#from Configurables import TrkVolumeManagerSvc
#trkvolmanservice = TrkVolumeManagerSvc("TrkVolMan")

#from Configurables import TruthSeedingTool
#truthseedtool = TruthSeedingTool()

#from Configurables import TrackFit
#trackFitAlg = TrackFit()
#trackFitAlg.trackSeedingTool = truthseedtool
#trackFitAlg.positionedTrackHits.Path = "positionedHits"
#trackFitAlg.trackHits.Path = "hits"

from Configurables import Fittest
exTest = Fittest()
exTest.positionedTrackHits.Path="positionedHits"
exTest.trackStates.Path="trackStates"
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [exTest, out],
                EvtSel = 'NONE',
                EvtMax   = 10,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice, ],
                OutputLevel=DEBUG,
 )