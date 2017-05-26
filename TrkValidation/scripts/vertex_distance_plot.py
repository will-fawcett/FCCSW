import matplotlib
matplotlib.use("Agg")
import ROOT
import numpy as np
import matplotlib.pyplot as plt
from ROOT import gStyle
from plotstyle import FCCStyle
from EventStore import EventStore

import sys

num_events = 100

def read_data_from_root(n=25):
  zPos = []
  f = EventStore(sys.argv[1:])
  for i, event in enumerate(f):
    vertexCollection = f.get("allGenVertices")
    zPos.append(vertexCollection[0].position().z)
    if i > n:
      break

  dat =  np.unique(zPos)
  dat = dat[np.abs(dat) < 100]
  return dat

def dummy_data(n):
  dat = np.random.normal(loc=0, scale=35, size=n) 
  return dat

for n in [25, 200, 1000]:
  datdiff = np.array([])
  for i in range(num_events):

    #dat = read_data_from_root(n)
    dat = dummy_data(n)
    datsorted = np.sort(dat)
    datdiff = np.concatenate([datdiff, np.diff(datsorted)])
  plt.figure("z diff")
  x, bins, p = plt.hist(datdiff, bins=np.logspace(np.log10(1e-6), np.log10(100.0), 100), histtype='stepfilled', alpha=0.6, label="PU: " + str(n), weights=[1./num_events]*len(datdiff))
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Z Distance between Neighbors [mm]")
plt.ylabel("\# Vertices")
plt.legend(loc="best")
plt.savefig("Vertex_neighbor_histo.png")



