#ifndef myHit_h
#define myHit_h

/*#include "TLorentzVector.h"*/

class myHit {

private:

  float m_x, m_y, m_z, m_t; // coordinates of the hit position
  mutable float m_phi, m_eta, m_rho; // derived coordinates of the hit position
  unsigned int m_SurfaceID; // ID number 

  mutable bool m_rho_cache{false};
  mutable bool m_phi_cache{false};
  mutable bool m_eta_cache{false};

public:

  // constructor  
  myHit(float x, float y, float z, float t, unsigned int SurfaceID){
    m_x = x;
    m_y = y;
    m_z = z;
    m_t = t;

    // calculate rho, eta and phi when the functions are called, not before. 
    m_eta = -999.0;
    m_rho = -999.0;
    m_phi = -999.0;
  }

  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float t() const { return m_t; }

  /*const TLorentzVector& Position() const;*/
  /*TLorentzVector Position() const;*/

  float phi() const;
  float eta() const;
  float rho() const; 

  /***********
  double& operator[](unsigned int const i) { return m_coordinates[i]; }
  double const& operator[](unsigned int const i) const { return m_coordinates[i]; }
  unsigned int getId() const { return m_identifier; }
  const unsigned int identifier() const { return m_identifier; }

  UInt_t SurfaceID; // an ID number corresponding to the surface on which the hit was generated 

  // Particle properties
  TRef Particle; // reference to generated particle
  Int_t PID; // Particle ID
  Int_t IsPU; // did the hit come from a pileup particle
  Float_t PT; // pT of the particle that created the hit
  UInt_t intPtKeVID; // pT of the particle, converted to a int and into KeV, to be used as a poor mans ID method for pileup hits  
  ****************/

};

#endif // myHit_h
