#!/usr/bin/python2

### Inputs ############################

# Diode forward voltage
VD = 0.7

# LED voltage
VLED = 40

# LED current
ILED = 1.25

# Boost FET voltage drop
VFET = 0.2

# Minimum input voltage
VINMIN = 24

# Switching frequency
fSW = 1e6

# Output voltage ripple
DVOUTRIPPLE = 0.01

# Input voltage ripple
DVIN = 0.12

### Calculations ######################

# Maximum duty cycle
DMAX = (VLED + VD - VINMIN) / (VLED + VD - VFET)
print "*DMAX =", DMAX, "on:", DMAX / fSW * 1e6, "us, period:", 1e6 / fSW, "us"

# Average inductor current
ILAVG = ILED / (1 - DMAX)
print "*ILAVG =", ILAVG

# Inductor current ripple
DIL = ILAVG * 0.3 * 2
print "*DIL =", DIL

# Peak inductor current
ILP = ILAVG + DIL / 2
print "*ILP =", ILP

# Inductor
L = (VINMIN - VFET) * DMAX / (fSW * DIL)
print "*> L =", L * 1e6, "uH"

# Current sense resistor
R8 = 0.25 / (ILP * 1.25)
print "*> R8 =", R8

# Output capacitor
C3 = ILED * 2 * DMAX / (DVOUTRIPPLE * fSW)
ESR_C3 = DVOUTRIPPLE / (2 * ILP)
print "*> C3 =", C3 * 1e6, "uF (ESR <", ESR_C3, ")"

# slope compensation capacitor
C2 = 3 * L * 100 * 1E-6 / ((VLED - VINMIN) * R8 *2)
print "*> C2 =", C2 * 1e9, "nF (assuming L =", L * 1e6, "uH)"

# Input capacitor
C1 = DIL / (4 * DVIN * fSW)
ESR_C1 = DVIN / (DIL * 2)
print "*> C1 =", C1 * 1e6, "uF (ESR <", ESR_C1, ")"

# Sense+ resistor
R10 = 3.7 / (ILED * 9.9 * 2)
print "*> R10 =", R10, "(V:", ILED * R10, ")"

ID = ILAVG * (1 - DMAX) * 1.5
print ">ID = ", ID

# Warnings

if DMAX > 0.5:
    print '!!! slope compensation required !!!'
if ILED * R10 > 0.3:
    print '!!! CC warning !!!'
