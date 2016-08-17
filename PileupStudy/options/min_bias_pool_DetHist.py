from Gaudi.Configuration import *

from geant_fullsim_baseconfig import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")


datafilename = sys.argv[1].replace("options/", "data/").replace(".py", ".root") 

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput, OccupancyHistograms
podioinput = PodioInput("PodioReader", filename=datafilename.replace("_DetHist", ''), collections=["hits", "clusters", "hitClusterAssociation"], OutputLevel=INFO)
histos = OccupancyHistograms()
THistSvc().Output = ["rec DATAFILE='" + datafilename + "' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

out = PodioOutput("out", filename="out2.root",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

ApplicationMgr(
    TopAlg = [podioinput, histos
              ],
    EvtSel = 'NONE',
    EvtMax   = 100,
    ExtSvc = [podioevent, geoservice],
 )
