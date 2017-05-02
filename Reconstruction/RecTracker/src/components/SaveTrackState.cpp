#include "SaveTrackState.h"

#include "ACTS/EventData/Measurement.hpp"



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
  m_trackStateCollection->create(phi, theta, qOp, z0, d0, referencePoint, cov);
  return StatusCode::SUCCESS;
}

StatusCode SaveTrackStateTool::newCollection() {
   m_trackStateCollection = m_trackStates.createAndPut(); 
   return StatusCode::SUCCESS;
 }

StatusCode SaveTrackStateTool::saveTrackInCollection(const Acts::TrackParametersBase& theActsResult) {
  
  auto p = theActsResult.getParameterSet().getParameters();
  auto cov = theActsResult.covariance();
  std::array<float, 15> edmCov;
  edmCov[0] = (*cov)(0,0);
  edmCov[1] = (*cov)(1,1);
  edmCov[2] = (*cov)(2,2);
  edmCov[3] = (*cov)(3,3);
  edmCov[4] = (*cov)(4,4);
  m_trackStateCollection->create(p[2], p[3], p[4], p[0], p[1], fcc::Point(), edmCov);
  return StatusCode::SUCCESS;
}

  


