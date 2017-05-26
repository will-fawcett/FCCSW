import os
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants
from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent   = FCCDataSvc("EventDataSvc", input='hitFilter_Example.root')
from Configurables import PodioInput
#podioinput = PodioInput("PodioReader", collections=["positionedHits"], OutputLevel=DEBUG)

from Configurables import GenAlg, MomentumRangeParticleGunPt
pgun_tool = MomentumRangeParticleGunPt(PdgCodes=[13], PhiMin=0., PhiMax=0, EtaMin=0, EtaMax=6, MomentumMin=100000, MomentumMax=100000)
gen = GenAlg("ParticleGun", SignalProvider=pgun_tool)
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  #"file:TrkValidation/compact/Tracker_Small.xml",
  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker_ACTSCompatible.xml',
  ],
  )
from Configurables import TrackingGeoSvc
trkgeoservice = TrackingGeoSvc("TrackingGeometryService")

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

from Configurables import SimG4Alg, SimG4SaveTrackerHitsWithTrackID, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHitsWithTrackID("saveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHitsWithTrackID/saveTrackerHits"],
                    eventProvider=particle_converter)


podioinput = PodioInput("PodioReader", collections=["positionedHits"], OutputLevel=DEBUG)



from Configurables import HitFilter
hitFilterAlg = HitFilter()
hitFilterAlg.positionedTrackHits.Path = "positionedHits"
hitFilterAlg.filteredTrackHits.Path = "filteredHits"

#hitFilterAlg.trackHits.Path = "hits"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename="hitFilter2_Example.root"

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [podioinput, hitFilterAlg, out],
                EvtSel = 'NONE',
                EvtMax   = 500,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice, ppservice,],
                OutputLevel=DEBUG
 )
