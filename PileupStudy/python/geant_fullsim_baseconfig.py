
import os

datapath = "PileupStudy/data/"
optionspath = "PileupStudy/options/"



from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import HepMCReader
reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/FCC_minbias_100TeV.dat")
reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import ConstPileUp
pileuptool = ConstPileUp(numPileUpEvents=0, Filename="PileupStudy/options/Pythia_minbias_pp_100TeV.cmd")

from Configurables import PythiaInterface
pythiafile="PileupStudy/options/Pythia_ttbar.cmd"
pythia8gen = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen.PileUpTool = pileuptool
pythia8gen.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc(
  "GeoSvc", 
  detectors=[
    'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
    'file:Detector/DetFCChhTrackerSimple/compact/Tracker.xml'
    ]
  )

from Configurables import SimG4Svc
geantservice = SimG4Svc(
  "SimG4Svc", 
  detector='SimG4DD4hepDetector', 
  physicslist="SimG4FtfpBert", 
  actions="SimG4FullSimActions"
  )

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHits(
  "saveTrackerHits", 
  readoutNames = [
    "TrackerBarrelReadout", 
    "TrackerEndcapReadout"
    ]
  )
savetrackertool.DataOutputs.trackClusters.Path = "clusters"
savetrackertool.DataOutputs.trackHits.Path = "hits"
savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"
from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=-5, etaMax=5, particleName="geantino")
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)

from Configurables import PodioOutput
out = PodioOutput("out", OutputLevel=DEBUG)
out.outputCommands = ["keep *"]



TopAlg = [reader, hepmc_converter, geantsim, out]

#from Configurables import ApplicationMgr
#ApplicationMgr( TopAlg = TopAlg,
#                EvtSel = 'NONE',
#                EvtMax   = 1,
#                ExtSvc = [podioevent, geoservice, geantservice],
#                OutputLevel=DEBUG
# )
