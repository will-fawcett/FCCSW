
from geantinoBaseConfig_trackerv3_03 import *
import sys

pgun.phiMin = 0
pgun.phiMax = 3.1415
pgun.etaMin = 0.01
pgun.etaMax = 0.01
pgun.energyMin = 0.1
pgun.energyMax = 100000
geantsim.eventProvider = pgun

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 5000 ,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
