
from geant_fullsim_baseconfig import *

from Configurables import PythiaInterface
pythiafile=optionspath + "Pythia_minbias_pp_100TeV.cmd"
pythia8gen = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen.DataOutputs.hepmc.Path = "hepmc"
out.Filename = datapath +  __file__.replace(".py", ".root")

TopAlg = [pythia8gen, hepmc_converter, geantsim, out]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = TopAlg,
                EvtSel = 'NONE',
                EvtMax   = 1040,
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
