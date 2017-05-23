from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc")
from Configurables import PodioInput


from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  "file:/afs/cern.ch/work/v/vavolkl/FCCSW/TrkValidation/compact/Tracker_Small.xml",
  #'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  ],
  )


from Configurables import OccupancyMap
occAlg = OccupancyMap()

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="occupancyMap.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [occAlg, out],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice],
                OutputLevel=DEBUG
 )
