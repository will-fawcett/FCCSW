#ifndef RECTRACKER_SAVETRACKSTATETEST_H
#define RECTRACKER_SAVETRACKSTATETEST_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "RecInterface/ISaveTrackStateTool.h"

namespace fcc {
class TrackStateCollection;
}

class SaveTrackStateTest: public GaudiAlgorithm {
public:
  SaveTrackStateTest(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~SaveTrackStateTest();

  virtual StatusCode initialize();

  virtual StatusCode execute();

  virtual StatusCode finalize();

private:
  ToolHandle<ISaveTrackStateTool> m_saveTrackStateTool{"SaveTrackStateTool", this};


};
#endif
