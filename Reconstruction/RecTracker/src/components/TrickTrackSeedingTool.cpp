#include "TrickTrackSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"


#include "DD4hep/Detector.h"
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
  declareProperty("LayerGraphTool", m_layerGraphTool);
  declareProperty("DoubletCreationTool", m_doubletCreationTool);
  declareTool(m_layerGraphTool, "BarrelLayerGraphTool/LayerGraphTool");
  declareTool(m_doubletCreationTool, "DoubletCreationTool/DoubletCreationTool");

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
                                                       int) {
  size_t hitCounter = 0;
  for (auto hit : *theHits) {
    m_decoder->setValue(hit.core().cellId);
    if ((*m_decoder)["system"] == sIndex.first) {
      if ((*m_decoder)["layer"] == sIndex.second) {
        thePoints.emplace_back(hit.position().x, hit.position().y, hit.position().z, hitCounter);

        indexToTrackId.insert(std::pair<int, unsigned long int>(hitCounter, hit.core().bits));
      }
    }
        ++hitCounter;
  }
}

std::multimap<unsigned int, unsigned int>
TrickTrackSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {

  std::multimap<unsigned int, unsigned int> theSeeds;


  std::vector<Hit> pointsLayer1;
  std::vector<Hit> pointsLayer2;
  std::vector<Hit> pointsLayer3;

  std::map<int, unsigned long int> mapLayer1;
  std::map<int, unsigned long int> mapLayer2;
  std::map<int, unsigned long int> mapLayer3;

  createBarrelSpacePoints(pointsLayer1, mapLayer1, theHits, m_seedingLayerIndices0, 0);
  createBarrelSpacePoints(pointsLayer2, mapLayer2, theHits, m_seedingLayerIndices1, 0);
  createBarrelSpacePoints(pointsLayer3, mapLayer3, theHits, m_seedingLayerIndices2, 0);

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
  int trackletCounter = 0;

  for (const auto& tracklet : foundTracklets) {
    auto l_id1 = cells[tracklet[0]].getInnerHit().identifier();
    auto l_id2 = cells[tracklet[1]].getInnerHit().identifier();
    auto l_id3 = cells[tracklet[1]].getOuterHit().identifier();
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id1));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id2));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id3));

    if ((mapLayer1[l_id1] == mapLayer2[l_id2]) && 
        (mapLayer3[l_id3] == mapLayer2[l_id2])) {
      ++numGoodTracklets;
    }
    ++trackletCounter;
  }
  debug() << "found " << numGoodTracklets << " correct tracklets" << endmsg;


  return theSeeds;
}

StatusCode TrickTrackSeedingTool::finalize() { return GaudiTool::finalize(); }
