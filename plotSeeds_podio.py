
import os
import ROOT
ROOT.gSystem.Load("libdatamodelExt.so")
ROOT.gSystem.Load("libdatamodelExtDict.so")
from EventStore import EventStore
import matplotlib.pyplot as plt
import numpy as np
import sys


def colorhash(num):
  return hash(str(num)) % 256 / 256.

def id_to_color(ID):
  return [colorhash(x) for x in [ID*13., ID*2., ID*6534.]]



filename = sys.argv[1]
basefilename = filename.replace(".root", "")
events = EventStore([filename])
pointCollection = {}


for i, store in enumerate(events):
  if i > 0:
    break
  for branchName in ['positionedHits', 'trajectoryPoints']:
    data = {'x': [], 'y': [], 'z':[], 'r': [], 'cellId': [], 'trackId': []}
    pointCollection[branchName] = store.get(branchName)
    print len(pointCollection[branchName])
    for point in pointCollection[branchName]:
      position = point.position()
      data['x'].append(position.x)
      data['y'].append(position.y)
      data['z'].append(position.z)
      data['trackId'].append(point.bits())
      data['cellId'].append(point.cellId())
    #print data['trackId']

    trackId = np.array(data['trackId'], dtype=np.int64)
    #print trackId
    
    cellId = np.array(data['cellId'], dtype=np.int64)
    filter_array = (trackId < 12) * (cellId % 32 == 0)
    #filter_array = (cellId % 32 == 0)
    x = np.array(data['x'])[filter_array]
    y = np.array(data['y'])[filter_array]
    z = np.array(data['z'])[filter_array]
    trackId_filtered = trackId[filter_array]
    unique, counts = np.unique(trackId_filtered, return_counts=True)

    plt.figure(branchName + "XY")
    plt.plot(x, y, '.')
    plt.figure(branchName + "RZ")
    plt.plot(z, np.sqrt(x**2 + y**2), '.')
    plt.figure(branchName + "countHist")
    plt.hist(counts, bins=range(100))


    plt.figure("All MC Truth Info")
    if branchName == "trajectoryPoints":
      markerstring = "-"
    else:
      markerstring = "o"
    for u in unique:
      xu = x[trackId_filtered == u]
      yu = y[trackId_filtered == u]
      zu = z[trackId_filtered == u]
      plt.plot(xu, yu, markerstring, color=id_to_color(u), alpha=0.5) 
    plt.xlim(-200,200)
    plt.ylim(-200,200)

  
  for branchName in ["trackSeeds"]:
    pointCollection[branchName] = store.get(branchName)
    print "trackSeeds: ", len(pointCollection[branchName])
    plt.figure(branchName)
    xs = []
    ys = []
    for point in pointCollection[branchName]:
      xs.append( [ 
        pointCollection["positionedHits"][point.hitIndex1()].position().x,
        pointCollection["positionedHits"][point.hitIndex2()].position().x,
        pointCollection["positionedHits"][point.hitIndex3()].position().x,
        ]
        )
      ys.append( [ 
        pointCollection["positionedHits"][point.hitIndex1()].position().y,
        pointCollection["positionedHits"][point.hitIndex2()].position().y,
        pointCollection["positionedHits"][point.hitIndex3()].position().y,
        ]
        )
    for xs_, ys_ in zip(xs,ys):
      plt.plot(xs_, ys_, '-', color = id_to_color(np.random.rand()))
    plt.xlim(-200, 200)
    plt.ylim(-200, 200)
      

figs = [plt.figure(n) for n in plt.get_fignums()]
for i, fig in enumerate(figs):
    fig.savefig('figure%d.png' % i)

os.system("cp figure*.png $AFSHOME/www/pileup/seeding")


			

