#include "TrackFitter.h" 
#include <cmath>
#include <algorithm>
#include <utility>
#include "TMath.h"

#include <ctime>


inline float quickDeltaPhi(float phi1, float phi2){
  if(phi1<0) phi1+= 2*M_PI;
  if(phi2<0) phi2+= 2*M_PI;
  float dPhi= fabs(phi1 - phi2);
  if(dPhi > M_PI) dPhi = 2*M_PI - dPhi;
  return dPhi; 
}

bool sortByHypot( const std::pair< float, float>& a, const std::pair< float, float>& b){
  return (TMath::Hypot(a.first, a.second) < TMath::Hypot(b.first, b.second));
}

inline float quotient(float r, float r2, float param1, float param2){
  return pow(r2,4)*param1*param1 - r*r * r2*r2 *(r2*r2 - 4*param2*param2);
}

// print out map 
void printNewHitMap(std::map<std::string, std::vector<myHit*>> theMap){
  for(const auto& thep : theMap){
    std::cout << "ID: " << thep.first << "\t" << thep.second.size() << std::endl;
  }
}
void printNewHitMap(std::map<std::string, std::vector<myHit>>& theMap){
  for(const auto& thep : theMap){
    std::cout << "ID: " << thep.first << "\t" << thep.second.size() << std::endl;
  }
}

bool TrackFitter::AssociateHits(hitContainer& hc){

  // Associate hits together such that tracks can be formed
  // The hit association algorithm is determined 
  
  // Make sure these are empty, in case the function is called with a different algorithm 
  //m_associatedHitCollection.clear();
  m_tracks.clear();


  switch (fitType) { 
    case simpleLinear:{
       float minZ = m_parameters.at(0);
       float maxZ = m_parameters.at(1);
       try{
         return this->associateHitsSimple(hc, minZ, maxZ, false);
        }
       catch (const std::exception& exception){
         std::cout << "ERROR detected: " << exception.what() << std::endl;
         return this->associateHitsSimple(hc, minZ, maxZ, true);
       }
    }
    case MAX:{
      return false;
    }
  }
  return true; 
}

std::map<std::string, std::vector<myHit> > TrackFitter::associateHitsSimplePattern(hitContainer& hc, Location& loc, bool debug) const{

  if(debug) std::cout << "DEBUG\tbegin: associateHitsSimplePattern()" << std::endl;

  /******************************
   *  Separate collection of hits into "layer-eta-phi" regions
   *  for faster pattern recognition
   ***********************************/

  // Some pre-defined knowledge about the tracker
  const float barrelLength = 2250; // [mm] 

  // WJF: remove set-of-locations functionality
  //std::vector<std::string> setOfLocations; 

  //std::map<std::string, std::vector<myHit*> > newMap; 
  std::map<std::string, std::vector<myHit> > newMap; 
  for(const auto layer : m_layerIDs){
    for(myHit hit : hc.at(layer)){
      std::string locationString = loc.locationFromHit(hit);
      //newMap[ locationString ].push_back(&hit);  // pointer to hit 
      newMap[ locationString ].push_back(hit); 
      //setOfLocations.push_back(locationString);
    }
  }

  // add set of locations to the Location object
  //loc.addSetOfLocationsStrings(setOfLocations);

  // debug 
  //printNewHitMap(newMap); 
  if(debug) std::cout << "DEBUG\tend: associateHitsSimplePattern()" << std::endl;

  return newMap;
}

std::vector<myHit> concatenateVector(std::vector<myHit>& A, std::vector<myHit>& B){
  // https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
  std::vector<myHit> AB;
  AB.reserve( A.size() + B.size() ); // preallocate memory
  AB.insert( AB.end(), A.begin(), A.end() );
  AB.insert( AB.end(), B.begin(), B.end() );
  return AB;
}

std::vector<myHit> concatenateHitsBasedOnLocations(std::map<std::string, std::vector<myHit>>& hitMap, std::vector<std::string>& locations){
  // Return a vector of all hits in all of the regions selected by locations 
  // Can probably make this more efficient ? 
  std::vector<myHit> newVec;
  for(const auto& location : locations){
    /******************
    try{
      newVec = concatenateVector(newVec, hitMap.at(location));
    }
    catch(const std::out_of_range& oor){
      std::cout << "out f range error with: " << location << std::endl;
      std::cout << oor.what() << std::endl;
    }
    ********************/
      newVec = concatenateVector(newVec, hitMap[location]);
    
  }
  return newVec; 
}



