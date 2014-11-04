#!/usr/bin/python2

from math import pi

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

# Dynamic resistance of LED at VLED
RLED = 4.28

### Calculations ######################

# Maximum duty cycle
DMAX = (VLED + VD - VINMIN) / (VLED + VD - VFET)
print "*DMAX =", round(DMAX, 3), "on:", round(DMAX / fSW * 1e6, 3), "us, period:", 1e6 / fSW, "us"

# Average inductor current
ILAVG = ILED / (1 - DMAX)
print "*ILAVG =", round(ILAVG, 2), "A"

# Inductor current ripple
DIL = ILAVG * 0.3 * 2
print "*DIL =", round(DIL, 2), "A"

# Peak inductor current
ILP = ILAVG + DIL / 2
print "*ILP =", round(ILP, 2), "A"

# Inductor
L = (VINMIN - VFET) * DMAX / (fSW * DIL)
print "*> L =", round(L * 1e6, 1), "uH"

# Current sense resistor
R8 = 0.25 / (ILP * 1.25)
print "*> R8 =", round(R8, 3), "(power:", round((ILP ** 2) * R8 * DMAX, 3), "W)"

# Output capacitor
C3 = ILED * 2 * DMAX / (DVOUTRIPPLE * fSW)
ESR_C3 = DVOUTRIPPLE / (2 * ILP)
print "*> C3 =", round(C3 * 1e6), "uF (ESR <", round(ESR_C3, 4), ")"

# slope compensation capacitor
C2 = 3 * L * 100 * 1E-6 / ((VLED - VINMIN) * R8 *2)
print "*> C2 =", round(C2 * 1e9), "nF (assuming L =", round(L * 1e6, 1), "uH)"

# Input capacitor
C1 = DIL / (4 * DVIN * fSW)
ESR_C1 = DVIN / (DIL * 2)
print "*> C1 =", round(C1 * 1e6, 1), "uF (ESR <", round(ESR_C1, 3), ")"

# Sense+ resistor
R10 = 3.7 / (ILED * 9.9 * 4)
print "*> R10 =", round(R10, 3), "(V:", round(ILED * R10, 3), ", power:", round(ILED * ILED * R10, 3), "W)"

ID = ILAVG * (1 - DMAX) * 1.5
print "*> D1: I = ", round(ID, 2), "A, P =", round(ID * 0.7, 2), "W"

fZRHP = VLED * (1 - DMAX) ** 2 / (2 * pi * L * ILED)
ROUT = (RLED + R10) * VLED / ((RLED + R10) * ILED * DMAX + VLED)
fP2 = 1 / (2 * pi * C3 * ROUT)

GMcomp = 500e-6
R7 = fZRHP * R8 / (5 * fP2 * (1 - DMAX) * R10 * 9.9 * GMcomp)
print "*> R7 =", round(R7)
C7 = 1 / (2 * pi * R7 * fP2)
print "*> C7 =", round(C7 * 1e9, 1), "nF"

R3 = 5e6 / fSW * 1000.
print "*> R3 =", round(R3)

# Minimum LED current = 0
R5 = 0
# Ratio for ILED
r = 9.9 * R10 * ILED / 3.7
# Headroom
r *= 1.2
# Potentiometer
RV = 10e3
# Limiting resistor
R6 = RV / r * (1 - r)
print "*> R5 =", round(R5)
print "*> RV =", round(RV)
print "*> R6 =", round(R6), "(IVREF =", round(3.7 / (RV + R6) * 1000, 2), "mA)"

# Warnings

if DMAX > 0.5:
    print '!!! slope compensation required !!!'
if ILED * R10 > 0.3:
    print '!!! CC warning !!!'
