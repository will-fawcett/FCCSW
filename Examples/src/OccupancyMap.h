#ifndef EXAMPLES_OCCUPANCYMAP_H
#define EXAMPLES_OCCUPANCYMAP_H

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



class OccupancyMap : public GaudiAlgorithm {
public:
  OccupancyMap(const std::string& name, ISvcLocator* svcLoc);

  ~OccupancyMap();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();
  void visitDetElement(const DD4hep::Geometry::DetElement& det);

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;



  float m_segGridSizeZ;
  float m_segGridSizeX;
  DD4hep::DDSegmentation::BitField64* m_decoderBarrel;


  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Writer, this};
};
#endif /* EXAMPLES_OCCUPANCYMAP_H */
