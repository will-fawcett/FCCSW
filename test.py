

from Gaudi.Configuration import *
from Configurables import GeoSvc, SimG4Svc, ApplicationMgr, SimG4GdmlTestDetector

# initialize master geometry of FCC-hh
#geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectMaster.xml'])
det = SimG4GdmlTestDetector("SimG4GdmlTestDetector", gdml = "TestBox.gdml")
# giving the names of tools will initialize the tools of that type
# adding G4 command that actually runs the overlap check
geantservice = SimG4Svc("SimG4Svc",
                        detector=det,
                        physicslist="SimG4FtfpBert",
                        actions="SimG4FullSimActions",
                        g4PostInitCommands=['/geometry/test/recursion_depth 3',  '/geometry/test/run'])

ApplicationMgr(TopAlg=[],
               EvtSel='NONE',
               EvtMax=1,
               ExtSvc=[ geantservice])
