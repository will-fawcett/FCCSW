
import matplotlib

from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)

matplotlib.use("TkAgg")

import sys
import os

import os


import ROOT
ROOT.gSystem.Load("libdatamodel.so")

from mock_track import *
plot_tracks = True


from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt
plt.figure("xy")
ax = plt.gca()
for r in [25, 60, 100, 150]:
  ax.add_artist(plt.Circle((0,0), r,fill=False, color="red", alpha=0.4, lw=6 ))
plt.figure("rz")
ax = plt.gca()
for z in [25, 60, 100, 150]:
  ax.add_artist(plt.Rectangle((-150,z - 3), 300, 5 ,fill=True, color="red", alpha=0.4, lw=0 ))

events = EventStore([sys.argv[1]])
print 'number of events: ', len(events)
for i, store in enumerate(events):
    print "event ", i
    if i > 0:
      break
    pos = []
    ids = []
    clusters = store.get('trajectoryPoints')
    print clusters    
    for c in clusters:
        if c.bits() < 100:
          cor = c.position()
          pos.append([cor.x, cor.y, cor.z])
          ids.append([c.bits()])
    pos = np.array(pos)
    ids = np.array(ids)
    plt.figure("xy")
    plt.plot(pos[:,0],pos[:,1], '-')
    plt.figure("rz")
    plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), '-')

    hits = store.get('positionedHits')
    pos = []
    ids = []
    for c in hits:
        cor = c.position()
        pos.append([cor.x, cor.y, cor.z])
        ids.append([c.bits()])
    pos = np.array(pos)
    ids = np.array(ids)
    plt.figure("xy")
    plt.plot(pos[:,0],pos[:,1], 'o', color="midnightblue")
    plt.figure("rz")
    plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), 'o', color="midnightblue")

    if plot_tracks:

      tracks = store.get('tracks')
      for t in tracks:
          print "track ID: ", t.bits()
          pos = []
          ids = []
          ts = t.states(0)
          trackparams = [
            ts.d0(),
            ts.z0(),
            ts.phi() - np.pi * 0.5,
            ts.theta(),
            ts.qOverP(),
            ]
          print "track parameters: ", trackparams
          pos2 = helix(trackparams)
              
          for j in range(t.hits_size()):
            cor = t.hits(j).position()
            pos.append([cor.x, cor.y, cor.z])
          pos = np.array(pos)
          plt.figure("xy")
          plt.plot(pos[:,0],pos[:,1], '--', color="black")
          plt.plot(pos2[:,0],pos2[:,1], '-', lw=3, color="green")
          plt.figure("rz")
          plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), '--', color="black")
          plt.plot(pos2[:,2], np.sqrt(pos2[:,0]**2 + pos2[:,1]**2), '--', color="green")
      part = store.get('simParticles')
      for t in part:
          print "sim trackID: ", t.core().bits, "sim pdgId: ", t.core().pdgId, "momentum: ", [t.core().p4.px, t.core().p4.py, t.core().p4.pz]
          print "sim phi: ", np.arctan2(t.core().p4.py, t.core().p4.px)
          print "sim cottheta: ", t.core().p4.pz / np.sqrt(t.core().p4.py**2 +  t.core().p4.py**2)
          
plt.figure("xy")
plt.xlim(-400, 400)
plt.ylim(-400,400)
plt.xlabel("x [mm]")
plt.ylabel("y [mm]")
plt.savefig("tt_xy.png")
plt.savefig("tt_xy.pdf")
plt.figure("rz")
plt.xlim(-150, 150)
plt.ylim(0,150)
plt.xlabel("z [mm]")
plt.ylabel("r [mm]")
plt.savefig("tt_rz.png")
plt.savefig("tt_rz.pdf")

plt.show()
