
from singleParticleBaseConfig import *
import sys

pgun.ptMin = float(sys.argv[1].split('ptMin')[1].split('_')[0])
pgun.ptMax = float(sys.argv[1].split('ptMin')[1].split('_')[0])
geantsim.eventProvider = pgun

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

topAlgList = [geantsim, trackFitAlg, out]
# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = topAlgList,
                EvtSel = 'NONE',
                EvtMax   = 500,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = svcList,
                OutputLevel=DEBUG
 )

