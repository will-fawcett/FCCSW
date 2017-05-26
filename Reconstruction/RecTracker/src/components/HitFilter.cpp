
#include "DetInterface/ITrackingGeoSvc.h"
#include "RecInterface/ITrkVolumeManagerSvc.h"
#include "DetInterface/IGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/ISaveTrackStateTool.h"



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

#include "HitFilter.h"



DECLARE_ALGORITHM_FACTORY(HitFilter)

HitFilter::HitFilter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("filteredTrackHits", m_filteredTrackHits, "Tracker hits (Input)");

  }

HitFilter::~HitFilter() {}

StatusCode HitFilter::initialize() {
  info() << "initialize" << endmsg;

  m_geoSvc = service ("GeoSvc");

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

StatusCode HitFilter::execute() {

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
    auto edmPositions = m_filteredTrackHits.createAndPut();

for (auto hit: *hits) {
    // loop over hits belonging to one track by checking that their ids match
    // check ids of the next hit, except for the last hit check for the previous hit

    int hitcounter = 0;
      long long int theCellId = hit.core().cellId;
      debug() << theCellId << endmsg;
      debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
      debug() << "phi: " << std::atan2(hit.position().y, hit.position().x) << endmsg;
      m_decoderBarrel->setValue(theCellId);
      int system_id = (*m_decoderBarrel)["system"];
      int system_id2 = theCellId & 31;
      if (system_id != system_id2) {
        return StatusCode::FAILURE;
      }

      if (system_id ==0) {
        edmPositions->create(hit.position(), hit.core());
      }
      debug() << " hit in system: " << system_id;
      int layer_id = (*m_decoderBarrel)["layer"];
      debug() << "\t layer " << layer_id;
      int module_id = (*m_decoderBarrel)["module"];
      debug() << "\t module " << module_id;
      debug() << endmsg;
      // The conventions in DD4hep and ACTS for the local coordinates differ,
      // and the conversion needs to reflect this. See the detector factories for details.
      (*m_decoderBarrel)["x"] = 0; // workaround for broken `volumeID` method --
      (*m_decoderBarrel)["z"] = 0; // set everything not connected with the VolumeID to zero,
      // so the cellID can be used to look up the tracking surface
      }

  return StatusCode::SUCCESS;
}

StatusCode HitFilter::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
