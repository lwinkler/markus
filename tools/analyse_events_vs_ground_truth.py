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
import subprocess


def overlap(evt1, evt2):
	return evt1['start'] < evt2['start'] < evt1['end'] or evt1['start'] < evt2['end'] < evt1['end'] or evt2['start'] < evt1['start'] < evt2['end'] or evt2['start'] < evt1['end'] < evt2['end']
	
# Read a subtitle file and store values
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

# format a list of events and write to html file
def format_events_list(evt_list, name, shortname, dst):
	dst.write("<h2>" + name + "</h2>\n")
	dst.write("<table border=\"1\" ><caption>" + name + ": %d</caption>\n" % len(evt_list))
	dst.write("<tr><th>picture</th><th>index</th><th>start</th><th>end</th><th>label</th><th>valid</th><th>matching</th></tr>\n")
	for index, evt in enumerate(evt_list):
		# if avconv is not available, use ffmpeg
		name = shortname + str(index)
		subprocess.call("avconv -ss " + str(evt['start']) + " -i " + video_file + " -r 1 -f image2 " + output_dir + "/" + name + "-%d.jpg", shell=True, stdout=None, stderr=None)
		dst.write(format_event(evt, name))
	dst.write("</table>")

# format an event in html
def format_event(evt, name):
	return "<tr><td><a href=\"" + name + "-1.jpg\"><img src=\"" + name + "-1.jpg\" width=\"120\" alt=\"" + name + "\"/></a><td>" \
	+ name + "</td><td>" \
	+ str(datetime.timedelta(seconds=evt["start"])) + "</td><td>" \
	+ str(datetime.timedelta(seconds=evt["end"])) + "</td><td>" \
	+ evt["label"] + "</td><td>" \
	+ str(evt["valid"]) + "</td><td>" \
	+ str(evt["matching"]) \
	+ "</td></tr>\n"

#---------------------------------------------------------------------------------------------------------------------------------------------------------------- 

## Main

if len(sys.argv) != 4:
	print "usage " + sys.argv[0] + " detected_events.srt ground_truth.srt video_file"
	exit(1)

evt_file   = sys.argv[1]
gt_file    = sys.argv[2]
video_file = sys.argv[3]

try:
	evt_list, n  = srttable(evt_file)
except:
	evt_list = []
	evt_file = ""
	n = 0
print "Found %d events in events list " % n

try:
	gt_list, n = srttable(gt_file)
except:
	gt_list = []
	gt_file = ""
	n = 0
print "Found %d events in ground truth " % n

output_dir   = "analysis"

try:
	os.mkdir(output_dir)
except:
	print "Cannot create directory " + output_dir + "/."
	# exit(0)

dst = open("analysis/analysis.html", "w")

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
	intrusion["matching"] = []
	for truth in gt_list:
		if overlap(truth, intrusion):
			islegit = True
			intrusion["matching"].append("gt" + str(truth["index"]))
	intrusion["valid"] = islegit
	if not islegit:
		falsepositives.append(intrusion)
for truth in gt_list:
	islegit = False
	truth["matching"] = []
	for intrusion in evt_list:
		if overlap(truth, intrusion):
			islegit = True
			truth["matching"].append("ev" + str(intrusion["index"]))
	truth["valid"] = islegit
	if not islegit:
		falsenegatives.append(truth)


#write results to text file

dst.write("""<!DOCTYPE html>
<html>
<head></head>
<body>""")
dst.write("<h1>Analysis of false alarms</h1>\n")
dst.write("<h2>Informations</h2>\n")
dst.write("<ul>\n")
dst.write("<li><b>Ground truth file: </b>%s</li>\n" % gt_file)
dst.write("<li><b>Detected events file: </b>%s</li>\n" % evt_file)
dst.write("<li><b>Video file: </b>%s</li>\n" % video_file)
dst.write("</ul>\n")


format_events_list(gt_list, "Ground truth", "gt", dst)
format_events_list(evt_list, "Detected events", "ev", dst)


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

dst.write("<h2>Statistics</h2>")
dst.write("<ul>\n")
dst.write("<li><b>Total time:</b> %s </li>\n" % (duration))
dst.write("<li><b>Events in ground truth: </b>%d</li>\n" % len(gt_list))
dst.write("<li><b>Events detected: </b> %d</li>\n" % len(evt_list))
dst.write("<li><b>False positives: </b> %d</li>\n" % fp)
dst.write("<li><b>False negatives: </b> %d</li>\n" % fn)
dst.write("<li>N:    %d TPR: %.2f FPR: %.2f precision: %.2f</li>\n" % (gtn, tpr, fpr, precision))
dst.write("</ul>\n")

try:
	# Not working TODO
	p = subprocess.Popen("avprobe " + video_file, stdout=subprocess.PIPE)
	dst.write("<h2>Information on video file</h2> %s" % p.communicate()[0])
except:
	print "Cannot read info on video file " + video_file


dst.write("</body>\n</html>")
dst.close()


# print "False positives:\n"
# print falsepositives
# print "False negatives:"
# print falsenegatives


exit(0)


