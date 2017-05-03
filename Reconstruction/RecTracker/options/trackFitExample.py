import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input="output_trk_singlemuons.root")
from Configurables import PodioInput
#podioinput = PodioInput("PodioReader", collections=["positionedHits"], OutputLevel=DEBUG)

from Configurables import ParticleGunAlg, MomentumRangeParticleGun
#pgun_tool = MomentumRangeParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=constants.pi*0.5, ThetaMin=constants.pi / 2., ThetaMax=constants.pi*0.9, MomentumMin=100000, MomentumMax=100000)
pgun_tool = MomentumRangeParticleGun(PdgCodes=[13], PhiMin=0., PhiMax=0., ThetaMin=constants.pi / 2., ThetaMax=constants.pi / 2., MomentumMin=50000, MomentumMax=50000)
gen = ParticleGunAlg("ParticleGun", ParticleGunTool=pgun_tool, VertexSmearingToolPGun="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
  ],
  )
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

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



from Configurables import TrkVolumeManagerSvc
trkvolmanservice = TrkVolumeManagerSvc("TrkVolMan")

from Configurables import TruthSeedingTool

truthseedtool = TruthSeedingTool()

from Configurables import SaveTrackStateTool

savetrackstatetool = SaveTrackStateTool()
savetrackstatetool.trackStates.Path = "trackStates"

from Configurables import TrackFit
trackFitAlg = TrackFit()
trackFitAlg.trackSeedingTool = truthseedtool
trackFitAlg.SaveTrackStateTool = savetrackstatetool
trackFitAlg.positionedTrackHits.Path = "positionedHits"

#trackFitAlg.trackHits.Path = "hits"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="trkFit_Example.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [gen, hepmc_converter, geantsim, trackFitAlg, out],
                EvtSel = 'NONE',
                EvtMax   = 1000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, trkgeoservice, trkvolmanservice, ppservice,],
                OutputLevel=DEBUG
 )
