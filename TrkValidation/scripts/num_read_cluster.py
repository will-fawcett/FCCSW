"""
Quick script to dump FCCSW tracker validation data to plaintext spacepoints.
Requires podio, fcc-edm in pythonpath and ld-library-path.
"""


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
numhits = []

barrel_ids = []
for i, store in enumerate(events):
      clusters = store.get('positionedHits')
      particles = store.get("GenParticles")

      for p in particles:
        pt = np.sqrt(p.core().p4.px**2 + p.core().p4.py**2)
        pl = p.core().p4.pz
      eta = np.arctanh(pl / np.sqrt(pl**2 + pt**2))
      clist = []
      for c in clusters:
          clist.append(c.cellId() % 1024)
          cor = c.position()
          
          if False: # (c.cellId() % 32) in barrel_ids:
            
            pos_b.append([cor.x, cor.y, cor.z])
            ids_b.append([c.bits(), c.cellId()])
          else:
            pos_e.append([cor.x, cor.y, cor.z])
            ids_e.append([c.bits(), c.cellId()])
      numhits.append([eta, pt, len(np.unique(clist))])
      #numhits.append([eta, pt, len(clusters)])

pos_e = np.array(pos_e)
ids_e = np.array(ids_e)
pos_b = np.array(pos_b)
ids_b = np.array(ids_b)
print "number of endcap hits: ", len(pos_e)
print "number of barrel hits: ", len(pos_b)
np.savetxt(basefilename + 'hit_positions_e.dat', pos_e)
np.savetxt(basefilename + 'hit_ids_e.dat', ids_e, fmt="%i")
np.savetxt(basefilename + 'hit_positions_b.dat', pos_b)
np.savetxt(basefilename + 'hit_ids_b.dat', ids_b, fmt="%i")
np.savetxt(basefilename + 'numhits.dat', numhits)
