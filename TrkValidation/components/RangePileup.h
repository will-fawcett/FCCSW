#ifndef GENERATION_CONSTPILEUP_H
#define GENERATION_CONSTPILEUP_H

#include "Generation/IPileUpTool.h"

#include "GaudiAlg/GaudiTool.h"


class RangePileup : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  RangePileup(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~RangePileup();  ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  /** Implements IPileUpTool::numberOfPileUp.
   */
  virtual unsigned int numberOfPileUp();

  virtual double getMeanPileUp();
  /** Implements IPileUpTool::printPileUpCounters.
   */
  virtual void printPileUpCounters();

  virtual std::string getFilename();

private:
  /// Number of min bias events to pile on signal event.
  Gaudi::Property<unsigned int> m_numPileUpEvents{this, "numPileUpEvents", 0, "number of pile-up events"};
  /// Name of the file from which to read the pile-up
  Gaudi::Property<std::string> m_filename{this, "filename", "", "File from which to read the pile-up"};
  Gaudi::Property<std::vector<int>> m_puValues{this, "pileUpValueList", {1, 2, 3}, "List with PileUpValues "};
  
  int m_puCounter;
};

#endif  // GENERATION_CONSTPILEUP_H
