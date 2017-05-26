"""
Quick plotting script for visualizing FCCSW tracker hits.
Reads space point from a plain text file -- generate the spacepoints with the corresponding read_cluster* script.
"""

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
import matplotlib
import sys

import numpy as np


matplotlib.rcParams.update({'font.size': 18})



filename = sys.argv[1]
basefilename = filename.replace(".root", "")

def colorhash(num):
  return hash(str(num)) % 256 / 256.

def id_to_color(ID):
  return [colorhash(x) for x in [ID, ID/10., ID/100.]]

def load_data(filename):
  return np.loadtxt(filename)

def plot_data_rphi(hits, ids, cellIds, data_epos, title="rphi"):
    f1 = plt.figure(title, figsize=(7,7))
    ax1 = plt.axes()
    for i in np.unique(ids):
      col = id_to_color(i)
      col = 'black'
      h = hits[np.abs(hits[:,2]) < 800 , 0:3]
      plt.title("Tracker hits from geant_pgun_fullsim")
      if i == 1:
        ax1.plot(h[:,0], h[:,1], 'o', color=col, ms=5, alpha=0.8)
      else:
        ax1.plot(h[:,0], h[:,1], 'o', color=col, ms=2, alpha=0.8)

    #ax1.plot(data_epos[:,0], data_epos[:,1], '*', ms=5, alpha=0.8, color="blue")
    ax1.set_title("Tracker Hits")
    ax1.set_xlabel("X [mm]")
    ax1.set_ylabel("Y [mm]")
    hmax = np.max(h[:,:2])
    hmin = np.min(h[:,:2])
    ax1.set_xlim((0, 1.1 * hmax))
    ax1.set_ylim((0, 1.1 * hmax))
    plt.axes().set_aspect('equal', 'datalim')
    plt.savefig(basefilename + "rphi.png")

def plot_data_rz(hits, ids, cells, data_epos, title="rz"):
    f2 = plt.figure(title, figsize=(6,4))
    ax2 = plt.axes()
    plt.title("Tracker hits from geant_pgun_fullsim")
    for i in np.unique(ids):
      col = id_to_color(i)
      col = 'black'
      h = hits[(ids == i), 0:3]
      r = np.sqrt(h[:,0]**2 + h[:,1]**2)
      c = cellIds[(ids == i)]
      if i == 1:
        ax2.plot(h[:,2], r, 'o', color=col, ms=5, alpha=0.8)
      else:
        ax2.plot(h[:,2], r, '.', color=col, ms=2, alpha=0.8)

    #ax2.plot(data_epos[:,2], np.sqrt(data_epos[:,0]**2 + data_epos[:,1]**2), '*', ms=5, alpha=0.8, color="blue")
    ax2.set_title("Tracker Hits")
    ax2.set_xlabel("Z [mm]")
    ax2.set_ylabel("R [mm]")
    rmax = np.max(r)
    hmax = np.max(h[:,2])
    hmin = np.min(h[:,2])
    ax2.set_xlim((0, 16100))
    ax2.set_ylim((0, 2000))
    plt.savefig(basefilename + "rz.png")

def plot_numhits(hits, ids, cells, title="numhits"):
    f3 = plt.figure(title + "_pt", figsize=(7,7))
    ax3 = plt.axes()
    ax3.plot(hits[:,1], hits[:,2], '<', color="blue", ms = 5, alpha=1)
    ax3.set_xlabel(r"$p_T$ [GeV]")
    ax3.set_ylabel("No. of hits")
    ax3.set_xlim(0,100)
    ax3.set_ylim(0,30)
    plt.savefig(basefilename + "numhits_pt.png")
    f3 = plt.figure(title + '_eta', figsize=(7,7))
    ax3 = plt.axes()
    ax3.plot(hits[:,0], hits[:,2], 'o', color="blue", ms=5, alpha=1)
    ax3.set_xlabel(r"$\eta$")
    ax3.set_ylabel("No. of hits")
    ax3.set_xlim(0,6)
    ax3.set_ylim(0,30)
    plt.savefig(basefilename + "numhits_eta.png")
  



if __name__ == "__main__":
    #data = load_data(basefilename + 'hit_positions_b.dat')
    #hits = data
    #id_data = load_data(basefilename + 'hit_ids_b.dat')
    #ids = id_data[:,0]
    #cellIds = id_data[:,1]
    #plot_data_rphi(hits, ids, cellIds, title="rphi-barrel")
    #plot_data_rz(hits, ids, cellIds, title="rz-barrel")
    data = load_data(basefilename + 'hit_positions_e.dat')
    #data_epos = load_data("epos.dat")
    data_epos = []
    hits = data
    id_data = load_data(basefilename + 'hit_ids_e.dat')
    ids = id_data[:,0]
    cellIds = id_data[:,1]
    plot_data_rz(hits, ids, cellIds, data_epos, title="rphi-endcap")
    plot_data_rphi(hits, ids, cellIds, data_epos, title="rz-endcap")
    data = load_data(basefilename+"numhits.dat")
    plot_numhits(data,ids, cellIds,title="numhits")
    plt.show()
