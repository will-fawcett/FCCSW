#ifndef PILEUPSTUDY_OCCUPANCYHISTOGRAMS_H
#define PILEUPSTUDY_OCCUPANCYHISTOGRAMS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "FWCore/DataHandle.h"
#include "GaudiKernel/Service.h"


#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackClusterHitsAssociationCollection.h"

#include "TH1F.h"
#include "DetInterface/IGeoSvc.h"

#include "DD4hep/Volumes.h"
#include "DD4hep/LCDD.h"

class OccupancyHistograms: public GaudiAlgorithm {
  friend class AlgFactory<OccupancyHistograms> ;
public:
  /// Constructor.
  explicit OccupancyHistograms(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Finalize.
  virtual StatusCode finalize();
  /// Execute.
  virtual StatusCode execute();
  /// Destructor
  virtual ~OccupancyHistograms() {}

private:
  SmartIF<IGeoSvc> m_geoSvc;
  ITHistSvc* m_ths; ///< THistogram service
  TH1F* m_occupancy;
  TH1F* m_detEta; 
  TH1F* m_hitEta; 
  std::map<int, TH1F*> m_detEtas;
  std::map<int, TH1F*> m_hitEtas;
  DataHandle<fcc::TrackHitCollection> m_trkHits;
  DataHandle<fcc::TrackClusterCollection> m_trkClusters;
  DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trkHitClusterAssociations;
  std::string m_moduleNodeName;
  DD4hep::Geometry::VolumeManager m_volMan;
  std::set<unsigned long long int> m_occupiedCells;
  unsigned int m_multipleOccupancy;
};

#endif /* PILEUPSTUDY_OCCUPANCYHISTOGRAMS_H */
