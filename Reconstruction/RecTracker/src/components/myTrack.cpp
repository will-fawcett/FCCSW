#include <iostream>
#include "myTrack.h"
#include "LineParameters.h"


bool myTrack::calculateTrackParameters( cartesianCoordinate coord, trackParamAlgo algorithm ){

  // function to calculate d0, z0 (and in principle theta, phi, qOverP)
  // relative to the specified coordinate!!!!!
  // TODO: add calculation if with respect to NOT (0, 0, 0)
  
  switch (algorithm) {
    case triplet:{
      return this->trackParametersTriplet();
    }
    case MAXIMUM:{
      return false;
    }
  }

  return true; 
}


bool myTrack::trackParametersTriplet(){

  /****************
   * First calculate the track parameters assuming a beamline constraint (and using the first and third hit)
   * Then calculate the track parameters using only the three hits
   * Good tracks will have consistent parameters
   * **************/

  ///////////////////////////////
  // Calculate track parameters with beamline constraint 
  // Assumes the track originates from (0, 0, z0)
  // Calculates the track parameters using the origin and two other points, corresponding to the innermost and outermost hit
  
  if(m_associatedHits.size() != 3){
    std::cerr << "ERROR: more than three hits associate to this track. Algorith is not compatible." << std::endl;
    return false;
  }

  //const myHit * hit1 = m_associatedHits.at(0);
  //const myHit * hit2 = m_associatedHits.at(1);
  //const myHit * hit3 = m_associatedHits.at(2);
  myHit hit1 = m_associatedHits.at(0);
  myHit hit2 = m_associatedHits.at(1);
  myHit hit3 = m_associatedHits.at(2);

  // Check hits are properly orderd radially. Shouldn't really happen
  if(hit1.rho() > hit2.rho() || hit2.rho() > hit3.rho()){
    std::cerr << "ERROR: hits not in the correct order!" << std::endl;
    std::cerr << "hit1: " << hit1.rho() << "\t (" << hit1.x() << ", " << hit1.y() << ", " << hit1.y() << ")" << " check: " << sqrt(hit1.x()*hit1.x()+hit1.y()*hit1.y()) << std::endl;
    std::cerr << "hit2: " << hit2.rho() << "\t (" << hit2.x() << ", " << hit2.y() << ", " << hit2.y() << ")" << " check: " << sqrt(hit2.x()*hit2.x()+hit2.y()*hit2.y()) << std::endl;
    std::cerr << "hit3: " << hit3.rho() << "\t (" << hit3.x() << ", " << hit3.y() << ", " << hit3.y() << ")" << " check: " << sqrt(hit3.x()*hit3.x()+hit3.y()*hit3.y()) << std::endl;
    return false;
  }

  // copy of hit coordinates
  float x1 = hit1.x();
  float y1 = hit1.y(); 
  float z1 = hit1.z(); 

  float x2 = hit2.x();
  float y2 = hit2.y(); 
  float z2 = hit2.z(); 

  float x3 = hit3.x();
  float y3 = hit3.y(); 
  float z3 = hit3.z(); 

  float r01 = hit1.rho(); // = sqrt(x^2 + y^2) 
  float r02 = hit2.rho();
  float r03 = hit3.rho(); 

  // Calculate the parameters in the longitudinal plane
  // Follows calculations by A. Schoning

  float r13 = hypotf( fabs(x3-x1) , fabs(y3-y1) ); // not sure if fabs actuall needed ...  
  float chord13 = x1*y3 - y1*x3; 
  float PHI1 = 2 * asin( chord13 / (r13*r03) );
  float PHI3 = 2 * asin( chord13 / (r13*r01) );

  // Calculate the track parameters in the transverse plane
  // Given the two points and the beamline constraint
  // This can be solved analytically ... 
   
  // calculate the centers of the circle touching coordinates (0,0) (x1, y1) (x3, y3)
  // described by (x-a)^2 + (y-b)^2 = R^2
  float a = (y3*r01*r01 - y1*r03*r03) / (2*(y3*x1 - y1*x3));
  float b = (x3*r01*r01 - x1*r03*r03) / (2*(x3*y1 - x1*y3));

  // radius of trajectory  
  //float radiusAndre = ( (x1*x1 - x3*x3) + (y1*y1 - y3*y3) ) / ( 2*(x1 - x3) ); // not sure if formulea in Andres paper is quite correct ... ? 
  float radiusAndre = (r01 * r03 * r13) / (2*chord13); 
  float radius = sqrt(a*a + b*b); 

  // kappa (1/radius) 
  float kappa_013 = 1/radius; 

  // transverse momentum
  float pT = 1.199 * fabs(radius/1000); // for a 4T magnetic field and radius in [m], divide by 1000 as length units in [mm]

  // arc lengths 
  float s1 = radius * PHI1;
  float s3 = radius * PHI3; 


  // phi angle given by line tangent to the circle at (0,0) 
  // atan2(y, x)
  float phi = atan2f(-b, a); // will return [-pi, pi] 

  // Assign the track parameters
  m_z0 = z1 - s1* ( z3 - z1 ) / (s3 - s1);
  m_theta = atan2f( (s3-s1), (z3-z1) );  // atan2f returns theta in [-pi, pi] 


  // check eta calculation is correct
  m_eta = -1*log( tan( fabs(m_theta)/2.0 )); // take fabs(theta), want -pi and pi to be treated the same
  if(isnan(m_eta)){
    std::cerr << "trackParametersBeamlineConstraint(): ERROR: Eta calculation performed incorrectly." << std::endl; 
    std::cerr << "m_theta: " << m_theta << std::endl;
    std::cerr << "tan(fabs(m_theta)/2.0): " << tan(fabs(m_theta)/2.0) << std::endl;
    std::cerr << "log(tan(fabs(m_theta))): " << log( tan(fabs(m_theta)/2.0) ) << std::endl;
    std::cerr << "Curious and curiouser ... " << std::endl;
    m_eta = -100.0;
    return false;
  }
  m_d0 = 0.0; // by definition (beamline constraint) 
  m_pT = pT;
  m_phi = phi;
  m_kappa_013 = kappa_013; 


  /////////////////////////////////////////////
  // Calculate the track parameters without the beamline constraint
  /////////////////////////////////////////////
  
  // circle described by (x-a)^2 + (y-b)^2 = r^2 
  float b_nbc = ( (x3 - x1)*(r02*r02 - r01*r01) - (x2 - x1)*(r03*r03 - r01*r01) ) / ( 2*( (y2-y1)*(x3-x1) - (y3-y1)*(x2-x1)  ) );
  float a_nbc = ( r02*r02 - r01*r01 - 2*b*(y2 - y1) ) / ( 2*(x2 - x1));
  float radius_nbc = hypotf( (x1-a_nbc), (y1-b_nbc) );

  // Andre's calculation
  float chord_123 = x2*y3 + x1*y2 + x3*y1 - x3*y2 -x2*y1 - x1*y3;
  float r12 = hypotf( (x2-x1), (y2-y1) );
  float r23 = hypotf( (x3-x2), (y3-y2) );
  m_kappa_123 = 2*chord_123 / (r12 * r13 * r23 ); 

  return true;
}


