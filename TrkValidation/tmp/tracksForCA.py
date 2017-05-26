
from geantFullsimBaseConfig import *
import sys

guntool.MomentumMin = 100000 #float(sys.argv[1].split('ptMin')[1].split('_')[0])
guntool.MomentumMax = 100000#float(sys.argv[1].split('ptMax')[1].split('_')[0])
guntool.PhiMin = 0
guntool.PhiMax = 2
guntool.ThetaMin = 0
guntool.ThetaMax = 1
gunalg.SignalProvider=guntool

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

#from Configurables import TruthSeedingTool
#truthseedtool = TruthSeedingTool()
#trackFitAlg.trackSeedingTool = truthseedtool
topAlgList = [gunalg, hepmc_converter, geantsim, out]
# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = topAlgList,
                EvtSel = 'NONE',
                EvtMax   = 500,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = svcList,
                OutputLevel=DEBUG
 )

