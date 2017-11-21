
#include "RangePileup.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(RangePileup)

RangePileup::RangePileup(const std::string& type, const std::string& name, const IInterface* parent)
    : m_puCounter(0), GaudiTool(type, name, parent) {
  declareInterface<IPileUpTool>(this);
}

RangePileup::~RangePileup() { ; }

StatusCode RangePileup::initialize() {
  StatusCode sc = GaudiTool::initialize();
  printPileUpCounters();
  return sc;
}

unsigned int RangePileup::numberOfPileUp() { 
  unsigned int result =  m_puValues[m_puCounter];
  ++m_puCounter;
  m_puCounter = m_puCounter % m_puValues.size();
  return result;
 }

double RangePileup::getMeanPileUp() { return m_numPileUpEvents; }

std::string RangePileup::getFilename() { return m_filename; }

void RangePileup::printPileUpCounters() {
  info() << "Current number of pileup events: " << m_numPileUpEvents << endmsg;
}
