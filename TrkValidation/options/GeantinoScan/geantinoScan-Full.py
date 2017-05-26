
from geantinoBaseConfig import *
import sys
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

pgun.phiMin = 0
pgun.phiMax = constants.pi
pgun.etaMin = -6
pgun.etaMax = 6
pgun.energyMin=1*units.GeV
pgun.energyMax=1000*units.GeV
geantsim.eventProvider = pgun

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = 10000 ,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=DEBUG
 )
