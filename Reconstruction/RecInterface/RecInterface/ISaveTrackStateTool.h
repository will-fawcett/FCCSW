#ifndef RECINTERFACE_SAVETRACKSTATETOOL_H
#define RECINTERFACE_SAVETRACKSTATETOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <array>

namespace fcc {
class Point;
}


class ISaveTrackStateTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISaveTrackStateTool, 1, 0);

  /**  Save the data output.
   *   @return status code
   */
  virtual StatusCode saveOutput(float d0, float z0, float phi, float theta, float qOp, fcc::Point referencePoint, std::array<float, 15> cov) = 0;

};
#endif /* RECINTERFACE_SAVETRACKSTATETOOL_H */
