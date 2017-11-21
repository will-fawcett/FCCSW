import numpy as np
import matplotlib.pyplot as plt

dat = np.loadtxt("res.dat")
plt.plot(dat[:,0], dat[:,2] / dat[:,1], 'o' )
plt.show()
