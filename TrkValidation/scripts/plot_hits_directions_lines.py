"""
Quick plotting script for visualizing FCCSW tracker hits.
Reads space point from a plain text file -- generate the spacepoints with the corresponding read_cluster* script.
"""

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
import sys

import numpy as np

filename = sys.argv[1]
basefilename = filename.replace(".root", "")

def colorhash(num):
  return hash(str(num)) % 256 / 256.

def id_to_color(ID):
  return [colorhash(x) for x in [ID, ID*2., ID*3.]]

def load_data(filename):
  return np.loadtxt(filename)

def plot_data_rphi(hits, directions, ids, cellIds, title="rphi"):
    f1 = plt.figure(title)
    ax1 = plt.axes()
    for i in np.unique(ids):
      col = id_to_color(i)
      h = hits[(ids == i) , 0:3]
      d = directions[(ids == i), 0:3]
      c = cellIds[(ids == i)]
    
      line_list = []
      color_list = []
      for hx, hy, dx, dy, _c in zip(h[:,0], h[:,1], d[:,0], d[:,1], c):
        line_list.append([(hx,hy), (hx+dx,hy+dy)])
        color_list.append(id_to_color(_c % 2**28))

      line_segments = LineCollection(line_list,
                                     linewidths=(1),
                                     facecolor=color_list,
                                     edgecolor=color_list,
                                     alpha=0.5,
                                     linestyles='solid')
      ax1.add_collection(line_segments)

      plt.title("Tracker hits from geant_pgun_fullsim")
      ax1.plot(h[:,0], h[:,1], 'o', color="red", ms=1, alpha=0.5)
      ax1.plot(h[:,0] + d[:,0], h[:,1] + d[:,1], 's', color="black", ms=1, alpha=0.5)

    ax1.set_title("Tracker Hits")
    ax1.set_xlabel("x")
    ax1.set_ylabel("y")
    hmax = np.max(h[:,0])
    hmin = np.min(h[:,0])
    ax1.set_xlim((1.1 * hmin, 1.1 * hmax))
    ax1.set_ylim((1.1 * hmin, 1.1 * hmax))

def plot_data_rz(hits, directions, ids, cells, title="rz"):
    f2 = plt.figure(title)
    ax2 = plt.axes()
    plt.title("Tracker hits from geant_pgun_fullsim")
    for i in np.unique(ids):
      col = id_to_color(i)
      h = hits[(ids == i), 0:3]
      d = directions[(ids == i), 0:3]
      r = np.sqrt(h[:,0]**2 + h[:,1]**2)
      r2 = np.sqrt((h[:,0] + d[:,0])**2 + (h[:,1] + d[:,1])**2)
      c = cellIds[(ids == i)]
      line_list = []
      color_list = []
      for _r1, _r2, hz, dz, _c in zip(r, r2, h[:,2], d[:,2], c):
        line_list.append([(hz,_r1), (hz+dz,_r2)])
        color_list.append(id_to_color(_c % 2**28))
      line_segments = LineCollection(line_list, linewidths=(1), linestyles='solid', 
                                     facecolor=color_list, edgecolor=color_list, 
                                     alpha=0.5)
      ax2.add_collection(line_segments)
      ax2.plot(h[:,2], r, 'o', color=col, ms=1, alpha=0.5)
      ax2.plot(h[:,2]+d[:,2], r2, 'o', color=col, ms=1, alpha=0.5)

    ax2.set_title("Tracker Hits")
    ax2.set_xlabel("z")
    ax2.set_ylabel("r")
    rmax = np.max(r)
    hmax = np.max(h[:,2])
    hmin = np.min(h[:,2])
    ax2.set_xlim((1.1 * hmin, 1.1 * hmax))
    ax2.set_ylim((0, 1.1 * rmax))




if __name__ == "__main__":
    data = load_data(basefilename + 'hit_positions_b.dat')
    hits = data[:,:3]
    directions = data[:,3:]
    id_data = load_data(basefilename + 'hit_ids_b.dat')
    ids = id_data[:,0]
    cellIds = id_data[:,1]
    plot_data_rphi(hits, directions, ids, cellIds, title="rphi-barrel")
    plot_data_rz(hits, directions, ids, cellIds, title="rz-barrel")
    data = load_data(basefilename + 'hit_positions_e.dat')
    hits = data[:,:3]
    directions = data[:,3:]
    id_data = load_data(basefilename + 'hit_ids_e.dat')
    ids = id_data[:,0]
    cellIds = id_data[:,1]
    plot_data_rz(hits, directions, ids, cellIds, title="rphi-endcap")
    plot_data_rphi(hits, directions, ids, cellIds, title="rz-endcap")
    plt.show()
