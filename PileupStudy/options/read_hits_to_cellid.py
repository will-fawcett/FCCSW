from Gaudi.Configuration import *

from geant_fullsim_baseconfig import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput, CellidTestAlgo
podioinput = PodioInput("PodioReader", filename="PileupStudy/data/geantino_scan.root", collections=["hits"], OutputLevel=DEBUG)
checker = CellidTestAlgo()

out = PodioOutput("out", filename="out2.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput, checker
              ],
    EvtSel = 'NONE',
    EvtMax   = 3,
    ExtSvc = [podioevent, geoservice],
 )
