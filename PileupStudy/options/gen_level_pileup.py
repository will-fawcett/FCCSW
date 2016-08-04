
from geant_fullsim_baseconfig import *


from Configurables import ConstPileUp
pileuptool = ConstPileUp(numPileUpEvents=1040, Filename=optionspath + "Pythia_minbias_pp_100TeV.cmd")

from Configurables import PythiaInterface
pythiafile=optionspath + "Pythia_ttbar.cmd"
pythia8gen = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen.PileUpTool = pileuptool
pythia8gen.DataOutputs.hepmc.Path = "hepmc"

out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

TopAlg = [pythia8gen, hepmc_converter, geantsim, out]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = TopAlg,
                EvtSel = 'NONE',
                EvtMax   = 1,
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
