import sys
from Gaudi.Configuration import *

from Configurables import GeoSvc

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")

geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
from Configurables import MaterialScan
material_filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")
materialservice = MaterialScan("GeoDump", filename=material_filename, etaBinning=0.06, etaMax=6, nPhiTrials=1)

from Configurables import PodioOutput
## PODIO algorithm
out = PodioOutput("out", OutputLevel=DEBUG)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, materialservice],
                OutputLevel=DEBUG
 )
