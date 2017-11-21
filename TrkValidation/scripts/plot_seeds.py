"""
Quick script to dump FCCSW tracker validation data to plaintext spacepoints.
Requires podio, fcc-edm in pythonpath and ld-library-path.
"""

import matplotlib
matplotlib.use("agg")

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
pos = []
ids = []
barrel_ids = []
for i, store in enumerate(events):
      if i > 100000:
        break
      for branch in [#"trajectoryPoints",
                         "positionedHits"]:
        clusters = store.get(branch)
        for c in clusters:
            if c.bits() < 10:
              cor = c.position()
                
              pos.append([cor.x, cor.y, cor.z])
              ids.append([c.bits(), c.cellId()])
        pos_np = np.array(pos)
        plt.plot(pos_np[:,0], pos_np[:,1], '.')
plt.xlim(-200,200)
plt.ylim(-200,200)
plt.savefig('test.png')

