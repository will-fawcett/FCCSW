#include "TripletSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"



// TrickTrack headers
#include "tricktrack/TTPoint.h"



using Hit = tricktrack::TTPoint; 

using namespace tricktrack;



DECLARE_TOOL_FACTORY(TripletSeedingTool)

TripletSeedingTool::TripletSeedingTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITrackSeedingTool>(this);
  declareTool(m_hitFilterTool, "FastHitFilterTool/FastHitFilterTool");

}

StatusCode TripletSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  return sc;
}



void TripletSeedingTool::createBarrelSpacePoints(std::vector<Hit>& thePoints,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex,
                                                       int) {
  size_t hitCounter = 0;
  std::set<int> trackIdsInThisLayer;
  for (auto hit : *theHits) {
    if (m_hitFilterTool->filter(hit.core())) {
        auto result = trackIdsInThisLayer.insert(hit.core().bits);
        if (result.second) {
        std::cout << "layer " << sIndex.second << "\thit id: " << hit.core().bits << std::endl;
        thePoints.emplace_back(
        hit.position().x,
        hit.position().y,
        hit.position().z,
        hit.core().time, 
        hitCounter);
        }

      }
        ++hitCounter;
  }
}



std::multimap<unsigned int, unsigned int>
TripletSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {
  std::vector<std::vector<Hit>> layerPoints;
  // output, associating trackIds with hitIndices
  std::multimap<unsigned int, unsigned int> theSeeds;
  for (unsigned int layerCounter = 0; layerCounter < 3; ++layerCounter) {

    layerPoints.emplace_back();
    // set the indices the hit filter uses to select hits
    m_hitFilterTool->setIds(m_seedingLayerIndices[layerCounter].first, m_seedingLayerIndices[layerCounter].second);
    createBarrelSpacePoints(layerPoints.back(), theHits, m_seedingLayerIndices[layerCounter], 0 /* debug parameter, currently unused */); // fill the vecor of hits from collection
    debug() << "found " << layerPoints.back().size() << " points on Layer " << endmsg;
    for (auto hit: layerPoints[layerCounter]) {

      debug() << "x: " << hit.x() << "\ty: " << hit.y() << "\tz: " << hit.z() << endmsg; 

      /********************
       *
       * do something with hits in this layer
       *
       */


    }
  }

    // it's possible to use the other fit infrastructure as well, by inserting in this map the pair (trackId, indexInPositionedTrackHitCollection) 
    // the first three hits in collection  belong to track 0
    theSeeds.insert(std::pair<unsigned int, unsigned int>(0,0));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(0,1));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(0,2));
  return theSeeds;
}

StatusCode TripletSeedingTool::finalize() { return GaudiTool::finalize(); }
