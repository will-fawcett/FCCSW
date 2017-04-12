
#include "DetInterface/ITrackingGeoSvc.h"
#include "DetInterface/IGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"

#include "datamodel/TrackHitCollection.h"

#include "ACTS/Utilities/Identifier.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/EventData/Measurement.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
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



#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDRec/API/IDDecoder.h"

#include <cmath>
#include <random>

#include "ExtrapolationTest.h"



DECLARE_ALGORITHM_FACTORY(ExtrapolationTest)

ExtrapolationTest::ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits,"hits/TrackerPositionedHits");

  }

ExtrapolationTest::~ExtrapolationTest() {}

StatusCode ExtrapolationTest::initialize() {

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

StatusCode ExtrapolationTest::execute() {

    auto lcdd = m_geoSvc->lcdd();
    auto allReadouts = lcdd->readouts();
    auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
    auto m_decoderBarrel = readoutBarrel.idSpec().decoder();

  fcc::PositionedTrackHitCollection* phitscoll = new fcc::PositionedTrackHitCollection();
  fcc::TrackHitCollection* hitscoll = new fcc::TrackHitCollection();

  std::vector<const Acts::Surface*> surfVec;

  /// @todo: get local position from segmentation
  double fcc_l1 = 0;
  double fcc_l2 = 0;
  int hitcounter = 0;

  std::uniform_real_distribution<double> std_loc1(1, 5);
  std::uniform_real_distribution<double> random_angle(0.0, 1.57);
  std::normal_distribution<double> g(0, 1);
  

  ActsVector<ParValue_t, NGlobalPars> pars;
  pars << 0,0,0,1, 0.001;
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
  propConfig.fieldService = std::make_shared<ConstantBField>(0, 0, 0.004);
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
  std::cout << "start extrapolation ..." << std::endl;
  m_exEngine->extrapolate(exCell);

  debug() << "got " << exCell.extrapolationSteps.size() << " extrapolation steps" << endmsg;




  double std1, std2, l1, l2;
  for (const auto& step : exCell.extrapolationSteps) {
    const auto& tp = step.parameters;
    //if (tp->associatedSurface().type() != Surface::Plane) continue;
    fcc::TrackHit edmHit = hitscoll->create();
    fcc::BareHit& edmHitCore = edmHit.core();
    auto position = fcc::Point();
    std::cout <<"accessing global position" <<  std::endl;
    std::cout << tp->position().x()<< std::endl;
    position.x = tp->position().x();
    position.y = tp->position().y();
    position.z = tp->position().z();
    phitscoll->create(position, edmHitCore);
  }

  m_positionedTrackHits.put(phitscoll);
  //m_trackHits.put(hitscoll);
  return StatusCode::SUCCESS;
}

StatusCode ExtrapolationTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
