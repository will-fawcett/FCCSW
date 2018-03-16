#ifndef RECTRACKER_TRUTHSEEDINGTOOL_H
#define RECTRACKER_TRUTHSEEDINGTOOL_H

// stl
#include <map>

#include "myTrack.h"
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "datamodel/PositionedTrackHitCollection.h"

/** @class TruthSeedingTool
 * Create track seeds from MC-Truth
 */

class TruthSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TruthSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TruthSeedingTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  /// create and return the map trackId -> hitIndex, associating hits to tracks
  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) override final;
  virtual std::multimap<unsigned int, unsigned int> findSeedsWithParticles(const fcc::PositionedTrackHitCollection* theHits, const fcc::MCParticleCollection*         theParticles, std::vector<myTrack>& theTracks) override final; 

};

#endif /* RECTRACKER_TRUTHSEEDINGTOOL_H */
