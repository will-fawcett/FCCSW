from rootpy.io import root_open
import sys
import ROOT

from plotstyle import FCCStyle
FCCStyle.initialize()


filename = sys.argv[1]

f = root_open(filename)
cv = ROOT.TCanvas()

d = f.events.Draw("positionedHits.position.x:positionedHits.position.y", selection="0. < positionedHits.position.z < 500.")
#d = f.ExtrapolationTest.Draw("StepZ:StepR")
d.SetLineWidth(0)
d.xaxis.SetTitle("X [mm]")
d.xaxis.SetLabelSize(0.03)
d.yaxis.SetTitle("Y [mm]")
d.yaxis.SetLabelSize(0.03)
d.SetMarkerStyle(1)
d.SetMarkerSize(2)
d.Draw("APL")
cv.Print(filename + "_hits_xy.png")
#e = f.events.Draw("sqrt(pow(positionedHits.position.x,2) + pow(positionedHits.position.y,2) + pow(positionedHits.position.z,2))/positionedHits.core.time")

d = f.events.Draw("positionedHits.position.z:sqrt(pow(positionedHits.position.y,2) + pow(positionedHits.position.x,2))")
d.SetLineWidth(0)
d.xaxis.SetTitle("Z [mm]")
d.xaxis.SetLabelSize(0.03)
d.yaxis.SetTitle("R [mm]")
d.yaxis.SetLabelSize(0.03)
d.SetMarkerStyle(1)

d.SetMarkerSize(2)
d.Draw("APL")
cv.Print(filename + "_hits_rz.png")

