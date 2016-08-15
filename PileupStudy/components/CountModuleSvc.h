#ifndef PILEUPSTUDY_COUNTMODULESSVC_H
#define PILEUPSTUDY_COUNTMODULESSVC_H

#include "ITestDD4hepSvc.h"
#include "GaudiKernel/ITHistSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"


class CountModuleSvc: public extends1<Service, ITestDD4hepSvc> {
public:
   /// Constructor.
   explicit CountModuleSvc(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Finalize.
   virtual StatusCode finalize();
   /// Destructor
   virtual ~CountModuleSvc() {}

private:
  ITHistSvc* m_ths; ///< THistogram service
};

#endif /* PILEUPSTUDY_COUNTMODULESSVC_H */