std::vector<myHit*> concatenateHitsBasedOnLocations_Jon(const std::map<std::string, std::vector<myHit*>>& hitMap, const std::vector<std::string>& locations) {

  // JB attempt, keeps giving out of range error 
  std::vector<const std::vector<myHit*>*> vectorsToAdd;
  vectorsToAdd.reserve(locations.size() );
  std::size_t output_size(0);
  for (const std::string& location : locations) {
    try{
      vectorsToAdd.push_back(&hitMap.at(location) );
      output_size += vectorsToAdd.back()->size();
    }
    catch(const std::out_of_range& oor){
      std::cout << "out f range error with: " << location << std::endl;
      std::cout << oor.what() << std::endl;
    }
  }
  std::vector<myHit*> outputVec;
  outputVec.reserve(output_size);
  for (const std::vector<myHit*>* vecPtr : vectorsToAdd)
    std::copy(vecPtr->begin(), vecPtr->end(), std::back_inserter(outputVec) );
  return outputVec;
}


bool TrackFitter::associateHitsSimple(hitContainer& hc, float minZ, float maxZ, bool debug){

  if(debug) std::cerr << "DEBUG\tSome error detected, entering debug mode!" << std::endl;
  if(debug) std::cout << "DEBUG\tbegin: associateHitsSimple()" << std::endl;

    // create a Location object (really just a function ... ) 
    Location loc(0.06, 0.1);
    //loc.printProperties(); 

    // mapping of hits to eta-phi locations 
    std::map<std::string, std::vector<myHit>> hitMap = this->associateHitsSimplePattern(hc, loc, debug); 

    //////////////////////////////////////////
    // Simplest possible algorithm 
    //////////////////////////////////////////

    const float tolerance = 0.1; // [mm]

    // get the inner and outer barrel radii
    const int innerLayerID = m_layerIDs.at(0);
    const int outerLayerID = m_layerIDs.back();
    const int middleLayerID = 1; 

    if( hc.at(outerLayerID).size() == 0 || hc.at(middleLayerID).size() == 0 || hc.at(innerLayerID).size() == 0){
      // no hits in the outer layer _at all_ for this event
      return false;
    }

    if(debug) std::cout << "DEBUG: there are " << hc.at(innerLayerID).size() << " hits in the inner layer" << std::endl;
    if(debug) std::cout << "DEBUG: there are " << hc.at(middleLayerID).size() << " hits in the middle layer" << std::endl;
    if(debug) std::cout << "DEBUG: there are " << hc.at(outerLayerID).size() << " hits in the outer layer" << std::endl;
    if(debug) std::cout << "DEBUG: getting radii..." << std::endl;

    const float rInner = hc.at(innerLayerID).at(0).rho();
    const float rOuter = hc.at(outerLayerID).at(0).rho(); 
    //const float rMiddle = hf[middleLayerID].at(0).rho();
    
    if(debug) std::cout << "DEBUG: found the inner and outer radii. Inner: " << rInner << "\touter: " << rOuter << std::endl;

    // reserve some space for the tracks (performance)  
    m_tracks.clear();
    m_tracks.reserve( hc[outerLayerID].size() ); 

    // Calculate the phi window in which the hits in the outer layer must match
    //float trackPtMin = 1.0; // [GeV] (minimum track pT to consider for phiWindow calculation)
    const float trackPtMin = 2.0; // [GeV] (minimum track pT to consider for phiWindow calculation)
    const float bendingRadius = 1000 * trackPtMin/1.199; // [mm]
    float phiWindow = fabs( acos(rInner / (2*bendingRadius)) - acos(rOuter / (2*bendingRadius)) );
    phiWindow *= 2; // multiply by two, to have the deviation travelling in either direction. 
    //phiWindow *= 1.1; // inflate by 10% 


    // Draw a line between the hit in the innermost and outermost layer
    // See if there is a hit on the line in the intermediate layer (within some tolerance)
    for(const myHit& innerHit : hc[innerLayerID]){

      const float zInner = innerHit.z();
      const float phiInner = innerHit.phi();

      // get locations (areas) for other hits
      std::string innerHitLocation = loc.locationFromHit(innerHit); 
      std::vector<std::string> outerHitLocations  = loc.listOfLocationsInLayer(innerHitLocation, outerLayerID );
      std::vector<std::string> middleHitLocations = loc.listOfLocationsInLayer(innerHitLocation, middleLayerID);

      // get vector of hits defined by list of locations
      std::vector<myHit> outerHitVector  = concatenateHitsBasedOnLocations(hitMap, outerHitLocations);
      std::vector<myHit> middleHitVector = concatenateHitsBasedOnLocations(hitMap, middleHitLocations);


      for(const myHit& outerHit : outerHitVector){

        // must be within phi criteria  
        if( quickDeltaPhi(phiInner, outerHit.phi()) > phiWindow){
          //std::cout << "phiInner: " << phiInner << " phiOuter: " << outerHit.phi() << std::endl;
          //std::cout << "Rejected by inner--outer phi constraint" << std::endl;
          continue; 
        }

        // calculate parameters of line from inner hit to outer hit 
        LineParameters params;
        //params.calculateLineParameters(zInner, rInner, zOuter, rOuter);
        params.calculateLineParameters(innerHit.z(), innerHit.rho(), outerHit.z(), outerHit.rho());

        // reject if line does not point to within 3 sigma of the luminous region
        const float beamlineIntersect = params.x_intercept() ;
        if(beamlineIntersect > maxZ || beamlineIntersect < minZ){
          //std::cout << "Rejected by beamline constraint" << std::endl;
          continue;
        }


        //for(const auto& intermediateHit : hc[middleLayerID]){
        for(const myHit& intermediateHit : middleHitVector){


          // intersection of the line with the intermediate layer
          const float intersect = (intermediateHit.rho() - params.y_intercept())/params.gradient();

          const float zMiddle = intermediateHit.z();

          // only select if intermediate hit matches within tolerance along Z  
          if(fabs( zMiddle - intersect) < tolerance){

            // reject the intermediate hit if it is also outside the phi window
            if( quickDeltaPhi(phiInner, intermediateHit.phi()) > phiWindow){
              //std::cout << "Rejected by inner--middle phi constraint" << std::endl;
              continue; 
            }

            // Three hits are matched -> a track 
            std::vector<myHit> matchedHits;
            matchedHits.reserve(4); // prevent vector from having to grow 
            matchedHits.push_back(innerHit); 
            matchedHits.push_back(intermediateHit);
            matchedHits.push_back(outerHit);

            //m_tracks.push_back( myTrack(matchedHits) ); 
            m_tracks.emplace_back( matchedHits ); 
          }
        }
      }
    }

    if(debug) std::cout << "end: associateHitsSimple()" << std::endl;

    return true;
}

