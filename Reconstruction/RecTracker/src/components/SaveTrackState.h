#ifndef RECTRACKER_SAVETRACKSTATE_H
#define RECTRACKER_SAVETRACKSTATE_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ISaveTrackStateTool.h"
class IGeoSvc;

// datamodel
namespace fcc {
class TrackStateCollection;
class Point;
}


class SaveTrackStateTool : public GaudiTool, virtual public ISaveTrackStateTool {
public:
  explicit SaveTrackStateTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SaveTrackStateTool();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   @return status code
   */
  virtual StatusCode saveOutput(float d0, float z0, float phi, float theta, float qOp, fcc::Point referencePoint, std::array<float, 15> cov) final;

private:
  /// Handle for track states
  DataHandle<fcc::TrackStateCollection> m_trackStates{"tracks/trackStates", Gaudi::DataHandle::Writer, this};
};

#endif
