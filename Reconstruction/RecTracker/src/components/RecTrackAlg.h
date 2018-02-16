#ifndef RECTRACKER_COMBINATORIALSEEDINGTEST_H
#define RECTRACKER_COMBINATORIALSEEDINGTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;
class ITrackSeedingTool;

namespace fcc {
class TrackHitCollection;
class TrackCollection;
class TrackStateCollection;
class PositionedTrackHitCollection;
}


/*** @class RecTrackAlg 
 *
 */
class RecTrackAlg : public GaudiAlgorithm {
public:
  RecTrackAlg(const std::string& name, ISvcLocator* svcLoc);

  ~RecTrackAlg() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// TrackHits as Input to the track seeding
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedTrackHits", Gaudi::DataHandle::Reader,
                                                                      this};
  DataHandle<fcc::TrackCollection> m_tracks{"tracks", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::TrackStateCollection> m_trackStates{"trackStates", Gaudi::DataHandle::Writer, this};
  /// Handle to Track Seeding Tool that does the work
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"CombinatorialSeedingTool/CombinatorialSeedingTool", this};
};

#endif /* RECTRACKER_COMBINATORIALSEEDINGTEST_H */
