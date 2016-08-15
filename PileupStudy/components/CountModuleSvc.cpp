#include "CountModuleSvc.h"

#include "TGeoManager.h"
#include "TVector3.h"
#include "TH1F.h"

#include "DetInterface/IGeoSvc.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXYZ.h"

#include "GaudiKernel/ITHistSvc.h"


void countNode2(TGeoNode* node, const std::string nodeName, int& nodeCount) {
  std::string currentNodeName = node->GetName();
    node->ls();
  if (currentNodeName.find(nodeName) != std::string::npos) {
    ++nodeCount;
    node->ls();
  }
  auto daughters = node->GetNodes();
  TIter iObj(daughters);
  while (TObject* obj = iObj()) {
    TGeoNode* next_node = dynamic_cast<TGeoNode*>(obj);
    if (next_node) countNode2(next_node, nodeName, nodeCount);
  }
}



DECLARE_SERVICE_FACTORY(CountModuleSvc)


CountModuleSvc::CountModuleSvc(const std::string& aName, ISvcLocator* aSL):
  base_class(aName, aSL) {
}

StatusCode CountModuleSvc::initialize() {
  if (Service::initialize().isFailure()){
    error()<<"Unable to initialize Service()"<<endmsg;
    return StatusCode::FAILURE;
  }

  if (service("THistSvc", m_ths).isFailure()) {
    error() << "Couldn't get THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  SmartIF<IGeoSvc> geoSvc = static_cast<SmartIF<IGeoSvc>> (service("GeoSvc"));
  auto lcdd = geoSvc->lcdd();
  DD4hep::Geometry::VolumeManager volman = lcdd->volumeManager();
  auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
  auto segmentationBarrel = readoutBarrel.segmentation();
  auto decoderBarrel = segmentationBarrel->decoder();
  //  for (int ix=0; ix < 1000; ix++) {
  //    (*decoderBarrel)["x"] = ix;
  //    for (int iy=0; iy < 1000; iy++) {
  //      (*decoderBarrel)["y"] = iy;
  //      info() << "x: seg " << (*decoderBarrel)["x"] << endmsg;
  //      unsigned long long int aCellId = (*decoderBarrel).getValue();
  //      auto pos = segmentationBarrel->position(aCellId);
  //      info() << "\t has position " << pos.X << "\t" << pos.Y << "\t" << pos.Z  <<endmsg;
  //    }
  //  }
  info() << "Counting physical volumes ... " << endmsg;
  auto highest_vol = gGeoManager->GetTopVolume();
  int nodeCount = 0;
  // the sensitive part of the module has the material string in its name
  std::string nodeName = "Silicon"; /// move to config
  info() << highest_vol->GetName() << endmsg;
  TGeoIterator next(highest_vol);
  TGeoNode* node;
  TH1F* etahist = new TH1F("DetEta", "Tracker Module Pseudorapidity", 100, -10, 10);
  if (m_ths->regHist("/rec/DetEta", etahist).isFailure()) {
    error() << "Couldn't register DetEta" << endmsg;
  }
  while ((node=next())) {
    std::string currentNodeName = node->GetName();
    if (currentNodeName.find(nodeName) != std::string::npos) {
      const TGeoMatrix* mat = next.GetCurrentMatrix();
      const Double_t* trans = mat->GetTranslation();
      const Double_t* rot = mat->GetRotationMatrix();
      //TVector3 pos(trans[0], trans[1], trans[2]);
      TVector3 pos(0, 0, 0);
      auto box = static_cast<TGeoBBox*>(node->GetVolume()->GetShape());
      auto seg = static_cast<DD4hep::DDSegmentation::CartesianGridXYZ*>(segmentationBarrel.segmentation());
      //for (double cx = -1* box->GetDX(); cx < box->GetDX(); cx += seg->gridSizeX()) { 
      //for (double cy = -1* box->GetDY(); cy < box->GetDY(); cy += seg->gridSizeY()) { 
      //for (double cz = -1* box->GetDZ(); cz < box->GetDZ(); cz += seg->gridSizeZ()) { 
      double cx = 0;
      double cy = 0;
      double cz = 0;
      //for (double cx = -1* box->GetDX(); cx <= box->GetDX(); cx += box->GetDX()) { 
      //for (double cy = -1* box->GetDY(); cy <= box->GetDY(); cy += box->GetDY()) { 
      //for (double cz = -1* box->GetDZ(); cz <= box->GetDZ(); cz += box->GetDZ()) { 
      const Double_t xyz[3] = {cx, cy, cz};
      Double_t master[3] = {0,0,0};
      mat->LocalToMaster(xyz, master);
      pos.SetX(master[0]);
      pos.SetY(master[1]);
      pos.SetZ(master[2]);
     // info() << "Current trafo: \t" << pos.X() << "\t" << pos.Y() << "\t" << pos.Z() << endmsg;
     // info() << "Current translation: \t" << trans[0] << "\t" << trans[1] << "\t" <<trans[2] << endmsg;
     // info() << "Current eta: \t" << pos.Eta() << endmsg;
     etahist->Fill(pos.Eta());
     // }
     // }
     // }


      info() << "Barrel segmentation of type " << segmentationBarrel->type() << endmsg;
      info() << "Barrel segmentation of type " << seg->gridSizeX() << "\t" << seg->gridSizeY() << "\t" << seg->gridSizeZ()  << endmsg;

      

      //info() << currentNodeName << endmsg;
      ++nodeCount;
    }

  }

  info() << nodeCount << " modules found!" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CountModuleSvc::finalize() {
  return Service::finalize();
}
