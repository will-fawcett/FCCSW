#ifndef GENERATION_MOMENTUMRANGEPARTICLEGUNPT_H
#define GENERATION_MOMENTUMRANGEPARTICLEGUNPT_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Generation/IParticleGunTool.h"

class MomentumRangeParticleGunPt : public GaudiTool, virtual public IParticleGunTool {

public:
  /// Constructor
  MomentumRangeParticleGunPt(const std::string& type, const std::string& name, const IInterface* parent);

  /// Destructor
  virtual ~MomentumRangeParticleGunPt();

  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle(Gaudi::LorentzVector& momentum, Gaudi::LorentzVector& origin, int& pdgId);

  /// Print counters
  virtual void printCounters() { ; };
  virtual StatusCode getNextEvent(HepMC::GenEvent&);

private:
  /// Minimum momentum (Set by options)
  Gaudi::Property<double> m_minMom{this, "MomentumMin", 100.0 * Gaudi::Units::GeV, "Minimal transverse momentum"};
  /// Minimum theta angle (Set by options)
  Gaudi::Property<double> m_minEta{this, "EtaMin", 0, "Minimal Eta"};
  /// Minimum phi angle (Set by options)
  Gaudi::Property<double> m_minPhi{this, "PhiMin", 0. * Gaudi::Units::rad, "Minimal phi"};

  /// Maximum momentum (Set by options)
  Gaudi::Property<double> m_maxMom{this, "MomentumMax", 100.0 * Gaudi::Units::GeV, "Maximal transverse momentum"};
  /// Maximum theta angle (Set by options)
  Gaudi::Property<double> m_maxEta{this, "EtaMax", 0.1, "Maximal Eta"};
  /// Maximum phi angle (Set by options)
  Gaudi::Property<double> m_maxPhi{this, "PhiMax", Gaudi::Units::twopi* Gaudi::Units::rad, "Maximal phi"};

  /// Momentum range
  double m_deltaMom;
  /// Theta range
  double m_deltaEta;
  /// Phi range
  double m_deltaPhi;

  /// Pdg Codes of particles to generate (Set by options)
  Gaudi::Property<std::vector<int>> m_pdgCodes{this, "PdgCodes", {-211}, "list of pdg codes to produce"};

  /// Masses of particles to generate (derived from PDG codes)
  std::vector<double> m_masses;

  /// Names of particles to generate (derived from PDG codes)
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator;
};

#endif  // GENERATION_MOMENTUMRANGEPARTICLEGUN_H