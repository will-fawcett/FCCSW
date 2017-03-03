import numpy as np
from numpy import *
from scipy import optimize
from matplotlib import pyplot as plt, cm, colors

def calc_R(x,y, xc, yc):
    """ calculate the distance of each 2D points from the center (xc, yc) """
    return np.sqrt((x-xc)**2 + (y-yc)**2)

def f(c, x, y):
    """ calculate the algebraic distance between the data points and the mean circle centered at c=(xc, yc) """
    Ri = calc_R(x, y, *c)
    return Ri - Ri.mean()

def leastsq_circle(x,y):
    # coordinates of the barycenter
    x_m = np.mean(x)
    y_m = np.mean(y)
    center_estimate = x_m, y_m
    center, ier = optimize.leastsq(f, center_estimate, args=(x,y))
    xc, yc = center
    Ri       = calc_R(x, y, *center)
    R        = Ri.mean()
    residu   = np.sum((Ri - R)**2)
    return xc, yc, R, residu



def leastsq_algebraic(x, y):
# coordinates of the barycenter
  x_m = mean(x)
  y_m = mean(y)

# calculation of the reduced coordinates
  u = x - x_m
  v = y - y_m

# linear system defining the center (uc, vc) in reduced coordinates:
#    Suu * uc +  Suv * vc = (Suuu + Suvv)/2
#    Suv * uc +  Svv * vc = (Suuv + Svvv)/2
  Suv  = sum(u*v)
  Suu  = sum(u**2)
  Svv  = sum(v**2)
  Suuv = sum(u**2 * v)
  Suvv = sum(u * v**2)
  Suuu = sum(u**3)
  Svvv = sum(v**3)

# Solving the linear system
  A = array([ [ Suu, Suv ], [Suv, Svv]])
  B = array([ Suuu + Suvv, Svvv + Suuv ])/2.0
  uc, vc = linalg.solve(A, B)

  xc_1 = x_m + uc
  yc_1 = y_m + vc

# Calcul des distances au centre (xc_1, yc_1)
  Ri_1     = sqrt((x-xc_1)**2 + (y-yc_1)**2)
  R_1      = mean(Ri_1)
  residu_1 = sum((Ri_1-R_1)**2)
  return xc_1, yc_1, R_1, residu_1

def plot_data_circle(x,y, xc, yc, R):
    f = plt.figure( facecolor='white')  #figsize=(7, 5.4), dpi=72,
    plt.axis('equal')
    theta_fit = np.linspace(-np.pi, np.pi, 180)
    x_fit = xc + R*np.cos(theta_fit)
    y_fit = yc + R*np.sin(theta_fit)
    plt.plot(x_fit, y_fit, 'b-' , label="fitted circle", lw=2)
    plt.plot([xc], [yc], 'bD', mec='y', mew=1)
    plt.xlabel('x')
    plt.ylabel('y')   
    # plot data
    plt.plot(x, y, 'ro', label='data', mew=1)
    plt.legend(loc='best',labelspacing=0.1 )
    plt.grid()

if __name__ == "__main__":
    dat = np.loadtxt("epos.dat")
    x = dat[:,0]
    y = dat[:,1]
    xc, yc, R, residu = leastsq_circle(x, y)
    #xc, yc, R, residu = leastsq_algebraic(x, y)
    print "Radius: ", R
    plot_data_circle(x,y, xc, yc, R)
    plt.show()
	
