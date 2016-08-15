from Gaudi.Configuration import *

from geant_fullsim_baseconfig import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput, CellidTestAlgo
podioinput = PodioInput("PodioReader", filename="PileupStudy/data/geantino_scan.root", collections=["hits", "clusters", "hitClusterAssociation"], OutputLevel=DEBUG)
checker = CellidTestAlgo()

out = PodioOutput("out", filename="out2.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

from Configurables import CountModuleSvc
countsvc = CountModuleSvc()
THistSvc().Output = ["rec DATAFILE='DetHist.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

ApplicationMgr(
    TopAlg = [
              ],
    EvtSel = 'NONE',
    EvtMax   = 3,
    ExtSvc = [podioevent, geoservice, countsvc],
 )
