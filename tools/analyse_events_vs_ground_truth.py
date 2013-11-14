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


def overlap(evt1, evt2):
	return evt1['start'] < evt2['start'] < evt1['end'] or evt1['start'] < evt2['end'] < evt1['end'] or evt2['start'] < evt1['start'] < evt2['end'] or evt2['start'] < evt1['end'] < evt2['end']
	

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


def format_event(evt, name):
	return "<tr><td><a href=\"" + name + "-1.jpg\"><img src=\"" + name + "-1.jpg\" width=\"120\" alt=\"" + name + "\"/></a><td>" \
	+ name + "</td><td>" \
	+ str(evt["start"]) + "</td><td>" \
	+ str(evt["end"]) + "</td><td>" \
	+ evt["label"] + "</td><td>" + str(evt["valid"]) + "</td></tr>\n"


if len(sys.argv) != 4:
	print "usage " + sys.argv[0] + " ground_truth.srt detection.srt video_file"
	exit(1)


gt_list, n = srttable(sys.argv[1])
print "Found %d events in ground truth " % n
evt_list, n  = srttable(sys.argv[2])
print "Found %d events in events list " % n
video_file   = sys.argv[3]
output_dir   = "analysis"

try:
	os.mkdir(output_dir)
except:
	print "ERROR: Cannot create directory " + output_dir + "/. If this directory is already existant, please delete by hand it first."
	exit(0)

dst          = open("analysis/analysis.html", "w")

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
		if overlap(truth, intrusion):
			islegit = True
			break;
	intrusion["valid"] = islegit
	if not islegit:
		falsepositives.append(intrusion)
for truth in gt_list:
	islegit = False
	for intrusion in evt_list:
		if overlap(truth, intrusion):
			islegit = True
			break;
	truth["valid"] = islegit
	if not islegit:
		falsenegatives.append(truth)


#write results to text file

dst.write("""<!DOCTYPE html>
<html>
<head></head>
<body>""")

dst.write("<h1>Ground truth</h1>\n")
dst.write("<table border=\"1\" ><caption>Ground truth</caption>\n")
dst.write("<tr><th>picture</th><th>index</th><th>start</th><th>end</th><th>label</th><th>detected</th></tr>\n")
for index, evt in enumerate(gt_list):
	# if avconv is not available, use ffmpeg
	name = "ground_truth" + str(index)
	call("avconv -ss " + str(evt['start']) + " -i " + video_file + " -r 1 -f image2 " + output_dir + "/" + name + "-%d.jpg", shell=True)
	dst.write(format_event(evt, name))
dst.write("</table>")

dst.write("<h1>Detected events</h1>\n")
dst.write("<table border=\"1\" ><caption>Detected events</caption>\n")
dst.write("<tr><th>picture</th><th>index</th><th>start</th><th>end</th><th>label</th><th>detected</th></tr>\n")
for index, evt in enumerate(evt_list):
	# if avconv is not available, use ffmpeg
	name = "event" + str(index)
	call("avconv -ss " + str(evt['start']) + " -i " + video_file + " -r 1 -f image2 " + output_dir + "/" + name + "-%d.jpg", shell=True)
	dst.write(format_event(evt, name))
dst.write("</table>")


# calculate metrics
fp = len(falsepositives)
fn = len(falsenegatives)
tp = ftot - fp
gtneg = gtn - gtp
tn = gtneg - fp

try:
	tpr = tp / float(tp + fn)
	fpr = fp / float(fp + tn)
	precision = tp / float(tp + fp)
except:
	tpr = 0
	fpr = 0
	precision = 0

duration = 0 # TODO
dst.write("Total time: %s\n" % (duration))
dst.write("N:    %d\nTPR: %.2f\nFPR: %.2f\nprecision: %.2f\n" % (gtn, tpr, fpr, precision))


dst.write("</body>\n</html>")
dst.close()


# print "False positives:\n"
# print falsepositives
# print "False negatives:"
# print falsenegatives


exit(0)


