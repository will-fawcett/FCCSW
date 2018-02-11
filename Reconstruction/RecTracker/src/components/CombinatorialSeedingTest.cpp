
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/TrackCollection.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "CombinatorialSeedingTest.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecTracker/TrackingUtils.h"

#include "tricktrack/RiemannFit.h"

DECLARE_ALGORITHM_FACTORY(CombinatorialSeedingTest)

CombinatorialSeedingTest::CombinatorialSeedingTest(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("Tracks", m_tracks, "Tracks (Output)");
  declareProperty("TrackStates", m_trackStates, "TrackStates (Output)");
  declareProperty("TrackSeedingTool", m_trackSeedingTool);
}

StatusCode CombinatorialSeedingTest::initialize() {
  info() << "initialize" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CombinatorialSeedingTest::execute() {

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
  fcc::TrackCollection* tracks = m_tracks.createAndPut();
  fcc::TrackStateCollection* trackStates = m_trackStates.createAndPut();
  auto seedmap = m_trackSeedingTool->findSeeds(hits);

  for (auto seedIdPair: seedmap) {
    debug() << " found trackseed: " << seedIdPair.first << "\t" << seedIdPair.second << endmsg;
    auto track = tracks->create();
    auto trackState = trackStates->create();
    trackState.phi(1.4);
    track.addhits((*hits)[seedIdPair.second]);
    track.addstates(trackState);
  }

  return StatusCode::SUCCESS;
}

StatusCode CombinatorialSeedingTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
