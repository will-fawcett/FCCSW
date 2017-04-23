from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")


from Configurables import ParticleGunAlg, MomentumRangeParticleGun
pgun_tool = MomentumRangeParticleGun(PdgCodes=[13], ThetaMin=constants.pi / 2., ThetaMax=constants.pi / 2., MomentumMin=10000, MomentumMax=100000)
gen = ParticleGunAlg("ParticleGun", ParticleGunTool=pgun_tool, VertexSmearingToolPGun="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc",
  detectors=[
    'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
    'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
    ],
  OutputLevel = DEBUG)

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHits("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/saveTrackerHits"],
                    eventProvider=particle_converter)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="output_trk_singlemuons.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 20,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [ppservice, podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
