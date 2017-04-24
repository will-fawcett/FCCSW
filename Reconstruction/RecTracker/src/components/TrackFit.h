#ifndef RECTRACKER_TRACKFIT_H
#define RECTRACKER_TRACKFIT_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "ACTSLogger.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FitUtils.h"
class IGeoSvc;
class ITrackingGeoSvc;
class ITrkVolumeManagerSvc;
class ITrackSeedingTool;


namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}



class TrackFit : public GaudiAlgorithm {
public:
  TrackFit(const std::string& name, ISvcLocator* svcLoc);

  ~TrackFit();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<ITrackingGeoSvc> m_trkGeoSvc;
  SmartIF<ITrkVolumeManagerSvc> m_trkVolMan;
  SmartIF<IGeoSvc> m_geoSvc;

  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"TrackSeedingTool/TruthSeedingTool", this};

  std::shared_ptr<Acts::TrackingGeometry> m_trkGeo;
  std::shared_ptr<Acts::IExtrapolationEngine> m_exEngine;


  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Reader, this};
};
#endif /* RECTRACKER_TRACKFIT_H */
