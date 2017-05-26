import sys
from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from FCCPileupScenarios import FCCPhase1PileupConf as pileupconf
from Configurables import GaussSmearVertex
smeartool = GaussSmearVertex(
     xVertexSigma=pileupconf['xVertexSigma'],
     xVertexMean=pileupconf['xVertexMean'],
     yVertexSigma=pileupconf['yVertexSigma'],
     yVertexMean=pileupconf['yVertexMean'],
     zVertexSigma=pileupconf['zVertexSigma'],
     zVertexMean=pileupconf['zVertexMean'],
     tVertexSigma=pileupconf['tVertexSigma'],
     tVertexMean=pileupconf['tVertexMean'])

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc(
  "GeoSvc", 
  detectors=[
    'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
    'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
    ],
  OutputLevel=INFO,
)


### Generation ################################################################
from Configurables import ConstPileUp
pileuptool = ConstPileUp(numPileUpEvents=0)


# option 1: pythia
pythiafile="Generation/data/Pythia_standard.cmd"
pythiafile_pileup="Generation/data/Pythia_minbias_pp_100TeV.cmd"
from Configurables import PythiaInterface, GenAlg
### PYTHIA algorithm
pythia8gentool = PythiaInterface("Pythia8Interface", Filename=pythiafile)
pythia8gen = GenAlg("Pythia8", SignalProvider=pythia8gentool, VertexSmearingTool=smeartool)
pythia8gen.PileUpTool = pileuptool
pythia8gen.hepmc.Path = "hepmc"


# option 2: particle gun
from Configurables import GenAlg, MomentumRangeParticleGunPt
guntool = MomentumRangeParticleGunPt("ParticleGunTool", PdgCodes=[11, -11, 13, -13, 22, 211, -211,-321, 321, 2212, -2212, 2112,])
gunalg = GenAlg("ParticleGun", SignalProvider=guntool, VertexSmearingTool="FlatSmearVertex")
gunalg.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
## Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")


from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
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

#from Configurables import SimG4SingleParticleGeneratorTool
#pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", etaMin=-5, etaMax=5, particleName="geantino")

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
