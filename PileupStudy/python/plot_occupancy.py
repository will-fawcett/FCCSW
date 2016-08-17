import ROOT
import numpy as np
from rootpy.io import root_open
from rootpy.plotting import Hist, HistStack, Legend, Canvas
from rootpy.plotting.style import get_style, set_style
from rootpy.plotting.utils import get_limits
from rootpy.interactive import wait
import rootpy.plotting.root2matplotlib as rplt
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator, MultipleLocator
import re
import sys
import os


# Histograms are assumed to have a number code suffix 
# detector_id * 100 + layer_id
# (approximately)


detector_names = ['Inner Barrel',
 "Outer Barrel",
 "Pos. Inner Endcap",
 "Neg. Inner Endcap",
 "Pos. Outer Endcap",
 "Neg. Outer Endcap"]
detector_colors = {'Inner Barrel': 'violet',
  "Outer Barrel" : 'darkviolet',
 "Pos. Inner Endcap": 'lightskyblue',
 "Neg. Inner Endcap": 'lightskyblue',
 "Pos. Outer Endcap": 'royalblue',
 "Neg. Outer Endcap": 'royalblue' }

filename_without_ending = sys.argv[1].replace(".root", "") 

def choose_color(detector_name):
    index = int(re.findall(r'\d+', detector_name)[0])
    return detector_colors[detector_names[index / 100]]

def make_occupancy_plots():
  detstack = HistStack()
  detstack.title = "Tracker Module Cell Distribution"
  detstack.fname = filename_without_ending + "_det.pdf"
  hitstack = HistStack()
  hitstack.title = "Tracker Hit Distribution"
  hitstack.fname = filename_without_ending + "_hit.pdf"
  myfile = root_open(sys.argv[1])

  detdir = myfile.DetEta

  det = []
  hit = []

  for path, dirs, objects in myfile.walk():
    for obj_name in sorted(objects):
        obj = myfile[os.path.join(path, obj_name)]
        # parse hist name, e.g. DetHist509
        obj.fillcolor = choose_color(obj.GetName())
        obj.fillstyle = "solid"
        if "DetEta" in path:
          detstack.Add(obj)
          det.append(obj)
        if "HitEta" in path:
          hitstack.Add(obj)
          hit.append(obj)



  set_style('ATLAS', mpl=True)
  occstack = HistStack()
  occstack.title = "Tracker Occupancy"
  occstack.fname = filename_without_ending + "_occ.pdf"

#calculate occupancy
  for h, d in zip(hit, det):
    occ_hist = h.Clone(h.GetName())
    occ_hist.Divide(d)
    occ_hist.linecolor = choose_color(occ_hist.GetName())
    occ_hist.fillstyle = "none"
    occ_hist.linewidth = 2
    occstack.Add(occ_hist)

  artists = []
  for d in detector_names:
    artists.append(plt.Line2D((0,1),(0,0), color=detector_colors[d], marker='s', linestyle=''))

  for h1 in [detstack, hitstack, occstack]:
    plt.figure()
    plt.title(h1.title)
    plt.legend(artists, detector_names, loc="best", fontsize=12)
    plt.xlabel(r"$\eta$")
    if h1.title == occstack.title:
      plt.ylim(1e-6, 1)
      plt.yscale("log")
      rplt.hist(h1, stacked=False, yerr=False)
    else:
      rplt.hist(h1, stacked=True, yerr=False)
    plt.savefig(h1.fname)
    with open(filename_without_ending + ".plotlog", "w") as f:
      f.write("...plotting done!")

if __name__ == "__main__":
  make_occupancy_plots()
