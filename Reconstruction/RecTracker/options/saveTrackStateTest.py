from Gaudi.Configuration import *

from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

from Configurables import SaveTrackStateTool
saveTool = SaveTrackStateTool("SaveTrackStateTool")
saveTool.trackStates.Path = "trackStates"

from Configurables import SaveTrackStateTest
saveTest = SaveTrackStateTest()
saveTest.SaveTrackStateTool = saveTool


from Configurables import PodioOutput
out = PodioOutput("out", filename="testSaveTrackStates.root")
out.outputCommands = ["keep *"]

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=[saveTest, out],
                EvtSel="NONE",
                EvtMax=100,
                ExtSvc=[podioevent],
                OutputLevel=DEBUG,
                )

