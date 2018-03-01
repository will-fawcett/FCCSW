#include "TripletSeedingTool.h"

// add myTrack
//#include "myTrack.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

// TrickTrack headers
#include "tricktrack/TTPoint.h"
using Hit = tricktrack::TTPoint; 

using namespace tricktrack;


DECLARE_TOOL_FACTORY(TripletSeedingTool)

// Constructor 
TripletSeedingTool::TripletSeedingTool(const std::string& type, const std::string& name, const IInterface* parent) : GaudiTool(type, name, parent) {
  declareInterface<ITrackSeedingTool>(this);
  declareTool(m_hitFilterTool, "FastHitFilterTool/FastHitFilterTool");
}

// Initialize 
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
  /**************
   * Filters through the hits in theHits, adds hit objects (tricktrack::TTPoint) to the vector thePoints
   * Args:
   *    std::vector<Hit>& thePoints : the vector to store the filtered hit objects
   *    const fcc::PositionedTrackHitCollection* theHits : the input hits 
   *    std::pair<int, int> sIndex : pair with layer index in the second element (first triplet layer has index 0. second triplet layer has index 1)  
   *    int : unused debug parameter
   *************/

  size_t hitCounter = 0;
  std::set<int> trackIdsInThisLayer;
  for (auto hit : *theHits) {
    if (m_hitFilterTool->filter(hit.core())) { // not sure what teh hitfiltertool does ... maybe to remove these duplicate hits? see RecTracker/src/components/FastHitFilterTool.cpp  
        auto result = trackIdsInThisLayer.insert(hit.core().bits); // insert returns std::pair (iterator to inserted element, bool set to true if insertion took place)
        if (result.second) {

        //std::cout << "layer " << sIndex.second << "\thit id: " << hit.core().bits << std::endl;
        //std::cout << "inputs: (x, y, z) : (" << hit.position().x << ", " << hit.position().y << ", " << hit.position().z << ")" << std::endl;

        // WJF: TTPoint constructed with (r, phi, z, t, ID) ? 
        thePoints.emplace_back( // emplace_back, kindov like push_back but seems to be able to create the object. Avoids the extra copy operation used by push_back
            //hit.position().x,
            sqrt(hit.position().x*hit.position().x + hit.position().y*hit.position().y), // r
            //hit.position().y,
            atan2f( hit.position().y, hit.position().x ), // phi ?  
        hit.position().z,
        hit.core().time, 
        hitCounter);
        
        }

      }
        ++hitCounter;
  }
}



std::multimap<unsigned int, unsigned int> TripletSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {

  /**************
   * Function to ...
   * ***************/

  static int wjfCounter(0);
  wjfCounter++;
  std::cout << "WJF: findSeeds() called: " << wjfCounter << std::endl;

  // fill layerPoints with the hits in each of the triplet layers 
  std::vector<std::vector<Hit>> layerPoints;
  for (unsigned int layerCounter = 0; layerCounter < 3; ++layerCounter) {

    layerPoints.emplace_back();

    // set the indices the hit filter uses to select hits
    m_hitFilterTool->setIds(m_seedingLayerIndices[layerCounter].first, m_seedingLayerIndices[layerCounter].second);

    // convert "theHits" to TTPoint hit class, store these in layerPoints.back(), somehow also only extracts hits in a specific layer (from setIds from hitFilterTool)  
    createBarrelSpacePoints(layerPoints.back(), theHits, m_seedingLayerIndices[layerCounter], 0 /* debug parameter, currently unused */); 

  //}

    debug() << "found " << layerPoints.back().size() << " points on Layer " << endmsg;
    
    // loop over all hits in this layer 
    for (Hit hit: layerPoints[layerCounter]) {

      //float hitR = sqrt(hit.x()*hit.x() + hit.y()*hit.y());// hypotf(hit.x(), hit.y());
      //debug() << "x: " << hit.x() << "\ty: " << hit.y() << "\tz: " << hit.z() << "\tr: " << hitR << endmsg; 


      /********************
       *
       * do something with hits in this layer
       *
       ******************/

    }
  }

  // output, associating trackIds with hitIndices
  std::multimap<unsigned int, unsigned int> theSeeds;

  // it's possible to use the other fit infrastructure as well, by inserting in this map the pair (trackId, indexInPositionedTrackHitCollection) 
  // the first three hits in collection  belong to track 0
  theSeeds.insert(std::pair<unsigned int, unsigned int>(0,0));
  theSeeds.insert(std::pair<unsigned int, unsigned int>(0,1));
  theSeeds.insert(std::pair<unsigned int, unsigned int>(0,2));
  return theSeeds;
}

StatusCode TripletSeedingTool::finalize() { return GaudiTool::finalize(); }
