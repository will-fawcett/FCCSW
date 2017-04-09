import sys
from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)


### Generation ################################################################
from Configurables import ConstPileUp
pileuptool = ConstPileUp(numPileUpEvents=0)

from Configurables import PythiaInterface
pythiafile="Generation/data/Pythia_minbias_pp_100TeV.cmd"
pythia8gen = PythiaInterface("Pythia8Interface", 
    Filename=pythiafile, 
    OutputLevel=INFO)
pythia8gen.PileUpTool = pileuptool
pythia8gen.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

### Simulation ################################################################

from Configurables import SimG4Svc
geantservice = SimG4Svc(
  "SimG4Svc", 
  detector='SimG4DD4hepDetector', 
  physicslist="SimG4FtfpBert", 
  actions="SimG4FullSimActions"
  )

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

from Configurables import SimG4SaveTrackerHits
savetrackertool = SimG4SaveTrackerHits(
  "saveTrackerHits", 
  readoutNames = [
    "TrackerBarrelReadout", 
    "TrackerEndcapReadout"
    ]
  )
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

from Configurables import SimG4PrimariesFromEdmTool
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"

from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=-5, etaMax=5, particleName="geantino")

from Configurables import SimG4Alg
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)

### Persistency ###############################################################
from Configurables import PodioOutput
out = PodioOutput("out", OutputLevel=DEBUG)
out.outputCommands = ["keep *"]



topAlgList = [pythia8gen, hepmc_converter, geantsim, out]
svcList = [podioevent, geoservice, geantservice]

# actual jobs need to add the application mgr like this
#from Configurables import ApplicationMgr
#ApplicationMgr( TopAlg = TopAlgList,
#                EvtSel = 'NONE',
#                EvtMax   = 1,
#                ExtSvc = svcList
#                OutputLevel=DEBUG
# )
