
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "TrackFitTest.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecTracker/TrackingUtils.h"

#include "tricktrack/RiemannFit.h"


DECLARE_ALGORITHM_FACTORY(TrackFitTest)

TrackFitTest::TrackFitTest(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
}

StatusCode TrackFitTest::initialize() {
  info() << "initialize" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode TrackFitTest::execute() {

  // get hits from event store
  const fcc::PositionedTrackHitCollection* fcchits = m_positionedTrackHits.get();
  // filter primary particle and create hit matrix
  //
  //
  constexpr unsigned int nhits = 4;


  // do the actual fit
  //
  tricktrack::Matrix3xNd riemannHits = tricktrack::Matrix3xNd::Zero(3,nhits);
  /*for (unsigned int i = 0; i < nhits; ++i) {
    riemannHits.col(i) << i, i, i;
  }
  */
  riemannHits.col(0) << 1, 0, 0;
  riemannHits.col(1) << 0, 1, 1;
  riemannHits.col(2) << -1, 0, 2;
  riemannHits.col(3) <<  0, -1, 3;



  
  std::cout << riemannHits << std::endl;
  std::cout << "Fast fit: " << std::endl;
  std::cout << tricktrack::Fast_fit(riemannHits) << std::endl; 
          
  tricktrack::Matrix3Nd hits_cov = tricktrack::Matrix3Nd::Random(3*nhits,3*nhits);
  auto h = tricktrack::Helix_fit(riemannHits, hits_cov, 1);
  std::cout << "charge " << h.q << std::endl;
  std::cout << "chi_2 circle " << h.chi2_circle << std::endl;
  std::cout << "chi_2 line " << h.chi2_line << std::endl;
  std::cout << "par " << h.par << std::endl; 
  //


  return StatusCode::SUCCESS;
}

StatusCode TrackFitTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
