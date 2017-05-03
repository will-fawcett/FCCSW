#ifndef RECINTERFACE_SAVETRACKSTATETOOL_H
#define RECINTERFACE_SAVETRACKSTATETOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <array>

namespace fcc {
class Point;
}

namespace Acts {
  class TrackParametersBase;
}


class ISaveTrackStateTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISaveTrackStateTool, 1, 0);

  /**  Save the data output.
   *   @return status code
   */
  virtual StatusCode saveOutput(float d0, float z0, float phi, float theta, float qOp, fcc::Point referencePoint, std::array<float, 15> cov) = 0;

  virtual StatusCode saveTrackInCollection(const Acts::TrackParametersBase& theActsResult) = 0;

  virtual StatusCode newCollection() = 0;

};
#endif /* RECINTERFACE_SAVETRACKSTATETOOL_H */
