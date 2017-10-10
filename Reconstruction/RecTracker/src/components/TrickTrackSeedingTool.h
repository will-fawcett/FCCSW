#ifndef RECTRACKER_TRICKTRACKSEEDINGTOOL_H
#define RECTRACKER_TRICKTRACKSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "tricktrack/SpacePoint.h"
#include "tricktrack/CMGraph.h"
#include "tricktrack/TrackingRegion.h"
#include "tricktrack/HitChainMaker.h"

#include <map>

class IGeoSvc;

namespace DD4hep {
namespace DDSegmentation {
class BitField64;
}
}




class TrickTrackSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TrickTrackSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TrickTrackSeedingTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) override final;
  void createBarrelSpacePoints(std::vector<tricktrack::SpacePoint<size_t>>& thePoints,
                               const fcc::PositionedTrackHitCollection* theHits, std::pair<int, int> sIndex);

private:
  ServiceHandle<IGeoSvc> m_geoSvc;
  DD4hep::DDSegmentation::BitField64* m_decoder;
  /// system and layer ids for the inner barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices0{this, "seedingLayerIndices0", {0, 0}};
  /// system and layer ids for the middle barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices1{this, "seedingLayerIndices1", {0, 1}};
  /// system and layer ids for the outer barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices2{this, "seedingLayerIndices2", {0, 2}};
  /// readout used for the barrel seeding layers
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "TrackerBarrelReadout"};
  /// output trackStates for found seeds
  DataHandle<fcc::TrackStateCollection> m_trackSeeds{"tracks/trackSeeds", Gaudi::DataHandle::Writer, this};
  /// Parameter for TrickTrack's TrackingRegion
  /// coordinate of the center of the luminous region
  Gaudi::Property<double> m_regionOriginX {this, "regionOriginX", 0};
  /// Parameter for TrickTrack's TrackingRegion
  /// coordinate of the center of the luminous region
  Gaudi::Property<double> m_regionOriginY {this, "regionOriginY", 0};
  /// Parameter for TrickTrack's TrackingRegion
  /// radius of the luminous region
  Gaudi::Property<double> m_regionOriginRadius {this, "regionOriginRadius", 1000};
  /// Parameter for TrickTrack's TrackingRegion
  /// minimum transverse momentum for tracks coming from the luminous region
  Gaudi::Property<double> m_ptMin {this, "ptMin", 0};
  /// Parameter for TrickTrack's cell connection
  Gaudi::Property<double> m_thetaCut {this, "thetaCut", 1};
  /// Parameter for TrickTrack's cell connection
  Gaudi::Property<double> m_phiCut {this, "phiCut", 1000};
  /// Parameter for TrickTrack's cell connection
  Gaudi::Property<double> m_hardPtCut {this, "hardPtCut", 1000};

  ToolHandle<ILayerGraphTool> m_layerGraphTool;

  std::unique_ptr<tricktrack::HitChainMaker<tricktrack::SpacePoint<size_t>>> m_automaton;
  std::unique_ptr<tricktrack::TrackingRegion> m_trackingRegion;
  tricktrack::CMGraph m_layerGraph;
};

#endif /* RECTRACKER_TRICKTRACKSEEDINGTOOL_H */
