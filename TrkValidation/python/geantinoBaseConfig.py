from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", 
                    detectors=[
                      'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                      #'file:TrkValidation/compact/Tracker.xml',
                      'Detector/DetFCChhTrackerTkLayout/compact/Tracker_ACTSCompatible.xml',
                      ],
                    OutputLevel = INFO
                    )


# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
# giving the names of tools will initialize the tools of that type
geantservice = SimG4Svc("SimG4Svc", 
                        detector='SimG4DD4hepDetector', 
                        physicslist="SimG4GeantinoDeposits", 
                        actions="SimG4FullSimActions"
                        )

# lots, lots, lots of debug information
#geantservice.G4commands += ["/tracking/verbose 1"]


from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4")

# first, create a tool that saves the tracker hits
# Name of that tool in GAUDI is "XX/YY" where XX is the tool class name ("SimG4SaveTrackerHits")
# and YY is the given name ("saveTrackerHits")
from Configurables import SimG4SaveTrackerHitsWithTrackID
savetrackertool = SimG4SaveTrackerHitsWithTrackID("saveTrackerHits",
                                       readoutNames=[ "TrackerBarrelReadout", "TrackerEndcapReadout"]
                                       )
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("GeantinoGun", 
                                        etaMin=-6, 
                                        etaMax=6, 
                                        particleName="chargedgeantino",
                                        saveEdm=True,
                                        )

from Configurables import SimG4SingleParticleGeneratorToolPt
pgunPt = SimG4SingleParticleGeneratorToolPt("GeantinoGunPt", 
                                        etaMin=-6, 
                                        etaMax=6, 
                                        particleName="chargedgeantino",
                                        saveEdm=True,
                                        )

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg
# create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHitsWithTrackID/saveTrackerHits" ],
                    eventProvider=pgunPt
                    )

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out", OutputLevel=DEBUG)
out.outputCommands = ["keep *"]

