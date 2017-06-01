
#include "DetInterface/IGeoSvc.h"

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
    if (d.first.find("module") != std::string::npos) {
    auto solid = d.second.volume().solid();
    // get the envelope of the shape
    TGeoBBox* box = (dynamic_cast<TGeoBBox*>(solid.ptr()));
    // get half-widths
    //return CLHEP::Hep3Vector(box->GetDX(), box->GetDY(), box->GetDZ());
    //debug() << "Bounding box dimensions: " << box->GetDX() << "\t" << box->GetDY() << "\t" << box->GetDZ() << endmsg;
    //debug() << "Segmentation size: " <<   m_segGridSizeX  << "\t" << m_segGridSizeZ  << endmsg;
    // default dd4hep units differ from fcc ones
    int cellsX = ceil((box->GetDX()  - m_segGridSizeX  / 2.) / m_segGridSizeX)  ;
    int cellsZ = ceil((box->GetDZ()  - m_segGridSizeZ  / 2.) / m_segGridSizeZ) ;
    //debug() << "number of cells: "  << cellsX << "\t" << cellsX *  m_segGridSizeX  << endmsg;
    for (int l_x = -1*cellsX; l_x < cellsX; ++l_x) {
      for (int l_z = -1*cellsZ; l_z < cellsZ; ++l_z) {
        m_globalPos.fill(0);
        m_localPos.fill(0);
        m_localPos[0] = l_x * m_segGridSizeX; // / static_cast<double>(m_segZoomFactor) * 5;
        m_localPos[2] = l_z * m_segGridSizeZ; // / static_cast<double>(m_segZoomFactor) * 5;
    d.second.worldTransformation().LocalToMaster(m_localPos.data(), m_globalPos.data());
    auto position = fcc::Point();
    position.x = m_globalPos[0] * CM_2_MM ;
    position.y = m_globalPos[1] * CM_2_MM ;
    position.z = m_globalPos[2] * CM_2_MM;
    fcc::TrackHit edmHit = m_hitscoll->create();
    fcc::BareHit& edmHitCore = edmHit.core();
    m_phitscoll->create(position, edmHitCore);
      }
    }




    }
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
  m_segGridSizeX = m_segZoomFactor* segmentationXZ->gridSizeX();
  m_segGridSizeZ = m_segZoomFactor* segmentationXZ->gridSizeZ();
  return sc;
}

StatusCode OccupancyMap::execute() {
   m_phitscoll = new fcc::PositionedTrackHitCollection();
   m_hitscoll = new fcc::TrackHitCollection();


  auto lcdd = m_geoSvc->lcdd();
  visitDetElement(lcdd->world());





  m_positionedTrackHits.put(m_phitscoll);

  return StatusCode::SUCCESS;
}

StatusCode OccupancyMap::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
