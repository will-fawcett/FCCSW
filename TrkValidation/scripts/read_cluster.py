"""
Quick script to dump FCCSW tracker validation data to plaintext spacepoints.
Requires podio, fcc-edm in pythonpath and ld-library-path.
"""


from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt
import sys

filename = sys.argv[1]
basefilename = filename.replace(".root", "")
events = EventStore([filename])
print 'number of events: ', len(events)
pos = []
ids = []
barrel_ids = []
for i, store in enumerate(events):
      if i > 100000:
        break
      clusters = store.get('positionedHits')
      for c in clusters:
          cor = c.position()
            
          pos.append([cor.x, cor.y, cor.z])
          ids.append([c.bits(), c.cellId()])
      print pos

np.savetxt(basefilename + 'hit_positions.dat', pos)
np.savetxt(basefilename + 'hit_ids.dat', ids, fmt="%i")
