
import matplotlib

from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)

matplotlib.use("TkAgg")

import ROOT
ROOT.gSystem.Load("libdatamodel.so")
from ROOT import TLorentzVector



from EventStore import EventStore
import sys
import numpy as np
import matplotlib.pyplot as plt

events = EventStore([sys.argv[1]])
print 'number of events: ', len(events)
for i, store in enumerate(events):
    print "event ", i
    if i > 0:
      break
    pos = []
    ids = []
    part = store.get('allGenParticles')
    for t in part:
      v  = TLorentzVector(t.core().p4.px, t.core().p4.py, t.core().p4.pz, 1.)
      print v.Pt()
      print v.Eta()

          
