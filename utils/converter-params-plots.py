#!/usr/bin/python2
from numpy import linspace, meshgrid
from matplotlib import pyplot
from mpl_toolkits.mplot3d.axes3d import Axes3D

fig = pyplot.figure()
ax = fig.gca(projection='3d')
#ax = fig.add_subplot(111, projection='3d')

### Inputs ############################

# Diode forward voltage
VD = 0.7

# LED power (W)
#PLED = 30
X = linspace(30, 90, 100)
# LED voltage
X = linspace(30, 80, 100)
VLED = 40
# Minimum input voltage
#VINMIN = 24
Y = linspace(10,25,100)
PLED, VINMIN = meshgrid(X,Y)

# LED current
#ILED = 1.25
ILED = PLED / VLED
#ILED = linspace(0.5, 5, 1000)

# Boost FET voltage drop
VFET = 0.2


# Switching frequency
fSW = 1e6

# Output voltage ripple
DVOUTRIPPLE = 0.01

# Input voltage ripple
DVIN = 0.12

### Calculations ######################

# Maximum duty cycle
DMAX = (VLED + VD - VINMIN) / (VLED + VD - VFET)
#print "*DMAX =", DMAX, "on:", DMAX / fSW * 1e6, "us, period:", 1e6 / fSW, "us"

# Average inductor current
ILAVG = ILED / (1 - DMAX)
#print "*ILAVG =", ILAVG

# Inductor current ripple
DIL = ILAVG * 0.3 * 2
#print "*DIL =", DIL

# Peak inductor current
ILP = ILAVG + DIL / 2
#print "*ILP =", ILP

# Inductor
L = (VINMIN - VFET) * DMAX / (fSW * DIL)
#print "*> L =", L * 1e6, "uH"

# Current sense resistor
R8 = 0.25 / (ILP * 1.25)
#print "*> R8 =", R8

# Output capacitor
C3 = ILED * 2 * DMAX / (DVOUTRIPPLE * fSW)
ESR_C3 = DVOUTRIPPLE / (2 * ILP)
#print "*> C3 =", C3 * 1e6, "uF (ESR <", ESR_C3, ")"

# Input capacitor
C1 = DIL / (4 * DVIN * fSW)
ESR_C1 = DVIN / (DIL * 2)
#print "*> C1 =", C1 * 1e6, "uF (ESR <", ESR_C1, ")"

# Sense+ resistor
R10 = 3.7 / (ILED * 9.9 * 2)
#print "*> R10 =", R10, "(V:", ILED * R10, ")"


### Plots #############################
#pyplot.plot(VLED,VINMIN,L)
p = ax.plot_surface(PLED, VINMIN, ILP, rstride=4, cstride=4, color='red')
pyplot.ticklabel_format(style='sci',axis='z', scilimits=(0,0))
pyplot.show()
