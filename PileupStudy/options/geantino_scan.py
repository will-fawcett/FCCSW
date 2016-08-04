
from geant_fullsim_baseconfig import *

geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=pgun)


out.Filename = datapath + __file__.replace(".py", ".root")
TopAlg = [geantsim, out]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = TopAlg,
                EvtSel = 'NONE',
                EvtMax   = 1000,
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
