import matplotlib
from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)

matplotlib.use("TkAgg")

import ROOT
ROOT.gSystem.Load("libdatamodel.so")


from EventStore import EventStore
import sys
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

    tracks = store.get('tracks')
    for t in tracks:
        pos = []
        ids = []
        print "track phi: ", t.states(0).phi()
        for j in range(t.hits_size()):
          cor = t.hits(j).position()
          pos.append([cor.x, cor.y, cor.z])
        pos = np.array(pos)
        plt.figure("xy")
        plt.plot(pos[:,0],pos[:,1], '--', color="black")
        plt.figure("rz")
        plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), '--', color="black")
    part = store.get('simParticles')
    for t in part:
        print "sim phi: ", np.arctan2(t.core().p4.py, t.core().p4.px)
          
plt.figure("xy")
plt.xlim(0, 1500)
plt.ylim(0,1500)
plt.xlabel("x [mm]")
plt.ylabel("y [mm]")
plt.savefig("tt_xy_medium.png")
plt.savefig("tt_xy_medium.pdf")
plt.figure("rz")
#plt.xlim(-150, 150)
#plt.ylim(0,150)
plt.xlabel("z [mm]")
plt.ylabel("r [mm]")
plt.savefig("tt_rz_large.png")
plt.savefig("tt_rz.pdf")

plt.show()
