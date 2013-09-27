#! /usr/bin/python

# import matplotlib as mpl
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as md
import pylab as P
# import matplotlib.cbook as cbook

import csv
import sys
import math
import datetime



# check arguments
if len(sys.argv) < 4:
	print "usage: " + sys.argv[0] + " file object feature"
	exit()

fileName = sys.argv[1]
objName  = sys.argv[2]
feature  = sys.argv[3]

data = np.genfromtxt(fileName, delimiter='\t', names=True, dtype=None)


# Filter data per feature

vect = np.where((data['object'] == objName) & (data['feature'] == feature))
result = data[vect[0]]

#Build and show plot
fig = plt.figure()
ax1 = fig.add_subplot(111)

xfmt = md.DateFormatter('%H:%M:%S,%f')
# time1 = [ t / 1000.0 for t in result['time']] 
time1 = [ datetime.datetime.fromtimestamp(ms/1000.0) for ms in result['time']] 


plt.xticks(rotation=90)
# ax1.xaxis.set_major_formatter(xfmt)

plt.errorbar(time1,result['value'],yerr=[math.sqrt(i) for i in result['sqVariance']], fmt="-o", label='value with variance') #, errorevery=5)
plt.plot(time1, result['mean'], color='r', label='mean')
# plt.plot(time1, result['min'], color='g', label='min')
# plt.plot(time1, result['max'], color='g', label='max')
# plt.plot(time1, result['initial'], color='y', label='initial')
# plt.plot(time1, result['nbSamples'], color='pink', label='nbSamples')


ax1.set_title("Evolution of " + feature + " for " + objName)
ax1.set_xlabel('time')
ax1.set_ylabel(feature)

#ax1.plot(x,y, c='r', label='the data')

leg = ax1.legend()

plt.show()
