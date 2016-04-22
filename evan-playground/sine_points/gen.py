import sys,os
import math
from numpy import arange

start = 0
stop = 2*math.pi
step = (stop-start)/100

xPoints = arange(start, stop, step)

out = []
for x in xPoints:
    y = 4096 * math.sin(x) + 2048 
    out.append( int(round(y)) )

print out


