#ifndef RECTRACKER_TRACKFIT_H
#define RECTRACKER_TRACKFIT_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecTracker/ACTSLogger.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FitUtils.h"
class IGeoSvc;
class ITrackingGeoSvc;
class ITrkVolumeManagerSvc;
class ITrackSeedingTool;
class ISaveTrackStateTool;


namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
class MCParticleCollection;
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
  ToolHandle<ISaveTrackStateTool> m_saveTrackStateTool{"SaveTrackStateTool/SaveTrackStateTool", this};

  std::shared_ptr<Acts::TrackingGeometry> m_trkGeo;
  std::shared_ptr<Acts::IExtrapolationEngine> m_exEngine;
  Acts::KalmanFitter<MyExtrapolator, CacheGenerator, NoCalibration, Acts::GainMatrixUpdator> m_KF;

  float m_segGridSizeZ;
  float m_segGridSizeX;
  DD4hep::DDSegmentation::BitField64* m_decoderBarrel;


  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::MCParticleCollection> m_genParticles{"allGenParticles", Gaudi::DataHandle::Reader, this};
};
#endif /* RECTRACKER_TRACKFIT_H */
