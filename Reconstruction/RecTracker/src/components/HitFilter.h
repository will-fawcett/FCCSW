#ifndef RECTRACKER_HITFILTER_H
#define RECTRACKER_HITFILTER_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;


namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}



class HitFilter : public GaudiAlgorithm {
public:
  HitFilter(const std::string& name, ISvcLocator* svcLoc);

  ~HitFilter();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;



  float m_segGridSizeZ;
  float m_segGridSizeX;
  DD4hep::DDSegmentation::BitField64* m_decoderBarrel;


  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_filteredTrackHits{"filteredHits", Gaudi::DataHandle::Writer, this};
};
#endif /* RECTRACKER_HITFILTER_H */
