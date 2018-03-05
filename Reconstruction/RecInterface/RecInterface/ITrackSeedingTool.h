#ifndef RECINTERFACE_ITRACKSEEDINGTOOL_H
#define RECINTERFACE_ITRACKSEEDINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <map>

#include "datamodel/MCParticleCollection.h" // WJF add


namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}


class ITrackSeedingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackSeedingTool, 1, 0);

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) = 0;
  virtual std::multimap<unsigned int, unsigned int> findSeedsWithParticles(const fcc::PositionedTrackHitCollection* theHits, const fcc::MCParticleCollection*         theParticles) = 0;
};

#endif /* RECINTERFACE_ITRACKSEEDINGTOOL_H */
