
#include "DetInterface/ITrackingGeoSvc.h"
#include "RecInterface/ITrkVolumeManagerSvc.h"
#include "DetInterface/IGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/ISaveTrackStateTool.h"


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

#include "datamodel/TrackHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"



#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"
#include "DDRec/API/IDDecoder.h"

#include <cmath>
#include <random>

#include "TrackFit.h"
#include "FastHelix.h"
#include "TrackParameterConversions.h"
#include "ACTSLogger.h"

using namespace Acts;


DECLARE_ALGORITHM_FACTORY(TrackFit)

TrackFit::TrackFit(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("trackSeedingTool", m_trackSeedingTool, "Track seeding tool" );
  declareProperty("SaveTrackStateTool", m_saveTrackStateTool, "Track saving tool" );

  }

TrackFit::~TrackFit() {}

StatusCode TrackFit::initialize() {
  info() << "initialize" << endmsg;

  m_geoSvc = service ("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_trkGeoSvc = service("TrackingGeoSvc");
  if (nullptr == m_trkGeoSvc) {
    error() << "Unable to locate Tracking Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }

  m_trkVolMan = service("TrkVolMan");
  if (nullptr == m_trkVolMan) {
    error() << "Unable to locate Tracking Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }

  m_trkGeo = m_trkGeoSvc->trackingGeometry();
  return sc;
}

StatusCode TrackFit::execute() {

  // initialize collection for tracks
  m_saveTrackStateTool->newCollection();
  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();

  m_exEngine = initMyExtrapolator(m_trkGeo);
  Acts::KalmanFitter<MyExtrapolator, CacheGenerator, NoCalibration, Acts::GainMatrixUpdator> m_KF;
  m_KF.m_oCacheGenerator = CacheGenerator();
  m_KF.m_oCalibrator = NoCalibration();
  m_KF.m_oExtrapolator = MyExtrapolator(m_exEngine);
  m_KF.m_oUpdator = GainMatrixUpdator();

  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
  auto m_decoderBarrel = readoutBarrel.idSpec().decoder();
  auto segmentationXZ = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXZ*>(
          readoutBarrel.segmentation().segmentation());


  double fcc_l1 = 0;
  double fcc_l2 = 0;
  GlobalPoint middlePoint;
  GlobalPoint outerPoint;

  std::multimap<unsigned int, unsigned int>  seedmap = m_trackSeedingTool->findSeeds(hits);


  for (std::multimap<unsigned int, unsigned int>::iterator it = seedmap.begin(); it != seedmap.end(); ++it) {
    // loop over hits belonging to one track by checking that their ids match
    // check ids of the next hit, except for the last hit check for the previous hit
    std::vector<FitMeas_t> fccMeasurements;
    std::vector<const Acts::Surface*> surfVec;
    fccMeasurements.reserve(hits->size());
    unsigned int l_currentTrackID = it->first;
    int hitcounter = 0;
    do {
      debug() << std::distance(it, seedmap.begin()) << "\t" << it->first << endmsg;
      debug() << "trackID: " << it->first << endmsg;
      auto hit = (*hits)[it->second]; // the TrackID maps to an index for the hit collection
      long long int theCellId = hit.core().cellId;
      debug() << theCellId << endmsg;
      debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
      debug() << "phi: " << std::atan2(hit.position().y, hit.position().x) << endmsg;
      m_decoderBarrel->setValue(theCellId);
      int system_id = (*m_decoderBarrel)["system"];
      debug() << " hit in system: " << system_id;
      int layer_id = (*m_decoderBarrel)["layer"];
      debug() << "\t layer " << layer_id;
      int module_id = (*m_decoderBarrel)["module"];
      debug() << "\t module " << module_id;
      debug() << endmsg;
      // The conventions in DD4hep and ACTS for the local coordinates differ,
      // and the conversion needs to reflect this. See the detector factories for details.
      fcc_l1 =  (*m_decoderBarrel)["x"] * segmentationXZ->gridSizeX() * CM_2_MM;
      fcc_l2 = -1 *  (*m_decoderBarrel)["z"] * segmentationXZ->gridSizeZ() * CM_2_MM;
      (*m_decoderBarrel)["x"] = 0; // workaround for broken `volumeID` method --
      (*m_decoderBarrel)["z"] = 0; // set everything not connected with the VolumeID to zero,
      // so the cellID can be used to look up the tracking surface
      if (hitcounter == 0) {
        middlePoint = GlobalPoint(hit.position().x, hit.position().y, hit.position().z);
      } else if (hitcounter == 1) {
        outerPoint = GlobalPoint(hit.position().x, hit.position().y, hit.position().z);
      }
      // need to use cellID without segmentation bits
      const Acts::Surface* fccSurf = m_trkVolMan->lookUpTrkSurface(Identifier(m_decoderBarrel->getValue()));
      debug() << " found surface pointer: " << fccSurf<< endmsg;
      if (nullptr != fccSurf) {
        double std1 = 1;
        double std2 = 1;
        ActsSymMatrixD<2> cov;
        cov << std1* std1, 0, 0, std2* std2;
        fccMeasurements.push_back(Meas_t<eLOC_0, eLOC_1>(*fccSurf, hit.core().cellId, std::move(cov), fcc_l1, fcc_l2));
        surfVec.push_back(fccSurf);
        ++hitcounter;
      }
      debug() << it->first << " " << it->second << endmsg;
      ++it;
      debug() << it->first << " " << it->second << endmsg;
    } while ((it->first == l_currentTrackID) && (it != seedmap.end()));

    
    debug() << "Estimating parameters..." << endmsg;
    FastHelix helix(outerPoint, middlePoint, GlobalPoint(0,0,0), 0.002);
    PerigeeTrackParameters res = ParticleProperties2TrackParameters(GlobalPoint(0,0,0), helix.getPt(), 0.002, 1);
    
    ActsVector<ParValue_t, NGlobalPars> pars;
    pars << res.d0, res.z0, res.phi0, res.theta, res.qOverPt;
    info() << "Estimated track parameters: " << res.d0 << "\t" << res.z0 << "\t" << res.phi0 << "\t" << res.theta << "\t" << res.qOverPt << endmsg;
    //pars << 0, 0, M_PI * 0.5, M_PI * 0.5, 0.001;
    auto startCov =
        std::make_unique<ActsSymMatrix<ParValue_t, NGlobalPars>>(ActsSymMatrix<ParValue_t, NGlobalPars>::Identity());
    (*startCov) *= 0.01; // starting values of 1 are too big for qop
    

    const Surface* pSurf =  m_trkGeo->getBeamline();

    info() << "Beamline pointer: " << pSurf << endmsg;;
    auto startTP = std::make_unique<BoundParameters>(std::move(startCov), std::move(pars), *pSurf);
    //info() << "trying parameters" <<  *startTP << endmsg;

    ExtrapolationCell<TrackParameters> exCell(*startTP);
    exCell.addConfigurationMode(ExtrapolationMode::CollectSensitive);
    exCell.addConfigurationMode(ExtrapolationMode::CollectPassive);
    exCell.addConfigurationMode(ExtrapolationMode::CollectBoundary);

    m_exEngine->extrapolate(exCell);

    info() << "got " << exCell.extrapolationSteps.size() << " extrapolation steps" << endmsg;


    std::vector<FitMeas_t> vMeasurements;
    vMeasurements.reserve(exCell.extrapolationSteps.size());

    // identifier
    long int id = 0;
    // random numbers for smearing measurements
    /// @todo: use random number service
    std::default_random_engine e;
    std::uniform_real_distribution<double> std_loc1(1, 5);
    std::uniform_real_distribution<double> std_loc2(0.1, 2);
    std::normal_distribution<double> g(0, 1);

    double std1, std2, l1, l2;
    for (const auto& step : exCell.extrapolationSteps) {
      const auto& tp = step.parameters;
      auto detPtr = tp->referenceSurface().associatedDetectorElement();
      if (detPtr == nullptr) continue;

      std1 = 1;
      std2 = 1;
      l1 = tp->get<eLOC_0>();
      l2 = tp->get<eLOC_1>();
      ActsSymMatrixD<2> cov;
      cov << std1* std1, 0, 0, std2* std2;
      vMeasurements.push_back(Meas_t<eLOC_0, eLOC_1>(tp->referenceSurface(), id, std::move(cov), l1, l2));
      ++id;
    }

    info() << "created " << vMeasurements.size() << " pseudo-measurements" << endmsg;
    //for (const auto& m : vMeasurements)
    //  debug() << m << endmsg;


    info() << "created " << fccMeasurements.size() << " fcc-measurements" << endmsg;
    //for (const auto& m : fccMeasurements)
    //  debug() << m << endmsg;

    auto track = m_KF.fit(vMeasurements, std::make_unique<BoundParameters>(*startTP));

    // dump track
   unsigned int trackStateCounter = 0;
   for (const auto& p : track) {
      if (trackStateCounter == vMeasurements.size()) { // use the last track state for persistency
        m_saveTrackStateTool->saveTrackInCollection(*p->getFilteredState());
      }
      ++trackStateCounter;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackFit::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
