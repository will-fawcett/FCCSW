
from geant_fullsim_baseconfig import *

geantservice.physicslist="SimG4GeantinoDeposits"

geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=pgun)

out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

TopAlg = [geantsim, out]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = TopAlg,
                EvtSel = 'NONE',
                EvtMax   = 1000,
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
