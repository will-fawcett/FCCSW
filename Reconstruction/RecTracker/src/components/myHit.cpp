#include "myHit.h"
#include <cmath>
#include "TLorentzVector.h"

float myHit::rho() const{
  if(!m_rho_cache){
    m_rho = sqrt( m_x*m_x + m_y*m_y);
    m_rho_cache = true;
  }
  return m_rho;
}

float myHit::phi() const{
  if(!m_phi_cache){
    m_phi = m_x == 0.0 && m_y == 0.0 ? 0.0 : atan2f(m_y, m_x); // Return phi coordinate from [-pi, pi]
    m_phi_cache = true;
  }
  return m_phi;
}

float myHit::eta() const{
  if(!m_eta_cache){
    m_eta = this->calculateEta();
    m_eta_cache = true;
  }
  return m_eta;
}


// WJF: TLorentzVector doesn't work with FCCSW
// ETA calculation, copied from TLorentzVector and TVector 3
float myHit::calculateEta() const{
  float magnitude = sqrt(m_x*m_x + m_y*m_y + m_z*m_z); 
  float cosTheta = (magnitude == 0.0) ? 1.0 : m_z/magnitude; 
  if (cosTheta*cosTheta < 1) return -0.5* log( (1.0-cosTheta)/(1.0+cosTheta) );
  if (m_z == 0) return 0;
  //Warning("PseudoRapidity","transvers momentum = 0! return +/- 10e10");
  if (m_z > 0) return 10e10;
  else        return -10e10;
}

