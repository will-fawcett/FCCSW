"""
Quick script to dump FCCSW tracker validation data to plaintext spacepoints.
Requires podio, fcc-edm in pythonpath and ld-library-path.
"""

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
      clusters = store.get('positionedHits')
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
          
import matplotlib.pyplot as plt
lens = np.array(lens)
print lens
plt.plot(lens[:,1], drawstyle="steps", label="No. hits in second endcap disc")
plt.plot(lens[:,0], drawstyle="steps", label="No. hits in first endcap disc")
plt.xlabel("Event no.")
plt.legend(loc='best')
plt.show()
          #cor = c.position()
          #if (c.cellId() % 32) == 0:
            
          #pos_b.append([cor.x, cor.y, cor.z])
          #ids_b.append([c.bits(), c.cellId()])
          #else:
          #  pos_e.append([cor.x, cor.y, cor.z])
          #  ids_e.append([c.bits(), c.cellId()])
#pos_e = np.array(pos_e)
#ids_e = np.array(ids_e)
#pos_b = np.array(pos_b)
#ids_b = np.array(ids_b)
#print "number of endcap hits: ", len(pos_e)
#print "number of barrel hits: ", len(pos_b)
#np.savetxt(basefilename + 'hit_positions_e.dat', pos_e)
#np.savetxt(basefilename + 'hit_ids_e.dat', ids_e, fmt="%i")
#np.savetxt(basefilename + 'hit_positions_b.dat', pos_b)
#np.savetxt(basefilename + 'hit_ids_b.dat', ids_b, fmt="%i")
