#include "SaveTrackState.h"



// datamodel
#include "datamodel/TrackStateCollection.h"
#include "datamodel/Point.h"


DECLARE_TOOL_FACTORY(SaveTrackStateTool)

SaveTrackStateTool::SaveTrackStateTool(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISaveTrackStateTool>(this);
  declareProperty("trackStates", m_trackStates, "Handle for tracker states");
}

SaveTrackStateTool::~SaveTrackStateTool() {}

StatusCode SaveTrackStateTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SaveTrackStateTool::finalize() { return GaudiTool::finalize(); }

 StatusCode SaveTrackStateTool::saveOutput(float d0, float z0, float phi, float theta, float qOp, fcc::Point referencePoint, std::array<float, 15> cov) {
   debug() << "Saving Track State ..." << endmsg;
  auto edmTrackStates = m_trackStates.createAndPut();
 auto edmTrackState = edmTrackStates->create(d0, z0, phi, theta, qOp, referencePoint, cov);
 edmTrackState.phi(16.);
  return StatusCode::SUCCESS;
}
