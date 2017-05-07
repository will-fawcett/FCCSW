from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt


f = EventStore(["trkFit_Example.root"])

res = []

for event in f:
  trackStateCollection = event.get("trackStates")
  genParticleCollection = event.get("allGenParticles")
  for particle in genParticleCollection:
    pz = particle.core().p4.pz
    px = particle.core().p4.px
    py = particle.core().p4.py
    p =  np.linalg.norm([px, py, pz])
    eta = 0.5 * np.log((p + pz) / (p - pz))

  for state in trackStateCollection:
    covariance = state.cov()
    parameters = [state.d0, state.z0, state.phi, state.theta, state.qOverP]
    res.append([eta, state.qOverP(), covariance[4]])

res = np.array(res)

eta = res[:,0]
qOverP = res[:, 1]
deltaQOverP = res[:,2]

plt.figure()
plt.semilogy(eta, qOverP, 'o')
plt.xlabel(r"$\eta$")
plt.ylabel(r"$ q / P$")
plt.savefig('trk_qOverP.png')
plt.savefig('trk_qOverP.pdf')

plt.figure()
plt.semilogy(eta, deltaQOverP, 'o')
plt.xlabel(r"$\eta$")
plt.ylabel(r"$\delta q / P$")
plt.savefig("trk_detlaQOverP.png")
plt.savefig("trk_detlaQOverP.pdf")

plt.figure()
plt.semilogy(eta, qOverP / deltaQOverP, "o")
plt.xlabel(r"$\eta$")
plt.ylabel(r"$\delta P / P$")
plt.savefig("trk_qOverP_res.png")
plt.savefig("trk_qOverP_res.pdf")




