
from geantFullsimBaseConfig import *
import sys

pileuptool.numPileUpEvents = 0#float(sys.argv[1].split('NUMPILEUP')[1].split('_')[0])
pythia8gen.PileUpTool = pileuptool

# take the name of this file, replace extension and place in data directory
out.filename = sys.argv[1].replace("options/", "data/").replace(".py", ".root")

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = topAlgList,
                EvtSel = 'NONE',
                EvtMax   = 5000,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = svcList,
                OutputLevel=INFO
 )

