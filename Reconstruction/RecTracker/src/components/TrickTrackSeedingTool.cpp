#include "TrickTrackSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "datamodelExt/HitTripletCollection.h"
#include "datamodelExt/HitQuadrupletCollection.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

// TrickTrack headers
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"

using Hit = tricktrack::SpacePoint<size_t>; 
using namespace tricktrack;



DECLARE_TOOL_FACTORY(TrickTrackSeedingTool)

TrickTrackSeedingTool::TrickTrackSeedingTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent),
      m_geoSvc("GeoSvc", "TrickTrackSeedingTool") {
  declareInterface<ITrackSeedingTool>(this);
  declareProperty("trackSeeds", m_trackSeeds, "Track Seeds (Output)");
  declareProperty("LayerGraphTool", m_layerGraphTool);
  declareProperty("DoubletCreationTool", m_doubletCreationTool);
  declarePrivateTool(m_layerGraphTool, "BarrelLayerGraphTool/LayerGraphTool");
  declarePrivateTool(m_doubletCreationTool, "DoubletCreationTool/DoubletCreationTool");

}

StatusCode TrickTrackSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto readout = lcdd->readout(m_readoutName);
  m_decoder = readout.idSpec().decoder();

  // get layer graph from tool and create automaton
  m_layerGraph = m_layerGraphTool->getGraph();

  debug() << "Create automaton ..." << endmsg;
  m_automaton = std::make_unique<HitChainMaker<Hit>>(m_layerGraph);
  m_trackingRegion = std::make_unique<tricktrack::TrackingRegion>(m_regionOriginX, m_regionOriginY, m_regionOriginRadius, m_ptMin);
  return sc;
}

void TrickTrackSeedingTool::createBarrelSpacePoints(std::vector<Hit>& thePoints,
                                                    std::map<int, unsigned long int>& indexToTrackId,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex,
                                                       int trackCutoff) {
  size_t hitCounter = 0;
  for (auto hit : *theHits) {
    m_decoder->setValue(hit.core().cellId);
    if ((*m_decoder)["system"] == sIndex.first) {
      if ((*m_decoder)["layer"] == sIndex.second) {
        //debug() << "trackId: " << hit.core().bits << endmsg;
        if(hit.core().bits < trackCutoff + 1 ) {
        thePoints.emplace_back(hit.position().x, hit.position().y, hit.position().z, hitCounter);
        debug() << "hitCounter: " << hitCounter << endmsg;

        indexToTrackId.insert(std::pair<int, unsigned long int>(hitCounter, hit.core().bits));
        debug() << hitCounter << "\t" <<  hit.core().bits << endmsg;
        ++hitCounter;
        }
      }
    }
  }
}

std::multimap<unsigned int, unsigned int>
TrickTrackSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {

  fccextedm::HitTripletCollection* trackSeedCollection = m_trackSeeds.createAndPut();
  std::multimap<unsigned int, unsigned int> theSeeds;

  for (int trackCutoff = 1; trackCutoff < 30; ++trackCutoff) {

  std::vector<Hit> pointsLayer1;
  std::vector<Hit> pointsLayer2;
  std::vector<Hit> pointsLayer3;

  std::map<int, unsigned long int> mapLayer1;
  std::map<int, unsigned long int> mapLayer2;
  std::map<int, unsigned long int> mapLayer3;

  createBarrelSpacePoints(pointsLayer1, mapLayer1, theHits, m_seedingLayerIndices0, trackCutoff);
  createBarrelSpacePoints(pointsLayer2, mapLayer2, theHits, m_seedingLayerIndices1, trackCutoff);
  createBarrelSpacePoints(pointsLayer3, mapLayer3, theHits, m_seedingLayerIndices2, trackCutoff);

  debug() << "found " << pointsLayer1.size() << " points on Layer 1" << endmsg;
  debug() << "found " << pointsLayer2.size() << " points on Layer 2" << endmsg;
  debug() << "found " << pointsLayer3.size() << " points on Layer 3" << endmsg;

  std::vector<HitDoublets<Hit>*> doublets;
  std::vector<Hit> pointsLayer2b = pointsLayer2;
  

  auto doublet1 = m_doubletCreationTool->findDoublets(pointsLayer1, pointsLayer2);
  auto doublet2 = m_doubletCreationTool->findDoublets(pointsLayer2b, pointsLayer3);

  doublets.push_back(doublet1);
  doublets.push_back(doublet2);


  debug() << "found "  << doublet1->size() << " doublets on the first layer "  << endmsg;
  debug() << "found "  << doublet2->size() << " doublets on the second layer "  << endmsg;

  debug() << "Create and connect cells ..." << endmsg;
  m_automaton->createAndConnectCells(doublets, *m_trackingRegion, m_thetaCut, m_phiCut, m_hardPtCut);
  debug() << "... cells connected and created." << endmsg;

  m_automaton->evolve(3);
  std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
  m_automaton->findNtuplets(foundTracklets, 3);

  debug() << "found " << foundTracklets.size()<< " tracklets" << endmsg;
  auto cells = m_automaton->getAllCells();
  int numGoodTracklets = 0;

  unsigned int trackCounter = 0;
  for (const auto& tracklet : foundTracklets) {
    auto l_id1 = cells[tracklet[0]].getInnerHit().identifier();
    auto l_id2 = cells[tracklet[1]].getInnerHit().identifier();
    auto l_id3 = cells[tracklet[1]].getOuterHit().identifier();
    debug() << "New Tracklet: " << endmsg;
    
    
    debug() << tracklet[0]<< "\t" << l_id1 << "\t" << mapLayer1[l_id1] << endmsg;
    debug() << tracklet[1]<< "\t" << l_id2 << "\t" << mapLayer2[l_id2] << endmsg;
    debug() << tracklet[2]<< "\t" << l_id3 << "\t" << mapLayer3[l_id3] << endmsg;

    if ((mapLayer1[l_id1] == mapLayer2[l_id2]) && 
        (mapLayer3[l_id3] == mapLayer2[l_id2])) {
      ++numGoodTracklets;
    }
    trackSeedCollection->create(l_id1, l_id2, l_id3);
    for (const auto& id : tracklet) {
      theSeeds.insert(std::pair<unsigned int, unsigned int>(trackCounter, id));
    }
  }
    info() << "RESULT: " << trackCutoff << "\t" << foundTracklets.size() << "\t" << numGoodTracklets << "\t" << pointsLayer1.size() + pointsLayer2.size() + pointsLayer3.size() << endmsg;


    ++trackCounter;
    }

  /*
  std::vector<CMCell<Hit>::CMntuplet> foundTracklets2;
  // prepare output
  unsigned int trackCounter = 0;
  for (const auto& tracklet : foundTracklets2) {
    std::array<float, 15> edmCov;
    float l_phi = 0;
    float l_theta = 0;
    float l_curvature = 0;
    trackSeedCollection->create(l_phi, l_theta, l_curvature, 0.f, 0.f, fcc::Point(), edmCov);
    for (const auto& id : tracklet) {
      theSeeds.insert(std::pair<unsigned int, unsigned int>(trackCounter, id));
    }

    ++trackCounter;
  }
  */

  return theSeeds;
}

StatusCode TrickTrackSeedingTool::finalize() { return GaudiTool::finalize(); }
