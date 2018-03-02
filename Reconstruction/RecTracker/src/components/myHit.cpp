#include "myHit.h"
#include <cmath>

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
    //m_eta = this->Position().Eta(); 
    m_eta = 5;
    m_eta_cache = true;
  }
  return m_eta;
}

//const TLorentzVector& myHit::Position() const {
//TLorentzVector myHit::Position() const {
//TLorentzVector vec;
//vec.SetXYZT(m_x, m_y, m_z, m_t);
//return vec;
//}
