#include "SimG4SaveTrackerHitsWithDigiSmearing.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "SimG4Common/Units.h"

// Gaudi
#include "GaudiKernel/IRndmGenSvc.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDSegmentation/CartesianGridXZ.h"

DECLARE_TOOL_FACTORY(SimG4SaveTrackerHitsWithDigiSmearing)

SimG4SaveTrackerHitsWithDigiSmearing::SimG4SaveTrackerHitsWithDigiSmearing(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("positionedTrackHits", m_positionedTrackHits, "Handle for tracker hits");
  declareProperty("smearedTrackHits", m_smearedTrackHits, "Handle for smeared tracker hits");
  declareProperty("trackHits", m_trackHits, "Handle for tracker hits including position information");
}

SimG4SaveTrackerHitsWithDigiSmearing::~SimG4SaveTrackerHitsWithDigiSmearing() {}

StatusCode SimG4SaveTrackerHitsWithDigiSmearing::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  for (auto& readoutName : m_readoutNames) {
    if (allReadouts.find(readoutName) == allReadouts.end()) {
      error() << "Readout " << readoutName << " not found! Please check tool configuration." << endmsg;
      return StatusCode::FAILURE;
    } else {
      debug() << "Hits will be saved to EDM from the collection " << readoutName << endmsg;
    }
  }

  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  m_gaussDist.initialize(randSvc, Rndm::Gauss(0.0, 1.0));

  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveTrackerHitsWithDigiSmearing::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveTrackerHitsWithDigiSmearing::saveOutput(const G4Event& aEvent) {
  auto lcdd = m_geoSvc->lcdd();
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4TrackerHit* hit;
  if (collections != nullptr) {
    auto edmPositions = m_positionedTrackHits.createAndPut();
    auto edmHits = m_trackHits.createAndPut();
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();
        info() << "\t" << n_hit << " hits are stored in a tracker collection #" << iter_coll << ": "
               << collect->GetName() << endmsg;
        // get position from cellID
        auto currentReadout = lcdd->readout(collect->GetName());
        auto currentDecoder = currentReadout.idSpec().decoder();
        auto segmentationXZ = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXZ*>(
                currentReadout.segmentation().segmentation());
        float segGridSizeX = segmentationXZ->gridSizeX() * CM_2_MM;
        float segGridSizeZ = segmentationXZ->gridSizeZ() * CM_2_MM;
        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<DD4hep::Simulation::Geant4TrackerHit*>(collect->GetHit(iter_hit));
          fcc::TrackHit edmHit = edmHits->create();
          fcc::BareHit& edmHitCore = edmHit.core();
          edmHitCore.cellId = hit->cellID;
          edmHitCore.energy = hit->energyDeposit * sim::g42edm::energy;
          edmHitCore.time = hit->truth.time;
          edmHitCore.bits = hit->truth.trackID;

          currentDecoder->setValue(hit->cellID);
          double l[3] = {(*currentDecoder)["x"] * segGridSizeX + m_gaussDist() * sqrt(12) * segGridSizeX,0, (*currentDecoder)["z"] * segGridSizeZ + m_gaussDist() * sqrt(12) * segGridSizeZ};
          double g[3] = {0,0,0};

          auto position = fcc::Point();
          position.x = hit->position.x() * sim::g42edm::length;
          position.y = hit->position.y() * sim::g42edm::length;
          position.z = hit->position.z() * sim::g42edm::length;
          edmPositions->create(position, edmHitCore);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
