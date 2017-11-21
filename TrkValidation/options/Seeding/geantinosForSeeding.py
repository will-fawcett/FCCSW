
from geantinoBaseConfig import *
import sys

#pileuptool.numPileUpEvents = 0#float(sys.argv[1].split('NUMPILEUP')[1].split('_')[0])
#pythia8gen.PileUpTool = pileuptool

geantservice.g4PostInitCommands  += ["/tracking/storeTrajectory 1"]

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")
pgun.etaMin = -1.5
pgun.etaMax = 1.5
pgun.energyMin = 100
pgun.energyMax = 5000 

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = topAlgList,
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = svcList,
                OutputLevel=INFO
 )

