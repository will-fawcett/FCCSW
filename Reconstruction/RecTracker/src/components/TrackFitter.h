#ifndef TrackFitter_h
#define TrackFitter_h

#include "LineParameters.h"
#include "myTrack.h"
#include "Location.h"
#include "myHit.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>

using hitContainer = std::map<int, std::vector<myHit*> >; 

// enum for different fit types (can add more later) 
enum fitTypes{
  simpleLinear,
  MAX
};


// class to 
class TrackFitter{

  private:

    bool m_debug;

    // private member variables
    std::vector< myTrack > m_tracks;
    fitTypes fitType;
    std::vector<float> m_parameters;
    /*std::vector<HitCollection> m_associatedHitCollection;*/
    std::vector<int> m_layerIDs;

    // algorithms to associate hits 
    bool associateHitsSimple(hitContainer&, float, float);

    std::map<std::string, std::vector<myHit*>>  associateHitsSimplePattern(hitContainer&, Location&) const;

    // functions to calculate search windows
    float calculateZWindowForNextLevel(float, float, float, float);
    bool calculateRPhiWindowOutToIn(const float, const float, const float);
    float calculateRPhiWindowInToOut(const float, const float, const float);

  public:
    
    // constructor 
    TrackFitter(const fitTypes ftIn, std::vector<float> paramIn, std::vector<int> layerIDs){
      fitType=ftIn;
      m_parameters=paramIn;
      m_debug = false;
      m_layerIDs = layerIDs;
    };

    TrackFitter(){
      fitType=MAX;
      std::vector<float> emptyVec;
      m_parameters = emptyVec;
      m_debug = false;
    }

    void debug(){ m_debug = true; }

    // public functions
    bool AssociateHits(hitContainer& hc);
    std::vector <myTrack> GetTracks();
    void ApplyCurvatureCut(float);

};
#endif // TrackFitter_h
