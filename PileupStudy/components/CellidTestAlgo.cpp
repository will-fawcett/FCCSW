#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"
#include "DetInterface/IGeoSvc.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackClusterHitsAssociationCollection.h"
#include "datamodel/GenVertex.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDRec/API/IDDecoder.h"


#include "TGeoManager.h"
#include "TVector3.h"



void countNodes(TGeoNode* node, const std::string nodeName, int& nodeCount) {
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
    if (next_node) countNodes(next_node, nodeName, nodeCount);
  }
}

class CellidTestAlgo : public GaudiAlgorithm {

public:
  CellidTestAlgo(const std::string& name, ISvcLocator* svcLoc) :
    GaudiAlgorithm(name, svcLoc)
  {
    declareInput("hits", m_trkHits, "hits");
    declareInput("clusters", m_trkClusters, "clusters");
    declareInput("hitClusterAssociation", m_trkHitClusterAssociations, "hitClusterAssociation");
  }

  ~CellidTestAlgo() {};

  StatusCode initialize() {
    m_geoSvc = service ("GeoSvc");
    return GaudiAlgorithm::initialize();
  }

  StatusCode execute() {
    auto lcdd = m_geoSvc->lcdd();
    DD4hep::Geometry::VolumeManager volman = lcdd->volumeManager();
    DD4hep::DDRec::IDDecoder& iddec = DD4hep::DDRec::IDDecoder::getInstance();
    auto allReadouts = lcdd->readouts();
    auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
    auto readoutEndcap = lcdd->readout("TrackerEndcapReadout");
    auto segmentationBarrel = readoutBarrel.segmentation();
    auto m_decoderBarrel = segmentationBarrel->decoder();
    info() << "Barrel segmentation of type " << segmentationBarrel->type() << endmsg;
    auto m_decoderEndcap = readoutEndcap.idSpec().decoder();
    auto segmentationEndcap = readoutEndcap.segmentation().segmentation();
    const fcc::TrackHitCollection* trkhits = m_trkHits.get();
    const fcc::TrackClusterCollection* trkclusters = m_trkClusters.get();
    const fcc::TrackClusterHitsAssociationCollection* trkhitclusterassociations = m_trkHitClusterAssociations.get();
    unsigned long long aCellId;

    info() << "hits size: " << trkhits->size() << endmsg;
    int cntr = 0;
    
    for (const auto& hitcluster : *trkhitclusterassociations) {
      auto hit = hitcluster.Hit();
      auto cluster = hitcluster.Cluster();
      aCellId = hit.Core().Cellid;

      auto cpos = cluster.Core().position;

      m_decoderBarrel->setValue(aCellId);
      int system_id = (*m_decoderBarrel)["system"];
      if (system_id == 10 || system_id == 12 ) {
        int layer_id = (*m_decoderBarrel)["layer"];
        int rod_id = (*m_decoderBarrel)["rod"];
        int module_id = (*m_decoderBarrel)["module"];
        int x_id= (*m_decoderBarrel)["x"];
        int y_id = (*m_decoderBarrel)["y"];
        //int z_id = (*m_decoderBarrel)["z"];
        //int z_id = (*m_decoderEndcap)["z"];
        int z_id = 0;
        //int layer_id = -1;
        info() << "hit cellid: " << aCellId << " in system " << system_id << ", layer " << layer_id << ", rod " << rod_id << ", module " << module_id << "  x " << x_id << " y  " << y_id << " z " << z_id << endmsg;
        auto pos = segmentationBarrel->position(aCellId);
        info() << "\t has position " << pos.X << "\t" << pos.Y << "\t" << pos.Z  <<endmsg;
        auto pvolb = volman.lookupPlacement(aCellId);
        auto volb = pvolb.volume();
        auto solidb = volb.solid();
        info() << "\t has shape " << solidb.name()  <<endmsg;
        TGeoTrd2* trapezoid = static_cast<TGeoTrd2*>(solidb.ptr());
        info() << "\t has DZ " << trapezoid->GetDZ() << endmsg;
        TGeoHMatrix trafo = volman.worldTransformation(aCellId);
        Double_t* translation = trafo.GetTranslation();
        info() << "trafo: " << " \t" << translation[0] << "\t" << translation[1] << "\t" << translation[2] << endmsg;
        info() << "\t has cluster position \t" << cpos.X << "\t" << cpos.Y << "\t" << cpos.Z  <<endmsg;
        auto pvol = volman.lookupPlacement(aCellId);
        TVector3 vec(pos.x(),pos.y(),pos.z());
        vec = (*trafo.GetRotationMatrix()) * vec;
        info() << "\t has cluster position \t" << 10* (translation[0] + vec.x()) << "\t" << 10* (translation[1] + vec.y()) << "\t" << 10* (translation[2] + vec.z())  <<endmsg;
        TVector3 glob(10*(translation[0] + vec.x()), 10*(translation[1] + vec.y()), 10*(translation[2] + vec.z()));
        info() << "position eta " << glob.PseudoRapidity() << endmsg;
        //info() << "\t has position " << pos.x() << "\t" << pos.y() << "\t" << pos.z()  <<endmsg;
      } else if (system_id == 11 || system_id == 13) {
        m_decoderEndcap->setValue(aCellId);
        int layer_id = (*m_decoderEndcap)["layer"];
        int petal_id = (*m_decoderEndcap)["petal"];
        int x_id= (*m_decoderEndcap)["x"];
        int y_id = (*m_decoderEndcap)["y"];
        //int z_id = (*m_decoderEndcap)["z"];
        int z_id = 0;
        info() << "hit cellid: " << aCellId << " in system " << system_id << ", layer " << layer_id << ", petal " << petal_id << " x " << x_id << " y " << y_id << " z " << z_id  << endmsg;
        auto volid = aCellId;//iddec.volumeID(aCellId);
        info() << "hit volid:  " << volid << endmsg;
        auto pos = segmentationEndcap->position(aCellId);
        TGeoHMatrix trafo = volman.worldTransformation(aCellId);
        double local[3] = {pos.x(), pos.y(), pos.z()};
        double master[3];
        //pos->GetCoordinates(local);
        trafo.LocalToMaster(local, master);
        //Double_t* translation = trafo.GetTranslation();
        //auto pvol = volman.lookupPlacement(volid);
        TVector3 vec(master[0], master[1], master[2]);
        //vec = (*trafo.GetRotationMatrix()) * vec;
        //info() << "\t module translation: " << " \t" << translation[0] << "\t" << translation[1] << "\t" << translation[2] << endmsg;
        info() << "\t cellid position: \t" << vec.x() << "\t" << vec.y() << "\t" << vec.z()  <<endmsg;
        info() << "\t has cluster position: \t" << cpos.X << "\t" << cpos.Y << "\t" << cpos.Z  <<endmsg;
        //info() << "\t final cellid position: \t" << 10* (translation[0] + vec.x()) << "\t" << 10* (translation[1] + vec.y()) << "\t" << 10* (translation[2] + vec.z())  <<endmsg;
        //auto vol = pvol.volume();
        //auto solid = vol.solid();
        //TGeoTrd2* trapezoid = static_cast<TGeoTrd2*>(solid.ptr());
        //info() << "\t has dx1 " << trapezoid->GetDZ() << endmsg;
        //info() << "\t has shape " << solid.name()  <<endmsg;
      }
    }
    
    // counting physical volumes
    info() << "Count physical volumes ... " << endmsg;
    auto highest_vol = gGeoManager->GetTopVolume();
    auto  daughters = highest_vol->GetNodes();
    int nodeCount = 0;
    // the sensitive part of the module has the material string in its name
    std::string nodeName = "layer";
    TIter iObj(daughters);
    info() << highest_vol->GetName() << endmsg;
    while (TObject* obj = iObj()) {
      // dynamic_cast to a node
      TGeoNode* node = dynamic_cast<TGeoNode*>(obj);
      info() << "\t  ... in " << node->GetName() << endmsg;
      if (node) countNodes(node, nodeName, nodeCount);
    }
    info() << nodeCount << " modules found!" << endmsg;


    return StatusCode::SUCCESS;
  }

  StatusCode finalize() {
    return GaudiAlgorithm::finalize();
  }

private:
  DataHandle<fcc::TrackHitCollection> m_trkHits;
  DataHandle<fcc::TrackClusterCollection> m_trkClusters;
  DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trkHitClusterAssociations;
  SmartIF<IGeoSvc> m_geoSvc;
};
DECLARE_COMPONENT(CellidTestAlgo)
