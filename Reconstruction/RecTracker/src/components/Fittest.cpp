
#include "DetInterface/ITrackingGeoSvc.h"
#include "DetInterface/IGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"

#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "ACTS/Utilities/Identifier.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/EventData/Measurement.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Fitter/KalmanFitter.hpp"
#include "ACTS/Fitter/KalmanUpdator.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/ParticleCollection.h"



#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDRec/API/IDDecoder.h"

#include <cmath>
#include <random>

#include "Fittest.h"
#include "FitUtils.h"



DECLARE_ALGORITHM_FACTORY(Fittest)

Fittest::Fittest(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits,"hits/TrackerPositionedHits");
  declareProperty("trackStates", m_trackStates,"tracks/trackStates");
  declareProperty("recoParticles", m_recoParticles,"parts/m_recoParticles");

  }

Fittest::~Fittest() {}

StatusCode Fittest::initialize() {

  m_geoSvc = service ("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_trkGeoSvc = service("TrackingGeoSvc");
  if (nullptr == m_trkGeoSvc) {
    error() << "Unable to locate Tracking Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }


  m_trkGeo = m_trkGeoSvc->trackingGeometry();

  return sc;
}

StatusCode Fittest::execute() {


  fcc::PositionedTrackHitCollection* phitscoll = new fcc::PositionedTrackHitCollection();
  fcc::TrackHitCollection* hitscoll = new fcc::TrackHitCollection();
  fcc::TrackStateCollection* trackStateCollection = new fcc::TrackStateCollection();
  fcc::ParticleCollection* particleCollection = new fcc::ParticleCollection();

  std::vector<const Acts::Surface*> surfVec;

  /// @todo: get local position from segmentation
  double fcc_l1 = 0;
  double fcc_l2 = 0;
  int hitcounter = 0;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> std_loc1(1, 5);
  std::uniform_real_distribution<double> random_angle(0.0, 1.57);
  std::normal_distribution<double> g(0, 1);
  

  ActsVector<ParValue_t, NGlobalPars> pars;
  pars << 0,0,M_PI * 0.5,M_PI*0.5, 0.001;
  std::cout << pars << std::endl;
  auto startCov =
      std::make_unique<ActsSymMatrix<ParValue_t, NGlobalPars>>(ActsSymMatrix<ParValue_t, NGlobalPars>::Identity());

  const Surface* pSurf =  m_trkGeo->getBeamline();
  std::cout << "beampipe surface pointer" << std::endl;
  std::cout << pSurf << std::endl;
  auto startTP = std::make_unique<BoundParameters>(std::move(startCov), std::move(pars), *pSurf);

  ExtrapolationCell<TrackParameters> exCell(*startTP);
  exCell.addConfigurationMode(ExtrapolationMode::CollectSensitive);
  exCell.addConfigurationMode(ExtrapolationMode::CollectPassive);
  exCell.addConfigurationMode(ExtrapolationMode::CollectBoundary);

  auto propConfig = RungeKuttaEngine<>::Config();
  /// @todo: use magnetic field service
  propConfig.fieldService = std::make_shared<ConstantBField>(0, 0, 0.002);
  auto propEngine = std::make_shared<RungeKuttaEngine<>>(propConfig);

  auto matConfig = MaterialEffectsEngine::Config();
  auto materialEngine = std::make_shared<MaterialEffectsEngine>(matConfig);

  auto navConfig = StaticNavigationEngine::Config();
  navConfig.propagationEngine = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry = m_trkGeo;
  auto navEngine = std::make_shared<StaticNavigationEngine>(navConfig);

  auto statConfig = StaticEngine::Config();
  statConfig.propagationEngine = propEngine;
  statConfig.navigationEngine = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<StaticEngine>(statConfig);

  auto exEngineConfig = ExtrapolationEngine::Config();
  exEngineConfig.trackingGeometry = m_trkGeo;
  exEngineConfig.propagationEngine = propEngine;
  exEngineConfig.navigationEngine = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};
  m_exEngine = std::make_shared<ExtrapolationEngine>(exEngineConfig);

  debug() << "start extrapolation ..." << endmsg;
  m_exEngine->extrapolate(exCell);
  debug() << "got " << exCell.extrapolationSteps.size() << " extrapolation steps" << endmsg;


  std::vector<FitMeas_t> vMeasurements;
  vMeasurements.reserve(exCell.extrapolationSteps.size());
  int id;
  float std1, std2, l1, l2;


  for (const auto& step : exCell.extrapolationSteps) {
    const auto& tp = step.parameters;
    //if (tp->associatedSurface().type() != Surface::Plane) continue;
    fcc::TrackHit edmHit = hitscoll->create();
    fcc::BareHit& edmHitCore = edmHit.core();
    auto position = fcc::Point();
    position.x = tp->position().x();
    position.y = tp->position().y();
    position.z = tp->position().z();
    phitscoll->create(position, edmHitCore);

    std1 = 1;//std_loc1(e);
    std2 = 1; //std_loc2(e);
    l1 = tp->get<eLOC_0>(); // + std1 * g(e);
    l2 = tp->get<eLOC_1>(); // + std2 * g(e);
    ActsSymMatrixD<2> cov;
    cov << std1* std1, 0, 0, std2* std2;
    vMeasurements.push_back(Meas_t<eLOC_0, eLOC_1>(tp->referenceSurface(), id, std::move(cov), l1, l2));
    id++;
  }
  std::cout << vMeasurements.size() << std::endl;

  Acts::KalmanFitter<MyExtrapolator, CacheGenerator, NoCalibration, Acts::GainMatrixUpdator> m_KF;
  m_KF.m_oCacheGenerator = CacheGenerator();
  m_KF.m_oCalibrator = NoCalibration();
  m_KF.m_oExtrapolator = MyExtrapolator(m_exEngine);
  m_KF.m_oUpdator = GainMatrixUpdator();

  auto track = m_KF.fit(vMeasurements, std::make_unique<BoundParameters>(*startTP));

  // dump track
  int trackStateCounter = 0;
  for (const auto& p : track) {
    if (trackStateCounter > 0) {
    auto smoothedState =  *p->getSmoothedState();
    auto filteredState =  *p->getFilteredState();
    fcc::Point l_point;
    l_point.x = 1.;
    l_point.y = 2.;
    l_point.z = 3.;

    fcc::Particle l_Particle = particleCollection->create();
    auto& p4 = l_Particle.p4();
    auto& vertex = l_Particle.vertex();
    p4.px = smoothedState.momentum().x();
    p4.py = smoothedState.momentum().y();
    p4.pz = smoothedState.momentum().z();
    vertex.x = filteredState.momentum().x();
    vertex.y = filteredState.momentum().y();
    vertex.z = filteredState.momentum().z();
    l_Particle.status(trackStateCounter);
    l_Particle.pdgId(13);


    
    fcc::TrackState l_trackState = trackStateCollection->create();
    //std::cout << "phi: " << l_trackState.phi() << std::endl;
    l_trackState.phi(166.);


    }
    ++trackStateCounter;
  }

  m_positionedTrackHits.put(phitscoll);
  m_trackStates.put(trackStateCollection);
  m_recoParticles.put(particleCollection);
  //m_trackHits.put(hitscoll);
  return StatusCode::SUCCESS;
}

StatusCode Fittest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
