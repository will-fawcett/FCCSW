from rootpy.io import root_open
import sys
import os
from shutil import copyfile
import ROOT




from plotstyle import FCCStyle
FCCStyle.initialize()


# get primary file from command line
rootfile = sys.argv[1]

# prepare the plot directory
publish_dir_base = "/afs/cern.ch/user/v/vavolkl/www/pileup"
publish_dir = os.path.join(publish_dir_base, os.path.dirname(rootfile))
print "Directory to publish plots to: ", publish_dir
if not os.path.exists(publish_dir):
  print "\t does not exist yet. Creating ...." 
  os.makedirs(publish_dir)

f = root_open(rootfile)
cv = ROOT.TCanvas()

d = f.events.Draw("positionedHits.position.x:positionedHits.position.y", selection="0. < positionedHits.position.z < 500.")
d.SetLineWidth(0)
d.xaxis.SetTitle("X [mm]")
d.xaxis.SetLabelSize(0.03)
d.yaxis.SetTitle("Y [mm]")
d.yaxis.SetLabelSize(0.03)
d.SetMarkerStyle(1)
d.SetMarkerSize(2)
d.Draw("APL")
filename = rootfile.replace(".root", '') + "_hits_xy.png"
cv.Print(filename)
print "publishing plot file..."
copyfile(filename, os.path.join(publish_dir_base, filename))
print "wrote plot file  ", os.path.join(publish_dir_base, filename)


d = f.events.Draw("positionedHits.position.z:sqrt(pow(positionedHits.position.y,2) + pow(positionedHits.position.x,2))")
d.SetLineWidth(0)
d.xaxis.SetTitle("Z [mm]")
d.xaxis.SetLabelSize(0.03)
d.yaxis.SetTitle("R [mm]")
d.yaxis.SetLabelSize(0.03)
d.SetMarkerStyle(1)

d.SetMarkerSize(2)
d.Draw("APL")
filename = rootfile.replace(".root", '') + "_hits_rz.png"
cv.Print(filename)
print "publishing plot file..."
copyfile(filename, os.path.join(publish_dir_base, filename))
print "wrote plot file  ", os.path.join(publish_dir_base, filename)

