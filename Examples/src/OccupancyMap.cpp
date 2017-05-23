
#include "DetInterface/IGeoSvc.h"

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

#include "OccupancyMap.h"



DECLARE_ALGORITHM_FACTORY(OccupancyMap)

OccupancyMap::OccupancyMap(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "DummyTracker hits (Output)");

  }


void OccupancyMap::visitDetElement(const DD4hep::Geometry::DetElement& det) {
  for (auto d: det.children()) {
    debug() << "DetElement Name: " << d.first << endmsg;
    debug() d.second()
    visitDetElement(d.second);
  }
}

OccupancyMap::~OccupancyMap() {}

StatusCode OccupancyMap::initialize() {
  info() << "initialize" << endmsg;

  m_geoSvc = service("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;


  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
  m_decoderBarrel = readoutBarrel.idSpec().decoder();
  auto segmentationXZ = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXZ*>(
          readoutBarrel.segmentation().segmentation());
  m_segGridSizeX = segmentationXZ->gridSizeX() * CM_2_MM;
  m_segGridSizeZ = segmentationXZ->gridSizeZ() * CM_2_MM;
  return sc;
}

StatusCode OccupancyMap::execute() {
  fcc::PositionedTrackHitCollection* phitscoll = new fcc::PositionedTrackHitCollection();
  fcc::TrackHitCollection* hitscoll = new fcc::TrackHitCollection();


  auto lcdd = m_geoSvc->lcdd();
  visitDetElement(lcdd->world());



  for (int i = 0; i < 10; ++i) {
    fcc::TrackHit edmHit = hitscoll->create();
    fcc::BareHit& edmHitCore = edmHit.core();
    auto position = fcc::Point();
    position.x = i;
    position.y = i;
    position.z = i;
    phitscoll->create(position, edmHitCore);
  }

  m_positionedTrackHits.put(phitscoll);

  return StatusCode::SUCCESS;
}

StatusCode OccupancyMap::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
