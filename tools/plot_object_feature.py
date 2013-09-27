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


def plot_object(objName, features, plotType):
# Filter data per feature

	if plotType == 'linear':
		vect = np.where((data['object'] == objName) & (data['feature'] == features[0]))
		result = data[vect[0]]
		time1 = [ datetime.datetime.fromtimestamp(ms/1000.0) for ms in result['time']] 
		plt.errorbar(time1, result['value'], yerr=[math.sqrt(i) for i in result['sqVariance']], fmt="-o") # , label='value with variance') #, errorevery=5)
		plt.plot(time1, result['mean']) # , color='r', label='mean')
		# plt.plot(time1, result['min'], color='g', label='min')
		# plt.plot(time1, result['max'], color='g', label='max')
		# plt.plot(time1, result['initial'], color='y', label='initial')
		# plt.plot(time1, result['nbSamples'], color='pink', label='nbSamples')

		# Set plot
		ax1.set_title("Evolution of " + features[0] + " for " + objName)
		ax1.set_xlabel('time')
		ax1.set_ylabel(features[0])
	elif plotType == 'scatter':
		vect1 = np.where((data['object'] == objName) & (data['feature'] == features[0]))
		result1 = data[vect1[0]]
		vect2 = np.where((data['object'] == objName) & (data['feature'] == features[1]))
		result2 = data[vect2[0]]
		plt.scatter(result1['value'], result2['value']) # , color='r', label='mean')

		# Set plot
		ax1.set_title("Evolution of " + features[0] + " vs " + features[1])
		ax1.set_xlabel(features[0])
		ax1.set_ylabel(features[1])





# Main

# check arguments
if len(sys.argv) < 4:
	print "usage: " + sys.argv[0] + " plot_type file object feature(s)"
	print "ex: " + sys.argv[0] + " linear file object feature"
	print "ex: " + sys.argv[0] + " scatter file object feature1 feature2"

	exit()

fileName = sys.argv[1]
plotType = sys.argv[2]
objName  = sys.argv[3]
features  = sys.argv[4:]

data = np.genfromtxt(fileName, delimiter='\t', names=True, dtype=None)


#Build and show plot
fig = plt.figure()
ax1 = fig.add_subplot(111)

# xfmt = md.DateFormatter('%H:%M:%S,%f')
# time1 = [ t / 1000.0 for t in result['time']] 


plt.xticks(rotation=90)
# ax1.xaxis.set_major_formatter(xfmt)

if objName == "all":
	objNames = set(data['object'])
else:
	objNames = [objName]


# Plot all objects
for name in objNames:
	print "plot_object " + name
	plot_object(name, features, plotType)


#ax1.plot(x,y, c='r', label='the data')

leg = ax1.legend()

plt.show()