float TrackFitter::calculateRPhiWindowInToOut(const float a, const float b, const float r){
  /***********
   * Calculates the maximum deviation in phi that a particle could have traversed 
   * when travelling from a hit point (a, b) in an inner barrel layer to an outer barrel 
   * layer with radius r 
   * ************************/

  // Radius of trajectory
  float R = r/2.0;

  // centres of the circle (alpha1, beta2) and (alpha2, beta1)
  float quotient = sqrt( (4*R*R - a*a - b*b) * (a*a + b*b) );
  float q = quotient / (a*a + b*b); 

  float alpha1 = 0.5 * ( a + b*q );
  float alpha2 = 0.5 * ( a - b*q );

  float beta1 = 0.5 * ( b + a*q );
  float beta2 = 0.5 * ( b - a*q );

  // angles
  float phi1 = atan2(beta1, alpha2);

  float phi2 = atan2(beta2, alpha1);

  if(m_debug){
    std::cout << "a, b, r : " << a << " " << b << " " << r  << std::endl;
    std::cout << "(a1, b2) (" << alpha1 << ", " << beta2 << ") : phi = " << phi2/M_PI << "pi" << std::endl;
    std::cout << "(a2, b1) (" << alpha2 << ", " << beta1 << ") : phi = " << phi1/M_PI << "pi" <<  std::endl;
    std::cout << "" << std::endl; 
  }
  
  // return the max deviation
  float deltaPhi = acos(cos(phi1 - phi2))/2.0; 
  if( isnan(deltaPhi) ){
    std::cerr << "ERROR: calculateRPhiWindowInToOut() deltaPhi is NAN!" << std::endl;
    return 0;
  }

  return deltaPhi;

}

