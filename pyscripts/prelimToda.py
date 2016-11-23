# Plot the solution that was generated
import math
from scipy.integrate import odeint
from numpy import loadtxt
from pylab import figure, plot, xlabel, grid, hold, legend, title, savefig, ylim
from matplotlib.font_manager import FontProperties


def vectorfield(w, t):
    """
    Defines the differential equations for the coupled spring-mass system.

    Arguments:
        w :  vector of the state variables:
                  w = [x1,y1,x2,y2]
        t :  time
        p :  vector of the parameters:
                  p = [m1,m2,k1,k2,L1,L2,b1,b2]
    """
    x1, y1, x2, y2, x3 = w

    # Create f = (x1',y1',x2',y2'):
    f = [2*math.pow(y1,2),
         y1*(x2-x1),
         -2*math.pow(y1,2)+2*math.pow(y2,2),
         y2*(x3-x2),
         -2*math.pow(y2,2)]
    return f

# Initial conditions
# x1 and x2 are the initial displacements; y1 and y2 are the initial velocities
x1 = 3
y1 = 0.25
x2 = 4
y2 = 0.25
x3 = 5

# ODE solver parameters
abserr = 1.0e-8
relerr = 1.0e-6
stoptime = 20.0
numpoints = 500

# Create the time samples for the output of the ODE solver.
# I use a large number of points, only because I want to make
# a plot of the solution that looks nice.
t = [stoptime * float(i) / (numpoints - 1) for i in range(numpoints)]

# Pack up the parameters and initial conditions:
#p = [m1, m2, k1, k2, L1, L2, b1, b2]
w0 = [x1, y1, x2, y2, x3]

# Call the ODE solver.
wsol = odeint(vectorfield, w0, t,
              atol=abserr, rtol=relerr)

with open('3d_toda_3.dat', 'w') as f:
    # Print & save the solution.
    for t1, w1 in zip(t, wsol):
        print >> f, t1, w1[0], w1[1], w1[2], w1[3], w1[4]


t, x1, y1, x2, y2, x3 = loadtxt('3d_toda_3.dat', unpack=True)

figure(1, figsize=(6, 4.5))

xlabel('t')
grid(True)
hold(True)
lw = 1

plot(t, x1, 'b', linewidth=lw)
plot(t, x2, 'g', linewidth=lw)
plot(t, x3, 'r', linewidth=lw)
plot(t, y1, 'y', linewidth=lw)
plot(t, y2, 'y', linewidth=lw)

ylim([-2,10])
legend((r'$x_1$', r'$x_2$', r'$x_3$', r'$y_1$', r'$y_2$'), prop=FontProperties(size=16))
title('3d Toda 3')
savefig('3d_toda_3.png', dpi=100)

