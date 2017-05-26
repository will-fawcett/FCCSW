#include "SaveTrackStateTest.h"

#include "datamodel/TrackStateCollection.h"




DECLARE_ALGORITHM_FACTORY(SaveTrackStateTest)

SaveTrackStateTest::SaveTrackStateTest(const std::string& name, ISvcLocator* pSvcLocator) : GaudiAlgorithm(name, pSvcLocator) {

  declareProperty("SaveTrackStateTool", m_saveTrackStateTool, "Tool that saves the track state");
}

SaveTrackStateTest::~SaveTrackStateTest() {}

StatusCode SaveTrackStateTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); //Initialize base class
  if (sc.isFailure()) return sc;
  return StatusCode::SUCCESS;
}

StatusCode SaveTrackStateTest::execute() {
  std::cout << "execute " << std::endl;
  debug() << "execute ..." << endmsg;
  std::array<float, 15> cov = {0, 1, 2, 3, 4, 5, 6, 7, 8,  9, 10, 11, 12, 13, 14};
  m_saveTrackStateTool->saveOutput(0., 1., 2., 3., 4., fcc::Point(), cov);
  return StatusCode::SUCCESS;

}

StatusCode SaveTrackStateTest::finalize() {return GaudiAlgorithm::finalize();}

