#! /usr/bin/python
#
# -*- coding: utf-8 -*-
# Created on Wed Oct 16 11:10:51 2013
# 
# @author: polchky
# 
# Modified Nov 2013: Laurent Winkler

import miscfunctions as mf
import string
import time
import datetime
import sys
import os
from subprocess import call

def srttable(textfile):
	gt = open(textfile,"r")
	flist = []
	findex = 1
	tindex = 0
	while True:
		line = gt.readline()
		if line == "":
			break
		tindex += 1
		line = gt.readline().rstrip()
		fstart, fend = string.split(line," --> ")
		secs, cents = fstart.split(",")
		x = time.strptime(secs,"%H:%M:%S")
		fstart = datetime.timedelta(hours=x.tm_hour,minutes=x.tm_min,seconds=x.tm_sec).total_seconds() + float(cents)/1000
		secs, cents = fend.split(",")
		x = time.strptime(secs,"%H:%M:%S")
		fend = datetime.timedelta(hours=x.tm_hour,minutes=x.tm_min,seconds=x.tm_sec).total_seconds() + float(cents)/1000
		label = gt.readline().rstrip()
		flist.append({'index': findex, 'start': fstart, 'end': fend, 'label': label})
		gt.readline()
		findex += 1
	return flist, tindex


def format_event(evti, name):
	return "<p><b>" + name + "</b>" + str(evt) + "<img src=\"" + name + "-1.jpg\"/>" "</p>"


if len(sys.argv) != 4:
	print "usage " + sys.argv[0] + " ground_truth.srt detection.srt video_file"
	exit(1)


gt_list, n = srttable(sys.argv[1])
print "Found %d events in ground truth " % n
evt_list, n  = srttable(sys.argv[2])
print "Found %d events in events list " % n
dst          = open("analysis/analysis.html", "w")
video_file   = sys.argv[3]
output_dir   = "analysis"

try:
	os.mkdir(output_dir)
except:
	pass

gtp = 0 # positives for ground thruth
gtn = 0 # total number of sequences for ground thruth
fn = 0 # number of false negatives
ftot = 0 # total number of found sequences
falsenegatives = []
falsepositives = []

# Analyse
gtp += len(gt_list)
gtn += n
ftot += len(evt_list)


for intrusion in evt_list:
	islegit = False
	for truth in gt_list:
		if truth['start'] < intrusion['start'] < truth['end'] or truth['start'] < intrusion['end'] < truth['end'] or intrusion['start'] < truth['start'] < intrusion['end'] or intrusion['start'] < truth['end'] < intrusion['end']:
			islegit = True
			break;
	if not islegit:
		falsepositives.append(intrusion)
for truth in gt_list:
	islegit = False
	for intrusion in evt_list:
		if truth['start'] < intrusion['start'] < truth['end'] or truth['start'] < intrusion['end'] < truth['end'] or intrusion['start'] < truth['start'] < intrusion['end'] or intrusion['start'] < truth['end'] < intrusion['end']:
			islegit = True
			break;
	if not islegit:
		falsenegatives.append(truth)

# calculate metrics
fp = len(falsepositives)
fn = len(falsenegatives)
tp = ftot - fp
gtneg = gtn - gtp
tn = gtneg - fp
tpr = 0 # tp / float(tp + fn)
fpr = 0 # fp / float(fp + tn)
precision = 0 # tp / float(tp + fp)

#write results to text file

dst.write("""<html>
<head></head>
<body>""")

dst.write("<h1>False positives</h1>\n")
for index, evt in enumerate(falsepositives):
	# if avconv is not available, use ffmpeg
	name = "fp" + str(index)
	call("avconv -ss " + str(evt['start']) + " -i " + video_file + " -r 1 -f image2 " + output_dir + "/" + name + "-%d.jpg", shell=True)
	dst.write(format_event(evt, name))

dst.write("""</html>
</html>""")



duration = 0 # TODO
dst.write("Total time: %s\n" % (duration))
dst.write("N:    %d\nTPR: %.2f\nFPR: %.2f\nprecision: %.2f\n" % (gtn, tpr, fpr, precision))
dst.write("False positives:\n")
for ff in falsepositives:
    dst.write(str(ff))
dst.write("\nFalse negatives:\n")
for ff in falsenegatives:
    dst.write(str(ff))
dst.close()


# print "False positives:\n"
# print falsepositives
# print "False negatives:"
# print falsenegatives


exit(0)


