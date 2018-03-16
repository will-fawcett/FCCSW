#ifndef RECTRACKER_TRICKTRACKSEEDINGTOOL_H
#define RECTRACKER_TRICKTRACKSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/IDoubletCreationTool.h"
#include "RecInterface/IHitFilterTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/MCParticleCollection.h" // WJF add 

#include "tricktrack/SpacePoint.h"
#include "tricktrack/CMGraph.h"
#include "tricktrack/FKDTree.h"
/*#include "tricktrack/TTPoint.h"*/
#include "tricktrack/TrackingRegion.h"
#include "tricktrack/HitChainMaker.h"

#include <map>


#include "myTrack.h"
#include <utility>

#include "myHit.h"

class TripletSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TripletSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TripletSeedingTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) override final;
  virtual std::multimap<unsigned int, unsigned int> findSeedsWithParticles(const fcc::PositionedTrackHitCollection* theHits, const fcc::MCParticleCollection*         theParticles, std::vector<myTrack>& theTracks) override final; 

  void createBarrelSpacePoints(std::vector<myHit>& thePoints, 
      const fcc::PositionedTrackHitCollection* theHits, 
      const fcc::MCParticleCollection* theParticles, 
      std::pair<int, int> sIndex, 
      int layerCounter, 
      int trackCutoff);

  tricktrack::HitDoublets<Hit>* findDoublets( std::vector<myHit> theInnerHits,  std::vector<myHit> theOuterHits);

  //void createKDTree( std::vector<Hit>& thePoints, std::pair<int, int> sIndex);
  
  void findDoublets(tricktrack::HitDoublets<Hit>* doublets, std::vector<myHit> theInnerHits,  tricktrack::FKDTree<double, 4> theOuterTree, std::vector<myHit> theOuterHits);

private:
  /// system and layer ids for the inner barrel layer to be used for seeding
  ToolHandle<IHitFilterTool> m_hitFilterTool;
  Gaudi::Property<std::vector<std::pair<int, int>>> m_seedingLayerIndices{this, "seedingLayerIndices", {{1, 0}, {1,1}, {1,2}}};
  Gaudi::Property<double> m_Parameter {this, "someParameter", 0};

};

#endif /* RECTRACKER_TRICKTRACKSEEDINGTOOL_H */
