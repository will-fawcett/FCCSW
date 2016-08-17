
from geant_fullsim_baseconfig import *

numPileUpEvents = int(sys.argv[1].split('NUMPILEUP')[1].split("_")[0])
print numPileUpEvents
pileuptool.numPileUpEvents=numPileUpEvents



out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

TopAlg = [pythia8gen, hepmc_converter, geantsim, out]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = TopAlg,
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
