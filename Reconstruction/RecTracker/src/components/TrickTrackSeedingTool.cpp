#include "TrickTrackSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

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
  declarePrivateTool(m_layerGraphTool, "BarrelLayerGraphTool/LayerGraphTool");

}

StatusCode TrickTrackSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto readout = lcdd->readout(m_readoutName);
  m_decoder = readout.idSpec().decoder();
  return sc;
}

void TrickTrackSeedingTool::createBarrelSpacePoints(std::vector<Hit>& thePoints,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex) {
  size_t hitCounter = 0;
  for (auto hit : *theHits) {
    m_decoder->setValue(hit.core().cellId);
    if ((*m_decoder)["system"] == sIndex.first) {
      if ((*m_decoder)["layer"] == sIndex.second) {
        thePoints.emplace_back(hit.position().x, hit.position().y, hit.position().z, hitCounter);
      }
    }
    ++hitCounter;
  }
}

std::multimap<unsigned int, unsigned int>
TrickTrackSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {

  fcc::TrackStateCollection* trackSeedCollection = m_trackSeeds.createAndPut();
  std::multimap<unsigned int, unsigned int> theSeeds;


  std::vector<Hit> pointsLayer1;
  std::vector<Hit> pointsLayer2;
  std::vector<Hit> pointsLayer3;

  createBarrelSpacePoints(pointsLayer1, theHits, m_seedingLayerIndices0);
  createBarrelSpacePoints(pointsLayer2, theHits, m_seedingLayerIndices1);
  createBarrelSpacePoints(pointsLayer3, theHits, m_seedingLayerIndices2);


  std::vector<HitDoublets<Hit>*> doublets;
  auto doublet1 = new HitDoublets<Hit>(pointsLayer1, pointsLayer2);
  auto doublet2 = new HitDoublets<Hit>(pointsLayer2, pointsLayer3);

  for (const auto& p0 : pointsLayer1) {
    for (const auto& p1 : pointsLayer2) {
      doublets[0]->add(p0.identifier(), p1.identifier());
    }
  }
  for (const auto& p1 : pointsLayer2) {
    for (const auto& p2 : pointsLayer3) {
      doublets[1]->add(p1.identifier(), p2.identifier());
    }
  }

  doublets.push_back(doublet1);
  doublets.push_back(doublet2);



  // get layer graph from tool and create automaton
  auto theLayerGraph = m_layerGraphTool->getGraph();
  auto automaton = new HitChainMaker<Hit>(theLayerGraph);
  TrackingRegion* myRegion2 = new TrackingRegion(10,0,0,1000);
  automaton->createAndConnectCells(doublets, *myRegion2, 1, 1000, 1000);

  automaton->evolve(3);
  std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
  automaton->findNtuplets(foundTracklets, 3);
  std::cerr << "found " << foundTracklets.size() <<  " tracklets!" << std::endl;
  std::cout << foundTracklets.size() << std::endl;


   /*
  findHelixSeeds(seedFinderCfg, pointsLayer1, pointsLayer2, pointsLayer3, seeds);

  debug() << "found " << seeds.size() << " track seeds" << endmsg;

  unsigned int trackCounter = 0;
  for (const auto& seed : seeds) {
    std::array<float, 15> edmCov;
    float l_phi = seed.phi();
    float l_theta = seed.theta();
    float l_curvature = seed.curvature();
    trackSeedCollection->create(l_phi, l_theta, l_curvature, 0.f, 0.f, fcc::Point(), edmCov);
    for (const auto& id : seed.identifiers()) {
      theSeeds.insert(std::pair<unsigned int, unsigned int>(trackCounter, id));
    }

    ++trackCounter;
  }

   */
  return theSeeds;
}

StatusCode TrickTrackSeedingTool::finalize() { return GaudiTool::finalize(); }
