
from geantFullsimBaseConfig import *
import sys

guntool.MomentumMin = float(sys.argv[1].split('ptMin')[1].split('_')[0])
guntool.MomentumMax = float(sys.argv[1].split('ptMax')[1].split('_')[0])
guntool.PhiMin = 0
guntool.PhiMax = 0
guntool.EtaMin = 0
guntool.EtaMax = 0
#geantsim.eventProvider = pgun
gunalg.SignalProvider = guntool

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

#from Configurables import TruthSeedingTool
#truthseedtool = TruthSeedingTool()
#trackFitAlg.trackSeedingTool = truthseedtool
topAlgList = [gunalg, hepmc_converter, geantsim, out]
svcList += [ppservice]
# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = topAlgList,
                EvtSel = 'NONE',
                EvtMax   = 5000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = svcList,
                OutputLevel=DEBUG
 )

