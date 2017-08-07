#ifndef RECTRACKER_FITTEST_H
#define RECTRACKER_FITTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ITrackingGeoSvc;
class ITrkGeoDumpSvc;
class ITrackSeedingTool;


namespace Acts {
class TrackingGeometry;
class IExtrapolationEngine;
}

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
class TrackStateCollection;
class ParticleCollection;
}

class Fittest : public GaudiAlgorithm {
public:
  Fittest(const std::string& name, ISvcLocator* svcLoc);

  ~Fittest();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<ITrackingGeoSvc> m_trkGeoSvc;
  SmartIF<IGeoSvc> m_geoSvc;


  std::shared_ptr<const Acts::TrackingGeometry> m_trkGeo;
  std::shared_ptr<const Acts::IExtrapolationEngine> m_exEngine;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::TrackStateCollection> m_trackStates{"trackStates", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::ParticleCollection> m_recoParticles{"RecoParticles", Gaudi::DataHandle::Writer, this};
  std::default_random_engine m_e;
};

using namespace Acts;

typedef FittableMeasurement<long int> FitMeas_t;
template <ParID_t... pars>
using Meas_t = Measurement<long int, pars...>;


#endif /* RECTRACKER_EXTRAPOLATIONTEST_H */
