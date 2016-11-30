# Plot the solution that was generated
import math
import matplotlib
matplotlib.use('Agg')
from scipy.integrate import ode
from scipy.integrate import odeint
from numpy import loadtxt
from pylab import figure, plot, xlabel, grid, hold, legend, title, savefig, ylim
from matplotlib.font_manager import FontProperties
import os
import random

def vectorfield(w, t):
    """
    Defines the differential equations for the coupled spring-mass system.

    Arguments:
        w :  vector of the state variables:
                  w = [x1,y1,x2,y2]
        t :  time
    """
    #x1
    f = [2*math.pow(w[1],2)]

    for i in range(len(w))[1:-1]:
      if (i % 2 == 1 ):
        #y's
        f.append(w[i]*(w[i+1]-w[i-1]))
      else:
        #x's
        f.append(-2 * math.pow(w[i-1],2) + 2 * math.pow(w[i+1],2))
    #xn
    f.append(-2*math.pow(w[len(w)-2], 2))
    return f

def getNum(n, dirpath):
  info = os.walk(dirpath).next()
  s = str(n) + 'd_toda'
  a = [x for x in info[2] if s in x]
  return len(a)/2

def solout(t, y):
  if t > 50:
    return -1
  else:
    return 0

def ode_sort(n, dirpath):
  # Initial conditions
  # x1 and x2 are the initial displacements; y1 and y2 are the initial velocities
  arr = [random.randint(0,9999)]
  for i in range(n-1):
    arr.append(0.001)
    arr.append(random.randint(0,9999))

  print arr
  # ODE solver parameters
  abserr = 1.0e-8
  relerr = 1.0e-6
  stoptime = 100
  numpoints = 400

  # Create the time samples for the output of the ODE solver.
  # I use a large number of points, only because I want to make
  # a plot of the solution that looks nice.
  t = [stoptime * float(i) / (numpoints - 1) for i in range(numpoints)]

  w0 = arr

  # Call the ODE solver.
  wsol = odeint(vectorfield, w0, t,
                atol=abserr, rtol=relerr, mxstep = 500000)
  filename = str(n) + 'd_toda_' + str(getNum(n, dirpath))
  with open(dirpath + filename + '.dat', 'w') as f:
      # Print & save the solution.
      for t1, w1 in zip(t, wsol):
          print >> f, t1, ' '.join([str(w) for w in w1])
          #print t1, w1
  with open(dirpath + filename + '_x.dat', 'w') as f:
      for t1, w1 in zip(t, wsol):
          print >> f, t1, ' '.join([str(w) for w in w1][0::2])
  return filename

def graph(dirpath, filename):
  cols = loadtxt(dirpath + filename + '.dat', unpack=True)
  t = cols[0]
  vals = cols[1:]

  figure(1, figsize=(6, 4.5))

  xlabel('t')
  grid(True)
  hold(True)
  lw = 1
  colors = ['b', 'g', 'r', 'c', 'm']
  count = 0 
  for i in range(len(vals)):
    if i%2 == 0:
      plot(t, vals[i], colors[count%5] , linewidth = lw)
      count += 1
    else:
      plot(t, vals[i], 'y', linewidth = lw)

  title(filename)
  savefig(dirpath + filename + '.png', dpi=100)

def iterate(dirpath, filename, n):
  rows = open(dirpath + filename + '_x.dat', 'r')
  prev = None
  time = 0
  for line in rows:
    arr = line.split(" ")
    for x in arr:
      x = x.replace('\n','')
    xvalues = [float(x) for x in arr]
    i = 1
    good = True

    if not prev==None:
      while(i < len(xvalues) - 1 and good):
        if xvalues[i] < xvalues[i+1] or abs(prev[i]-xvalues[i]) > 0.1:
          good = False
        i = i + 1
      if abs(prev[i]-xvalues[i]) > 0.1:
        good = False
      if good:
        time = xvalues[0]
        break

    prev = xvalues

  with open('n_t.txt', 'a') as f:
    print >> f, n, time

def main():
  dirpath = 'Graphs/'
  for n in range(300):
    filename = ode_sort(n/6, dirpath)
    graph(dirpath, filename)
    iterate(dirpath, filename, n/6)

if __name__ == "__main__":
  main()

