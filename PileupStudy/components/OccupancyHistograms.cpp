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
  m_geoSvc = service("GeoSvc");
  auto lcdd = m_geoSvc->lcdd();
  m_volMan = lcdd->volumeManager();
  info() << "Counting physical volumes ... " << endmsg;
  auto highest_vol = gGeoManager->GetTopVolume();
  int nodeCount = 0;
  info() << highest_vol->GetName() << endmsg;
  TGeoIterator next(highest_vol);
  TGeoNode* node;
  TString tpath;
  std::string delimiter = "_";
  int layerIndexOffset;
  auto readoutEndcap = lcdd->readout("TrackerEndcapReadout");
  auto segmentationEndcap = static_cast<DD4hep::DDSegmentation::CartesianGridXYZ*>(readoutEndcap.segmentation().segmentation());
  auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
  auto segmentationBarrel = static_cast<DD4hep::DDSegmentation::CartesianGridXYZ*>(readoutBarrel.segmentation().segmentation());
  float segmentationCellVolume; //
  int layerIndex;
  while ((node=next())) {
    std::string currentNodeName = node->GetName();
    next.GetPath(tpath);
    std::string path(tpath.Data());
    //info() << currentNodeName << endmsg;
    //info() << path << endmsg;
    if (currentNodeName.find("layer") != std::string::npos) {
      if (path.find("BarrelInner") != std::string::npos) {
        layerIndexOffset = 0;

      } else if (path.find("BarrelOuter") != std::string::npos) {
        layerIndexOffset = 100;
      } else if (path.find("EndcapInner_0") != std::string::npos) {
        layerIndexOffset = 200;
      } else if (path.find("EndcapInner_1") != std::string::npos) {
        layerIndexOffset = 300;
      } else if (path.find("EndcapOuter_0") != std::string::npos) {
        layerIndexOffset = 400;
      } else if (path.find("EndcapOuter_1") != std::string::npos) {
        layerIndexOffset = 500;
      }
      info() << path << "\t" << layerIndexOffset << endmsg;
      std::string subs =  currentNodeName.substr(currentNodeName.find(delimiter) + 1, currentNodeName.size());
      std::stringstream convert(subs);
      convert >> layerIndex;
      layerIndex += layerIndexOffset;
      //info() << subs << "\t " << layerIndex <<endmsg;
      std::string histname = "DetEta" + std::to_string(layerIndex);
      m_detEta = new TH1F(histname.c_str(), "Tracker Module Pseudorapidity", 50, -6, 6);
      if (m_ths->regHist("/rec/DetEta/" + path , m_detEta).isFailure()) {
           error() << "Couldn't register DetEta" << endmsg;
           info() << path << endmsg; 
      }
      m_detEtas[layerIndex] = m_detEta;
      histname = "HitEta" + std::to_string(layerIndex);
      m_hitEta = new TH1F(histname.c_str(), "Tracker Module Pseudorapidity", 50, -6, 6);
      if (m_ths->regHist("/rec/HitEta/" + path, m_hitEta).isFailure()) {
           error() << "Couldn't register HitEta" << endmsg;
      }
      m_hitEtas[layerIndex] = m_hitEta;
    } else if (currentNodeName.find(m_moduleNodeName) != std::string::npos) {
      const TGeoMatrix* mat = next.GetCurrentMatrix();
      TVector3 pos(0, 0, 0);
      // loop over all signs
      float cornerEtasMin = 10;
      float cornerEtasMax = -10;
      float shapeVolume = 0;
      int numCells;
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
        shapeVolume = 4 * shape->GetDz() * shape->GetDY() * (shape->GetDx1() + shape->GetDx2());
      if (layerIndexOffset < 200) {
        info() << shape->GetDX() << "\t" << segmentationBarrel->gridSizeX() << endmsg;
        segmentationCellVolume = 100*  segmentationBarrel->gridSizeX() * segmentationBarrel->gridSizeZ() * shape->GetDY();


      } else {
        segmentationCellVolume = 100*  segmentationEndcap->gridSizeX() * segmentationEndcap->gridSizeZ() * shape->GetDY();
      }
        numCells = shapeVolume / segmentationCellVolume;
        debug() << ", segCellVol " << segmentationCellVolume << ", numCells " << numCells << endmsg;
      } else {
      TGeoBBox* shape = static_cast<TGeoBBox*>(node->GetVolume()->GetShape());
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
        shapeVolume = 8 * shape->GetDX() * shape->GetDY() * shape->GetDZ();
      if (layerIndexOffset < 200) {
        info() << shape->GetDZ() << "\t" << segmentationBarrel->gridSizeZ() << endmsg;
        segmentationCellVolume = 100 * segmentationBarrel->gridSizeX() * segmentationBarrel->gridSizeZ() * 2 * shape->GetDY();


      } else {
        segmentationCellVolume = 100 * segmentationEndcap->gridSizeX() * segmentationEndcap->gridSizeZ() * 2 * shape->GetDY();
      }
        numCells = shapeVolume / segmentationCellVolume;
      }
      // TODO: get real value
        info() << ", segCellVol " << segmentationCellVolume << ", numCells " << numCells << endmsg;
      float dEta = (cornerEtasMax - cornerEtasMin ) / static_cast<float>(numCells);
      m_detEta = m_detEtas[layerIndex];
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
  auto m_decoderEndcap = readoutEndcap.idSpec().decoder();

  const fcc::TrackClusterHitsAssociationCollection* trkhitclusterassociations = m_trkHitClusterAssociations.get();
  debug() << "filling histograms with " << trkhitclusterassociations->size() << " hits " << endmsg;
  int layerIndexOffset;
  for (const auto& hitcluster : *trkhitclusterassociations) {
    auto hit = hitcluster.Hit();
    auto cluster = hitcluster.Cluster();
    auto aCellId = hit.Core().Cellid;
    DD4hep::Geometry::Position pos;
    m_decoderBarrel->setValue(aCellId);
    int system_id = (*m_decoderBarrel)["system"];
    int layer_id;
    if (system_id == 10) {
      pos  = segmentationBarrel->position(aCellId);
      layerIndexOffset = 0;
      layer_id = (*m_decoderBarrel)["layer"];
    } else if ( system_id == 12 ) {
       layerIndexOffset = 100;
      pos  = segmentationBarrel->position(aCellId);
      layer_id = (*m_decoderBarrel)["layer"];
    } else if (system_id == 11) {
    m_decoderEndcap->setValue(aCellId);
      layerIndexOffset = 200 +(*m_decoderEndcap)["posneg"] * 100;
      layer_id = (*m_decoderEndcap)["layer"];
      
      pos  = segmentationEndcap->position(aCellId);
    } else if (system_id == 13) {
    m_decoderEndcap->setValue(aCellId);
      layerIndexOffset = 400 + (*m_decoderEndcap)["posneg"] * 100;
      pos  = segmentationEndcap->position(aCellId);
      layer_id = (*m_decoderEndcap)["layer"];
    }
    
    --layer_id; //TODO: understand
    auto cpos = cluster.Core().position;
    TVector3 finalPosition (cpos.X, cpos.Y, cpos.Z); 
    debug() << "CellId hit position: \t" << finalPosition.X() << "\t" << finalPosition.Y() << "\t" << finalPosition.Z() << endmsg;
    debug() << "Cluster position \t" << cpos.X << "\t" << cpos.Y << "\t" << cpos.Z  <<endmsg;
    const bool notYetOccupied = m_occupiedCells.find(aCellId) == m_occupiedCells.end();
    if (notYetOccupied) {
      m_hitEtas[layerIndexOffset + layer_id ]->Fill(finalPosition.Eta());
      m_occupiedCells.insert(aCellId);
    } else {
        ++m_multipleOccupancy;
      }
    }
  
  return StatusCode::SUCCESS;
}

StatusCode OccupancyHistograms::finalize() {
  m_hitEta->GetXaxis()->SetTitle("Eta");
  m_hitEta->GetYaxis()->SetTitle("Hits");
  m_detEta->GetXaxis()->SetTitle("Eta");
  m_detEta->GetYaxis()->SetTitle("Det. Cells");
  info() << m_multipleOccupancy << " hits not counted because their cell was already occupied" << endmsg;
  return GaudiAlgorithm::finalize();
}
