import matplotlib
matplotlib.use("Agg")
from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt
import sys

for filename in sys.argv[1:]:
  pt = int(filename.split('pT')[1].split('_')[0])
  print filename, pt

  f = EventStore([filename])

  res = []

  for event in f:
    trackStateCollection = event.get("trackStates")
    genParticleCollection = event.get("GenParticles")
    for particle in genParticleCollection:
      pz = particle.core().p4.pz
      px = particle.core().p4.px
      py = particle.core().p4.py
      p =  np.sqrt(px**2 + py**2 + pz**2)#np.linalg.norm([px, py, pz])
      eta = 0.5 * np.log((p + pz) / (p - pz))

    for state in trackStateCollection:
      covariance = state.cov()
      parameters = [state.d0, state.z0, state.phi, state.theta, state.qOverP]
      res.append([eta, state.qOverP(), covariance[4], px, py, pz])

  res = np.array(res)

  eta = res[:,0]
  qOverP = res[:, 1]
  deltaQOverP = res[:,2]
  px = res[:,3]

  plt.figure('qOverP')
  plt.semilogy(eta, np.abs(qOverP), 'o', label=str(pt))
  plt.xlabel(r"$\eta$")
  plt.ylabel(r"$ q / P$")
  plt.legend(title='$p_T$')
  plt.savefig('trk_qOverP.png')
  plt.savefig('trk_qOverP.pdf')

  plt.figure('deltaQOverP')
  plt.semilogy(eta, deltaQOverP, 'o', label=str(pt))
  plt.xlabel(r"$\eta$")
  plt.ylabel(r"$\delta q / P$")
  plt.legend(title='$p_T$')
  plt.savefig("trk_deltaQOverP.png")
  plt.savefig("trk_deltaQOverP.pdf")

  plt.figure('resqOverP')
  plt.semilogy(eta, 1. /  np.abs(qOverP) * deltaQOverP, "o", label=str(pt))
  plt.xlabel(r"$\eta$")
  plt.ylabel(r"$\delta P / P$")
  plt.legend(title='$p_T$')
  plt.savefig("trk_qOverP_res.png")
  plt.savefig("trk_qOverP_res.pdf")

  plt.figure('px')
  plt.semilogy(eta, px, "o", label=str(pt))
  plt.xlabel(r"$\eta$")
  plt.ylabel(r"$P_x$")
  plt.legend(title='$p_T$')
  plt.savefig("trk_px_res.png")
  plt.savefig("trk_px_res.pdf")



