from rootpy.io import root_open
import ROOT
from ROOT import gStyle
from plotstyle import FCCStyle
FCCStyle.initialize()
#gStyle.SetOptStat(11)

f = root_open("min_bias_pool.root")

nEvents = f.events.GetEntries()
nEventStr = "NEvents: " + str(f.events.GetEntries())

gStyle.SetNdivisions(405)
cv = ROOT.TCanvas()


###################################################################################################################3
htime = ROOT.TH1F("Hit Time Info, " + nEventStr, "Hit Time", 100, 0, 11600)
htime = f.events.Draw("positionedHits.core.time", selection="", hist=htime)
htime.Scale(1. / nEvents)
htime.Draw("HIST")

htime.GetXaxis().SetTitle("Time [ns]")
htime.GetYaxis().SetTitle("#Tracker Hits / Min. Bias Event")
#htime.GetXaxis().SetLabelSize(0.045)

cv.SetLogy()

cv.Print("plots/time.png")


##########################################################################################################################
gStyle.SetNdivisions(510)

h = ROOT.TH1F("Tracker Hit Rapidities", "Tracker Hit Eta Distribution", 100, -6, 6)

h = f.events.Draw("atanh(positionedHits.position.z / sqrt(pow(positionedHits.position.x,2) + pow(positionedHits.position.y,2) + pow(positionedHits.position.z,2)))", hist=h)

h.Scale(1. / nEvents)
h.Draw("HIST")

h.GetXaxis().SetTitle("#eta")
h.GetYaxis().SetTitle("#Tracker Hits / Min. Bias Event")

cv.Print("plots/HitsEta.png")

##################################################################################################################################
cv.SetLogy(0)
h = ROOT.TH1F("GenParticle Rapidities", "Primary Particle Eta Distribution", 50, -6, 6)
h = f.events.Draw("atanh(allGenParticles.core.p4.pz / sqrt(pow(allGenParticles.core.p4.px,2) + pow(allGenParticles.core.p4.py,2) + pow(allGenParticles.core.p4.pz,2)))", hist=h)
h.GetYaxis().SetLabelSize(0.04)
h.GetYaxis().SetRange(0, 110000)
h.Scale(1. / nEvents)

h.Draw("HIST")

h.GetXaxis().SetTitle("#eta")
h.GetYaxis().SetTitle("#Primary Particles / Min. Bias Event")

cv.Print("plots/GenParticleEta.png")


###################################################################################################################################3
cv.SetLogy(1)
gStyle.SetNdivisions(405)
h = ROOT.TH1F("GenVertex Z-Distribution", "Primary Particle Vertex Distribution", 100, -100, 100)
f.events.Draw("2* allGenVertices.position.z", hist=h)
h.Scale(1. / nEvents)
h.Draw("HIST")

h.GetXaxis().SetTitle("Z [mm]")
h.GetYaxis().SetTitle("#Primary Vertices / Min. Bias Event")
h.GetYaxis().SetLabelSize(0.04)

cv.Print("plots/GenVertexZ.png")


#########################################################################################################################################
cv.SetLogy(0)

h = ROOT.TH1F("Tracker Hits per Event", "Tracker Hits per Event", 100, 0, 70000)
f.events.Draw("@positionedHits.size()", hist=h)
h.Draw("HIST")
h.GetXaxis().SetTitle("No. of Tracker Hits")
h.GetYaxis().SetTitle("No. of Events")

cv.Print("plots/EventSize.png")

