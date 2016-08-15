#include "OccupancyHistograms.h"

#include "TGeoManager.h"
#include "TVector3.h"
#include "TH1F.h"


#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDRec/API/IDDecoder.h"

#include "GaudiKernel/ITHistSvc.h"


#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackClusterHitsAssociationCollection.h"

DECLARE_COMPONENT(OccupancyHistograms)


OccupancyHistograms::OccupancyHistograms(const std::string& aName, ISvcLocator* aSL):
  GaudiAlgorithm(aName, aSL),
  m_ths(nullptr),
  m_occupancy(nullptr),
  m_detEta(nullptr),
  m_hitEta(nullptr),
  m_multipleOccupancy(0) {
  declareInput("hits", m_trkHits, "hits");
  declareInput("clusters", m_trkClusters, "clusters");
  declareInput("hitClusterAssociation", m_trkHitClusterAssociations, "hitClusterAssociation");
  declareProperty("moduleNodeName", m_moduleNodeName="Silicon");
}

StatusCode OccupancyHistograms::initialize() {
  if (service("THistSvc", m_ths).isFailure()) {
    error() << "Couldn't get THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_detEta = new TH1F("DetEta", "Tracker Module Pseudorapidity", 50, -6, 6);
  if (m_ths->regHist("/rec/DetEta", m_detEta).isFailure()) {
    error() << "Couldn't register DetEta" << endmsg;
  }
  m_hitEta = new TH1F("HitEta", "Tracker Hit Pseudorapidity", 50, -6, 6);
  if (m_ths->regHist("/rec/HitEta", m_hitEta).isFailure()) {
    error() << "Couldn't register HitEta" << endmsg;
  }
  m_geoSvc = service("GeoSvc");
  auto lcdd = m_geoSvc->lcdd();
  m_volMan = lcdd->volumeManager();
  info() << "Counting physical volumes ... " << endmsg;
  auto highest_vol = gGeoManager->GetTopVolume();
  int nodeCount = 0;
  info() << highest_vol->GetName() << endmsg;
  TGeoIterator next(highest_vol);
  TGeoNode* node;
  TString path;
  while ((node=next())) {
    std::string currentNodeName = node->GetName();
    if (currentNodeName.find(m_moduleNodeName) != std::string::npos) {
      const TGeoMatrix* mat = next.GetCurrentMatrix();
      TVector3 pos(0, 0, 0);
      // loop over all signs
      float cornerEtasMin = 10;
      float cornerEtasMax = -10;
      std::array<int, 2> signs = {-1, 1};
      std::string shapename = node->GetVolume()->GetShape()->GetName();
      if (shapename == "TGeoTrd2" ) {
        TGeoTrd2* shape = static_cast<TGeoTrd2*>(node->GetVolume()->GetShape());
        // loop over all the corners
        for (int signx: signs) {
          for (int signy: signs) {
            for (int signz: signs) {
              // check which side of the trapezoid the current corner is on
              double x, y;
              if (1 == signz) {
                x = shape->GetDx2();
                y = shape->GetDy2();
              } else {
                x = shape->GetDx1();
                y = shape->GetDy1();
              }
              const Double_t local[3] = {signx * x, signy * y, signz * shape->GetDz()};
              Double_t master[3] = {0, 0, 0};
              mat->LocalToMaster(local, master);
              pos.SetX(master[0]);
              pos.SetY(master[1]);
              pos.SetZ(master[2]);
              float currentEta = pos.Eta();
              cornerEtasMax = std::max(cornerEtasMax, currentEta);
              cornerEtasMin = std::min(cornerEtasMin, currentEta);
            }
          }
        }
      } else {
      TGeoBBox* shape = static_cast<TGeoBBox*>(node->GetVolume()->GetShape());
        debug() << node->GetVolume()->GetShape()->GetName() << "\t" << node->GetVolume()->GetShape()->GetId() << endmsg; 
        next.GetPath(path);
        debug() << path << endmsg;
        // loop over all the corners
        for (int signx: signs) {
          for (int signy: signs) {
            for (int signz: signs) {
              const Double_t local[3] = {signx * shape->GetDX(), signy * shape->GetDY(), signz * shape->GetDZ()};
              Double_t master[3] = {0, 0, 0};
              mat->LocalToMaster(local, master);
              pos.SetX(master[0]);
              pos.SetY(master[1]);
              pos.SetZ(master[2]);
              float currentEta = pos.Eta();
              cornerEtasMax = std::max(cornerEtasMax, currentEta);
              cornerEtasMin = std::min(cornerEtasMin, currentEta);
            }
          }
        }
      }
      // TODO: get real value
      int numCells = 1000;
      float dEta = (cornerEtasMax - cornerEtasMin ) / static_cast<float>(numCells);
      for (int cellIndex = 0; cellIndex < numCells; ++cellIndex) {
        m_detEta->Fill(cornerEtasMin + cellIndex * dEta);
      }
      ++nodeCount;
    }
  }
  info() << nodeCount << " modules containing string " << m_moduleNodeName << " found!" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode OccupancyHistograms::execute() {
  auto lcdd = m_geoSvc->lcdd();
  m_volMan = lcdd->volumeManager();

  auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
  auto segmentationBarrel = readoutBarrel.segmentation();
  auto m_decoderBarrel = readoutBarrel.idSpec().decoder();

  auto readoutEndcap = lcdd->readout("TrackerEndcapReadout");
  auto segmentationEndcap = readoutEndcap.segmentation().segmentation();

  const fcc::TrackClusterHitsAssociationCollection* trkhitclusterassociations = m_trkHitClusterAssociations.get();
  debug() << "filling histograms with " << trkhitclusterassociations->size() << " hits " << endmsg;
  for (const auto& hitcluster : *trkhitclusterassociations) {
    auto hit = hitcluster.Hit();
    auto cluster = hitcluster.Cluster();
    auto aCellId = hit.Core().Cellid;
    DD4hep::Geometry::Position pos;
    m_decoderBarrel->setValue(aCellId);
    int system_id = (*m_decoderBarrel)["system"];
    if (system_id == 10 || system_id == 12 ) {
      pos  = segmentationBarrel->position(aCellId);
    } else if (system_id == 11 || system_id == 13) {
      pos  = segmentationEndcap->position(aCellId);
    }
    auto cpos = cluster.Core().position;
    TVector3 finalPosition (cpos.X, cpos.Y, cpos.Z); 
    debug() << "CellId hit position: \t" << finalPosition.X() << "\t" << finalPosition.Y() << "\t" << finalPosition.Z() << endmsg;
    debug() << "Cluster position \t" << cpos.X << "\t" << cpos.Y << "\t" << cpos.Z  <<endmsg;
    const bool notYetOccupied = m_occupiedCells.find(aCellId) == m_occupiedCells.end();
    if (notYetOccupied) {
      m_hitEta->Fill(finalPosition.Eta());
      m_occupiedCells.insert(aCellId);
    } else {
        ++m_multipleOccupancy;
      }
    }
  
  return StatusCode::SUCCESS;
}

StatusCode OccupancyHistograms::finalize() {
  info() << m_multipleOccupancy << " hits not counted because their cell was already occupied" << endmsg;
  return GaudiAlgorithm::finalize();
}
