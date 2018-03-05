#include "TripletSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "LineParameters.h"
#include "myTrack.h"
#include "TrackFitter.h"

// TrickTrack headers
//#include "tricktrack/TTPoint.h"
//using Hit = tricktrack::TTPoint; 
//using namespace tricktrack;



DECLARE_TOOL_FACTORY(TripletSeedingTool)

// Constructor 
TripletSeedingTool::TripletSeedingTool(const std::string& type, const std::string& name, const IInterface* parent) : GaudiTool(type, name, parent) {
  declareInterface<ITrackSeedingTool>(this);
  declareTool(m_hitFilterTool, "FastHitFilterTool/FastHitFilterTool");
}

//------------------------------------------------------------------------------

// Initialize 
StatusCode TripletSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}


//------------------------------------------------------------------------------

inline int countFakes(std::vector<myTrack>& theTracks){
    // Utility function to count the number of fake tracks
    int nFakes(0);
    for(const auto& track : theTracks){
      if(track.isFake()) nFakes++;
    }
    return nFakes;
}


//------------------------------------------------------------------------------

void TripletSeedingTool::createBarrelSpacePoints(std::vector<myHit>& thePoints,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex,
                                                       int layerCounter, 
                                                       int) {
  /**************
   * Filters through the hits in theHits, adds hit objects (myHit) to the vector thePoints
   * Args:
   *    std::vector<Hit>& thePoints : the vector to store the filtered hit objects
   *    const fcc::PositionedTrackHitCollection* theHits : the input hits 
   *    std::pair<int, int> sIndex : pair with layer index in the second element (first triplet layer has index 0. second triplet layer has index 1)  
   *    int layerCounter: an integer labelling the layer the hit is in
   *    int : unused debug parameter
   *************/

  size_t hitCounter = 0;
  std::set<int> trackIdsInThisLayer;
  for (auto hit : *theHits) {
    // not sure what the hitfiltertool does ... maybe to remove these duplicate hits? see RecTracker/src/components/FastHitFilterTool.cpp  
    if (m_hitFilterTool->filter(hit.core())) { 
        auto result = trackIdsInThisLayer.insert(hit.core().bits); // insert returns std::pair (iterator to inserted element, bool set to true if insertion took place)
        if (result.second) {

        //std::cout << "layer " << sIndex.second << "\thit id: " << hit.core().bits << std::endl;
        //std::cout << "inputs: (x, y, z) : (" << hit.position().x << ", " << hit.position().y << ", " << hit.position().z << ")" << std::endl;

        /****************
        // emplace_back, kindov like push_back but seems to be able to create the object. Avoids the extra copy operation used by push_back
        thePoints.emplace_back(  
          sqrt(hit.position().x*hit.position().x + hit.position().y*hit.position().y), 
          atan2f( hit.position().y, hit.position().x ), 
          hit.position().z,
          hit.core().time, 
          hitCounter);
        *************/

        thePoints.emplace_back(
            hit.position().x,
            hit.position().y,
            hit.position().z,
            hit.core().time,
            layerCounter, // layer ID
            hit.core().bits, // particle ID
            hitCounter); // something for a track ID 
        
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
  std::vector<std::vector<myHit>> layerPoints;
  for (unsigned int layerCounter = 0; layerCounter < 3; ++layerCounter) {

    layerPoints.emplace_back();

    // set the indices the hit filter uses to select hits
    m_hitFilterTool->setIds(m_seedingLayerIndices[layerCounter].first, m_seedingLayerIndices[layerCounter].second);

    // convert "theHits" to TTPoint hit class, store these in layerPoints.back(), somehow also only extracts hits in a specific layer (from setIds from hitFilterTool)  
    createBarrelSpacePoints(layerPoints.back(), theHits, m_seedingLayerIndices[layerCounter], layerCounter, 0 /* debug parameter, currently unused */); 

    //debug() << "found " << layerPoints.back().size() << " points on Layer " << endmsg;

  }

  // output, associating trackIds with hitIndices
  std::multimap<unsigned int, unsigned int> theSeeds;

  // Define parameters for TrackFitter 
  float vertexDistSigma = 53.0; // standard deviation (1 sigma) of the distribution of vertices along z
  int nVertexSigma = 3;
  // based on the spread of vertices, calculate the maximum tolerance along the beam line to which a track can point
  float maxZ = nVertexSigma*vertexDistSigma;
  float minZ = -1*maxZ;
  std::vector<float> parameters;
  parameters.push_back(minZ);
  parameters.push_back(maxZ);

  // Layer ID
  std::vector<int> layerIDs = {0, 1, 2};
  TrackFitter tf(fitTypes::simpleLinear, parameters, layerIDs);

  // Associate hits 
  if(tf.AssociateHits(layerPoints)){

    // Apply curvature cut 
    tf.ApplyCurvatureCut( 0.005 );

    // Get the tracks ...  
    std::vector< myTrack > theTracks = tf.GetTracks();
    int nFakes = countFakes(theTracks);

    int trackID(0);
    for(const myTrack& track : theTracks ) {
      for(const myHit* hit : track.getAssociatedHits()){
        theSeeds.insert(std::pair<unsigned int, unsigned int>(trackID,hit->identifier()));
      } // end loop over hits associated to the track 
      trackID++;
    } // end loop over tracks 

  } // end AssociateHits 


  // it's possible to use the other fit infrastructure as well, by inserting in this map the pair (trackId, indexInPositionedTrackHitCollection) 
  // the first three hits in collection  belong to track 0
  return theSeeds;
}

StatusCode TripletSeedingTool::finalize() { return GaudiTool::finalize(); }