bool TrackFitter::calculateRPhiWindowOutToIn(const float r2, const float a, const float b){
  /***********
   * Calculates the coordinates of the intersection of:
   * - a circle defined as touching the origin and point (a,b)
   * - with another circle of radius r2 which is centered at the origin
   * Note that: r2^2 < a^2 + b^2
   * ************************/

  // check geometry of coordinates
  if ((a*a + b*b) < r2*r2){
    std::cerr << "ERROR: hit coordinates are inside the barrel layer" << std::endl;
    return false;
  }

  // Calculate center of circle (0, 0) -- (a, b)
  float alpha = a/2;
  float beta  = b/2; 
  // radius of the circle
  float rad = sqrt(alpha*alpha + beta*beta); 

  // coordinates of the intersection
  float x1 = (r2*r2*alpha + sqrt( quotient(rad, r2, alpha, beta) )) / (2*rad*rad);
  float x2 = (r2*r2*alpha - sqrt( quotient(rad, r2, alpha, beta) )) / (2*rad*rad);

  float y1 = (r2*r2*beta + sqrt( quotient(rad, r2, beta, alpha) )) / (2*rad*rad);
  float y2 = (r2*r2*beta - sqrt( quotient(rad, r2, beta, alpha) )) / (2*rad*rad);

  // dont know which comination are on the circle with radius r2, check
  /******************
   * WJF: will probably have an analytic solition! 
  std::cout << "rad: " << rad << std::endl;  
  float h1 = TMath::Hypot(x1, y1);
  float h2 = TMath::Hypot(x1, y2);
  float h3 = TMath::Hypot(x2, y1);
  float h4 = TMath::Hypot(x2, y2);
  std::cout << "(x1, y1) : h1 = " << h1 << std::endl;
  std::cout << "(x2, y1) : h2 = " << h2 << std::endl;

  std::vector< std::pair<float, float> > combinations; // could use map, but with integer index so it's the same as a vector
  combinations.push_back( std::make_pair( x1, y1 ) ); 
  combinations.push_back( std::make_pair( x1, y2 ) ); 
  combinations.push_back( std::make_pair( x2, y1 ) ); 
  combinations.push_back( std::make_pair( x2, y2 ) ); 

  // sort the combinations 
  // TODO: Write correct sorting angle
  std::sort(combinations.begin(), combinations.end(), sortByHypot); 

  //
  std::pair<float, float> c1, c2;
  c1 = combinations.at(1);
  c2 = combinations.at(2); 

  // now find angles 
  // TODO: check this gives the correct angle (!) 
  float phi1 = atan2( c1.second, c1.first ) ;
  float phi2 = atan2( c2.second, c2.first ) ;
  *****************************/

  // return these angles

  return true;  
}



float TrackFitter::calculateZWindowForNextLevel(float y0, float x0, float y2, float x1){
  /***********************************************
   * Calculate the parameters of the straight line passing through the coordinates (x0, y0), (x1, y1) where y1=0
   * Return the x coordinate of the line at y2
   * ********************************************/
  float y1 = 0.0;
  
  // line parameters
  LineParameters params;
  params.calculateLineParameters(x0, y0, x1, y1);

  return (y2 - params.y_intercept())/params.gradient(); 
}



std::vector <myTrack> TrackFitter::GetTracks(){
  return m_tracks;
}


void TrackFitter::ApplyCurvatureCut(float cut){
  // loop over all tracks, remove those with curvature difference deemed too large
  std::vector< myTrack > newVec;
  for(const myTrack& track: m_tracks){
    if( fabs(track.kappa_bc() - track.kappa_nbc()) < cut){
      newVec.push_back(track);
    }
  }
  m_tracks = newVec; 

}