bool myTrack::isNotFake() const{
  //return (!this->isNotFake()); 
  return (!this->isFake()); 
}

//bool myTrack::isNotFake() const{
bool myTrack::isFake() const{

  // If all of the hits associated to this track have the same particle ID, the the track is not a fake track
  
  std::vector<int> uniqueIDs;
  for(const myHit& hit : m_associatedHits){
    uniqueIDs.push_back(hit.particleID());
  }

  for(int i=0; i<m_associatedHits.size()-1; ++i){
    if(m_associatedHits.at(i).particleID() != m_associatedHits.at(i+1).particleID()){

      std::cout << "fake detected, UID: " << uniqueIDs.at(i) << ", " << uniqueIDs.at(i+1) << std::endl;
      std::cout << "pT: " << m_associatedHits.at(i).pT() << ", " << m_associatedHits.at(i+1).pT() << std::endl;
      this->printHitInfo();
      return true; // track is fake if any of these are different 
    }
  }
  return false;

}

void myTrack::printTrackParameters() const {
   std::cout << "d0: " << m_d0 << std::endl;
   std::cout << "z0: " << m_z0 << std::endl;
   std::cout << "phi: " << m_phi << std::endl;
   std::cout << "theta: " << m_theta << std::endl;
   std::cout << "eta: " << m_eta << std::endl;
   std::cout << "pT: " << m_pT << std::endl;
}

void myTrack::printHitInfo() const {
  std::cout << "Track has " << m_associatedHits.size() << " hits." << std::endl; 
  //std::cout << "isFake: " << this->isFake() << std::endl;
  this->printTrackParameters();

  //for(const myHit& hit : m_associatedHits){
  for(const myHit& hit : m_associatedHits){
    std::cout << "\t(" << hit.x() << ", " << hit.y() << ", " << hit.z() << ") " 
      << " r=" << hit.rho() << " phi=" << hit.phi() // << " eta=" << hit.Eta eta not available ? 
      << " rCalc=" << sqrt(hit.x()*hit.x() + hit.y()*hit.y()) 
      << "\tSurfaceID: " << hit.SurfaceID() 
      << ", ParticleID: " << hit.particleID()
      << ", identifier: " << hit.identifier() 
      //<< "\tpu: " << hit.IsPU 
      //<< " pt: " << hit.PT
      //<< " ID: " << hit.intPtKeVID
      << std::endl; 
  }

}
