


from ROOT import TH1F
nbins, lo, hi = 32, 0, 32
hist1 = TH1F('hist', 'my hist', nbins, lo, hi)
hist2 = TH1F('hist', 'my hist', nbins, lo, hi)

from EventStore import EventStore
import numpy as np
import sys

filename = sys.argv[1]
basefilename = filename.replace(".root", "")
events = EventStore([filename])
print 'number of events: ', len(events)
pos_b = []
ids_b = []
pos_e = []
ids_e = []
barrel_ids = []
lens = []
for i, store in enumerate(events):
      if i > 100:
        break
      clusters = store.get('trackSeeds')
      particles = store.get("GenParticles")
      lens.append([len(particles), len(clusters)])

"""
      #print clusters    
      dat1 = []
      dat2 = []
      for c in clusters:
          if c.cellId() % 32 == 2:
            if (c.cellId() >> 5) % 32 ==0:
              dat1.append(c)
            elif (c.cellId() >> 5) % 32 == 1:
              dat2.append(c)
      print len(dat1), len(dat2)
      lens.append([len(dat1), len(dat2)])
"""          
import matplotlib.pyplot as plt
lens = np.array(lens)
print lens
plt.figure(figsize=(5,5))
x = np.unique(lens[:,0])
y = np.array([np.max(lens[lens[:,0] == u,1]) for u in x])
plt.plot(x,y, marker='o')#, label="No. hits in second endcap disc")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Event size [No. Tracks]")
plt.ylabel("No. of Track Seeds")
plt.grid(True, which='minor')
plt.legend(loc='best')
plt.show()
