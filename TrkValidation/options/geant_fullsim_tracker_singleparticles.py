from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")


from Configurables import ParticleGunAlg, MomentumRangeParticleGun
## Particle Gun using MomentumRangeParticleGun tool and FlatSmearVertex
# MomentumRangeParticleGun generates particles of given type(s) within given momentum, phi and theta range
# FlatSmearVertex smears the vertex with uniform distribution

pgun_tool = MomentumRangeParticleGun(PdgCodes=[-13], ThetaMin=1.5707, ThetaMax=1.5707, MomentumMin=10, MomentumMax=10)
gen = ParticleGunAlg("ParticleGun", ParticleGunTool=pgun_tool, VertexSmearingToolPGun="FlatSmearVertex")
gen.DataOutputs.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
## Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)

from Configurables import HepMCConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool, SimG4SingleParticleGeneratorTool
# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.DataOutputs.positionedTrackHits.Path = "positionedHits"
savetrackertool.DataOutputs.trackHits.Path = "hits"
# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.DataInputs.genParticles.Path = "allGenParticles"

pgun = SimG4SingleParticleGeneratorTool("MuonGun", phiMin=0, phiMax=0, etaMin=1, etaMax=1, energyMin=5000, energyMax=5000, particleName="mu-", saveEdm=True)
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveTrackerHits" ],
                    eventProvider=pgun)
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=pgun)

x = 1123532
geantservice.G4commands += ["/random/setSeeds "+str(x)+" 0"] #where x is the number you want

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 15,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [ppservice, podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
