import matplotlib

matplotlib.use("TkAgg")

import ROOT
ROOT.gSystem.Load("libdatamodel.so")


from EventStore import EventStore
import sys
import numpy as np
import matplotlib.pyplot as plt

events = EventStore([sys.argv[1]])
print 'number of events: ', len(events)
plt.figure()
for i, store in enumerate(events):
    print "event ", i
    if i > 1:
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
    plt.plot(pos[:,0],pos[:,1], '-')

    hits = store.get('positionedHits')
    pos = []
    ids = []
    for c in hits:
        cor = c.position()
        pos.append([cor.x, cor.y, cor.z])
        ids.append([c.bits()])
    pos = np.array(pos)
    ids = np.array(ids)
    plt.plot(pos[:,0],pos[:,1], 'o')

    tracks = store.get('tracks')
    pos = []
    ids = []
    for t in tracks:
        print t.hits_size()
plt.show()
