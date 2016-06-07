#!/usr/bin/env python

"""
Evaluation of a video events regarding to its ground truth

This python module evaluate a file containing video events in comparison with
the ground truth of the same video.

Example:
	To analyse an events file with its ground truth:
		$ python ./analyse_events.py events_file.srt ground_truth.srt

Created 2014-01-29 Fabien Dubosson
"""

import re
import os
import vplib
import pickle
import argparse
import subprocess
import json
import datetime
from time import strftime, localtime
from platform import platform
from vplib.time import Time
from vplib.HTMLTags import *
from vplib.parser import subrip
from collections import namedtuple, OrderedDict

# Evaluation metrics
Evaluation = namedtuple('Evaluation',
		'det '
		'pos '
		'tp '
		'fp '
		'fn '
		'dup_ev '
		'dup_gt '
		'ambs ')

# Video metrics
Video = namedtuple('Video', 'duration start_timestamp')

# Truth tuple
Truth = namedtuple('Truth', 'id begin end match_begin match_end is_valid is_ambiguous text')

# Event tuple
Event = namedtuple('Event', 'id begin end image_file')

# Global arguments, will be overwritten at runtime
args = None


def is_tool(name):
	""" Check if a tool exists """
	try:
		devnull = open(os.devnull, 'w')
		subprocess.check_call(['which', name], stdout=devnull, stderr=subprocess.STDOUT)
	except subprocess.CalledProcessError:
		return False
	return True


# Compare with event name: fall, intrusion, ...
def is_valid(text):
	""" Function to check if a subtitle is an event

	Syntaxe (convention):
		eventName1 [eventName2...][: comment]

	Examples:
		intrusion
		intrusion: this is a normal intrusion with a dummy comment
		intrusion ambiguous
		foobar: this is not an intrusion
	"""

	# Extract only the event names
	if ":" in text:
		text = (text.split(":"))[0]

	return args.EVENT_NAME.lower() in text.lower()

# Check event name if ambiguous
def is_ambiguous(text):
	""" Function to check if a subtitle is a ambiguous """
	return len(args.AMBIGUOUS) > 0 and args.AMBIGUOUS.lower() in text.lower()


def video_info(video):
	""" Function to get information about the video. Note: This MUST be similat to what is done in markus/util/util.cpp : timeStampFromFileName """
	# If no video
	if video is None:
		return None
	
	# Detect starting time stamp from the video file name
	start_timestamp = datetime.datetime.utcfromtimestamp(0)
	if args.start_timestamp == -1:
		try:
			word=re.search(r'^(\d+_\d+)_',os.path.basename(video)).group(1)
			# start_timestamp = (datetime.datetime.strptime(word,"%Y%m%d_%H%M%S") - epoch).total_seconds() * 1000.0
			start_timestamp = datetime.datetime.strptime(word,"%Y%m%d_%H%M%S")
			print "Video is considered to start at %s" % str(start_timestamp)
		except:
			print "Video is considered to start at %s" % str(start_timestamp)

	try:
		# Run avprobe to get info. Note: This replaces ffprobe
		ff = subprocess.Popen(['avprobe', video], stderr=subprocess.PIPE)
		# Get back results
		ffout = str(ff.communicate())
		# Search the duration in the output
		dval = re.search('Duration: ([^,]*),', ffout)
		# Return the informations
		return Video(duration=Time(text=dval.group(1), sep_ms='.'),start_timestamp=start_timestamp)
	except subprocess.CalledProcessError:
		return None


def extract_images(events, truths, video, out='out'):
	""" Extract thumbnails images of events and ground truths """
	# If no video
	if video is None:
		return

	# Ensure directory exists
	path = os.path.join(out, 'images')
	if not os.path.exists(path):
		os.makedirs(path)

	if is_tool('avconv'):
		cmd = 'avconv'
	else:
		cmd = 'ffmpeg'

	# The function to extract a precise image
	def extract(kind, time, name):
		# note: force images to qvga 320x240 to reduce archive size
		str_time = str(time).replace(',','.')  # note: avoid ","
		subprocess.call([cmd, '-ss', str_time, '-i', str(video),
						 '-frames:v', '1', '-s', 'qvga',
						 os.path.join(path, str(kind) + '_' + str(name) +
									  '.jpg'), '-y'], stderr=subprocess.PIPE)

		# We have a problem with precision of snapshots in time: this happens due to reencoded videos and
		# due to the "-s qvga" parameters
		# This version solves the problem for some videos but is much slower
		# target   = os.path.join(path, str(kind) + '_' + str(name) + '.jpg')
		# subprocess.call([cmd, '-ss', str_time, '-i', str(video),
		#				   '-frames:v', '1', target, '-y'], stderr=subprocess.PIPE)
		# subprocess.call(['convert', target, '-resize' ,'320x240', target])

	# Extract events
	for event in events:
		# Note: We take the snapshot in the middle of the event
		if event.image_file == "./images/event_" + str(event.id) + ".jpg":
			extract('event', (event.begin + event.begin) / 2, event.id)

	# Extract ground truth
	for truth in truths:
		extract('truth', (truth.begin + truth.end) / 2, truth.id)


def match_times(begin, end):
	""" Return the time which must be used to check if the event occurs """
	
	# If uncompromising, impose the begin time to be at least d seconds after
	if args.uncompromising:
		m_begin = begin + Time(seconds=args.delay) - Time(seconds=args.tolerance)
	else:
		# Otherwise, accept alert arriving before delay
		m_begin = begin - Time(seconds=args.tolerance)
	
	# End time adjustements
	m_end = end + Time(seconds=args.delay) + Time(seconds=args.tolerance)

	# Return matching times
	return m_begin, m_end


def read_events(file_path, start_timestamp):
	"""Read the events file"""
	# previous version: return evtfiles.parse(file_path)
	entries = subrip.parse(file_path)
	if start_timestamp == 0:
		start_timestamp = datetime.datetime.utcfromtimestamp(0)

	# Prepare the events array
	events = []

	# Parse each subtitle
	for entry in entries:

		# Match times
		entry.begin.subtractDate(start_timestamp)
		match_begin, match_end = match_times(entry.begin, entry.end)
		ident = entry.begin.milis

		if args.VIDEO_FILE:
			# By default we set the path of an image to be extracted from video
			image_file = "./images/event_" + str(ident) + ".jpg"
		else:
			image_file = ""

		try:
			# Detect if an image exists already
			detail = json.loads(entry.text)
			image_file = '../events_img/' + os.path.basename(detail['external']['files']['globalImage'])
		except:
			print "Cannot find image for event at %s" % entry.begin

		events.append(Event(id=ident, begin=entry.begin, end=entry.end, image_file=image_file))

	return events

def read_truths(file_path):
	"""Read the ground truth file"""

	# Read the file
	entries = subrip.parse(file_path)

	# Prepare the truths array
	truths = []

	# Parse each subtitle
	for entry in entries:

		# Look if this is a fall
		fall	  = is_valid(entry.text)
		ambiguous = is_ambiguous(entry.text)

		# If is is garbage skip
		if not fall or fall is None:
			continue

		entry_begin = entry.begin
		entry_end   = entry.end

		# Match times
		match_begin, match_end = match_times(entry_begin, entry_end)

		# Otherwise add to the truths array
		truths.append(Truth(id=entry.number,
							begin=entry_begin,
							end=entry_end,
							match_begin=match_begin,
							match_end=match_end,
							text=entry.text.strip(' \t\n\r'),
							is_valid=fall,
							is_ambiguous=ambiguous))

	return truths


def evaluate(events, truths):
	"""Evaluate the events on the ground truth"""

	# We use text files to store false positives, false negatives and true positives
	# TODO: In the future the generated files should respect an existing format. E.g. *.ass or *.csv or pickle
	#   - the 3 first lines were added by Alexandre Heili for his scripts
	#   - the 4th line was added by Florian Rossier
	fid_fp = open(args.output + '/FP.txt','w')
	fid_fn = open(args.output + '/FN.txt','w')
	fid_tp = open(args.output + '/TP.txt','w')
	fid_gt = open(args.output + '/GT.txt','w') 
	
	# Variable to log event
	log_event = []
	log_truth = []

	# Matched
	matched_events	   = dict()
	matched_truths	   = dict()

	# Stats
	fp = 0
	fn = 0
	ndup_ev = 0
	ambs = 0
	ndup_gt = 0

	# For each event
	for event in events:
		evt_dup = False
		# Search for a matching truth
		for i, truth in enumerate(truths):
			# Count as ambiguous
			matched = event.begin >= truth.match_begin and event.begin <= truth.match_end

			# Test for matching
			if matched:
				fid_tp.write('%s %s\n' % (event.begin.milis, event.end.milis))
				# Keep track of matched ground truth
				if event.id not in matched_events:
					matched_events[event.id] = []
				else:
					ndup_gt += 1 # several gt for one event
				if truth.id not in matched_truths:
					matched_truths[truth.id] = []
				else:
					ndup_ev += 1 # several event for one gt
					evt_dup = True
				matched_events[event.id].append(truth)
				matched_truths[truth.id].append(event)
		if event.id in matched_events:
            		fid_gt.write('1\n') # this is a true event mark it with 1
		# Log event
		log_event.append((event, matched_events[event.id] if event.id in matched_events else [], event.id in matched_events, evt_dup))

		if not event.id in matched_events:
			fp += 1
			fid_fp.write('%s %s\n' % (event.begin.milis, event.end.milis))
			fid_gt.write('0\n') #false positive event mark it with 0

	tp = 0
	for truth in truths:
		log_truth.append((truth, matched_truths[truth.id] if truth.id in matched_truths else [], truth.id in matched_truths))
		if truth.is_ambiguous:
			ambs += 1
			continue
		if not truth.id in matched_truths:
			fid_fn.write('%s %s\n' % (truth.begin.milis, truth.end.milis))
			fn += 1
		else:
			tp += 1

	# print "fp %d fn %d tp %d. dup_ev %d dup_gt %d" % (fp, fn, tp, ndup_ev, ndup_gt)

	# Prepare evaluation results
	results = Evaluation(tp=tp,
						 fp=fp,
						 fn=fn,
						 dup_ev=ndup_ev,
						 ambs=ambs,
						 dup_gt=ndup_gt,
						 det=len(events),
						 pos=len(truths) - ambs)
	fid_fp.close()
	fid_fn.close()
	fid_tp.close()
	fid_gt.close()

	return (results, (log_event, log_truth))


def statistics(evaluation, video=None):
	"""Compute the statistics on the evaluation"""

	# Shortcuts
	e = evaluation
	v = video

	# Prepare the statistic dictionnary
	stats = OrderedDict()

	# Counters statistics
	stats['Counters'] = ('-' * 20, '%s')
	stats['Events'] = (e.pos, '%4d')
	stats['Detections'] = (e.det, '%4d')
	stats['Correct detections'] = (e.tp, '%4d')
	stats['False alarms'] = (e.fp, '%4d')
	stats['Missed detections'] = (e.fn, '%4d')
	stats['Duplicated detections'] = (e.dup_ev, '%4d')
	stats['Duplicated gt'] = (e.dup_gt, '%4d')
	stats['Ambiguous']	= (e.ambs, '%4d')

	# Confusion matrix statistics
	stats['Statistics'] = ('-' * 20, '%s')
	if not e.pos == 0:
		stats['Detected'] = ((float(e.tp) / e.pos) * 100, '%3.2f%%')
	else:
		stats['Detected'] = ('N/A', '%s')

	# Recall and precision based on measures of ilids
	recall_bias = 0.5
	recall		= 0 if e.tp + e.fn == 0 else float(e.tp) / (e.tp + e.fn)
	precision	= 0 if e.tp + e.fp == 0 else float(e.tp) / (e.tp + e.fp)
	f1			= 0 if recall + recall_bias * precision == 0 else (recall_bias + 1) * recall * precision / (recall + recall_bias * precision)
	stats['Recall']    = (recall	* 100, '%3.2f%%')
	stats['Precision'] = (precision * 100, '%3.2f%%')
	stats['F1 score']  = (f1 * 100, '%3.2f%%')

	# old measure used for fall detection
	# if not e.tp == 0:
	#	  stats['Precision'] = (float(e.tp) / (e.tp + e.fp) * 100, '%3.2f%%')
	#	  stats['F1 score'] = (2 * float(e.tp) / (2 * e.tp + e.fp + e.fn) * 100,
	# else:
	#	  stats['Precision'] = ('N/A', '%s')
	#	  stats['F1 score'] = ('N/A', '%s')

	# Video statistics
	if video is not None:
		stats['Video related'] = ('-' * 20, '%s')
		stats['Video duration'] = (v.duration, '%s')
		stats['False alarm rate'] = (9999 if v.duration.milis == 0 else float(e.fp * 60 * 60 * 1000) /
									 v.duration.milis, '%.3f alarms/hour')

	return stats


def format_report(stats):
	"""Display the statistics"""

	# Search for length of longest string
	maxlen = max([len(name) for name in stats])

	# Prepare resulting string
	result = ''

	# Iterate over each statistics
	for name in stats:

		# Extract value and format
		(val, fmt) = stats[name]

		# Add it to resulting string
		result += ('%-' + str(maxlen) + 's : ' + fmt + '\n') % (name, val)

	return result


def video_thumbnail(video, path):
	if is_tool('avconv'):
		cmd = 'avconv'
	else:
		cmd = 'ffmpeg'

	subprocess.call([cmd, '-i', str(video), '-frames:v', '1',
					 os.path.join(path, 'thumbnail.jpg'), '-y'],
					 stderr=subprocess.PIPE)


def generate_html(stats, logs, data, out='out', filename='report.html'):
	""" Generate an HTML report """

	# Get data
	events, truths		 = data
	log_event, log_truth = logs

	# Create HEAD and BODY
	head = HEAD(TITLE('Report'))
	head <= SCRIPT(src='http://code.jquery.com/jquery-1.11.0.min.js')
	body = BODY(H1('Report'))

	# Parameters section
	body <= H2('Parameters')

	# Add Command line arguments
	body <= H3('Command-line arguments')
	table = TABLE(border=1, style='border-collapse: collapse;')
	table <= TR(TH('Name') + TH('Value'), style='background: lightgray;')
	for arg in sorted(vars(args)):
		row = TR()
		row <= TD(B(arg))
		row <= TD(vars(args)[arg])
		table <= row
	body <= table

	# Add other informations
	body <= H3('Other informations')
	table = TABLE(border=1, style='border-collapse: collapse;')
	table <= TR(TH('Name') + TH('Value'), style='background: lightgray;')
	table <= TR(TD(B('Date')) + TD(strftime("%Y-%m-%d %H:%M:%S", localtime())))
	table <= TR(TD(B('Script Version')) + TD(vplib.__version__))
	table <= TR(TD(B('System info')) + TD(platform()))
	table <= TR(TD(B('Links')) + TD(A("dir", href="..") + ", " + A("log", href="../markus.log")))
	body <= table

	# Statisticts section
	body <= H2('Statistics')
	body <= PRE(CODE(format_report(stats)),
				style='background: lightgray; padding: 5px;')

	# Events section
	body <= H2('Events')
	body <= INPUT(type='button',
				  value='Hide thumbnails',
				  onclick="$('.images').css('display', 'none');") + \
		INPUT(type='button',
			  value='Show thumbnails',
			  onclick="$('.images').css('display', 'block');") + P()
	table = TABLE(border=1, style='border-collapse: collapse;')
	table <= TR(TH('ID') + (TH('Thumbnail') if args.images else '') + TH('Time') + TH('Matched'),
				style='background: lightgray;')
	for (event, matches, good, evt_dup) in log_event:

		if evt_dup:
			bg = "#FFF4D3"
		elif good:
			bg = "#DCFFD3"
		else:
			bg = "#FFD4D3"

		row = TR(id="event%d" % event.id, style='background: ' + bg + ';')
		if event.image_file:
			row <= TD(A(B(event.id), href=event.image_file))
			row <= TD(CENTER(IMG(src=event.image_file, width='100', CLASS='images')))
		else:
			row <= TD(B(event.id))
		row <= TD(event.begin)
		cell = TD()
		comma = ""
		for match in matches:
			cell <= comma
			cell <= A(str(match.id), href="./images/truth_" + str(match.id) + ".jpg")
			comma = ', '
		row <= cell
		table <= row
	body <= table

	# Ground truth
	body <= H2('Ground truth')
	table = TABLE(border=1, style='border-collapse: collapse;')
	header = TR(style='background: lightgray;')
	header <= TH('ID')
	if args.images:
		header <= TH('Thumbnail')
	header <= TH('Matched')
	header <= TH('Begin')
	header <= TH('End')
	header <= TH('Match begin')
	header <= TH('Match end')
	header <= TH('Annotation')
	table <= header
	for (truth, matches, good) in log_truth:

		if truth.is_ambiguous:
			bg = "#FFF4D3"
		elif not matches:
			bg = "#FFD4D3"
		else:
			bg = "#DCFFD3"

		row = TR(style='background: ' + bg + ';')
		if args.images:
			row <= TD(A(B(truth.id), href="./images/truth_" + str(truth.id) + ".jpg"))
			row <= TD(CENTER(IMG(src="./images/truth_" + str(truth.id) + ".jpg", width='100', CLASS='images')))
		else:
			row <= TD(B(truth.id))
		cell = TD()
		comma = ''
		for match in matches:
			cell <= comma
			cell <= A(str(match.id), href="#event%d" % match.id)
			comma = ', '
		row <= cell
		row <= TD(truth.begin, style='padding-left: 20px; padding-right: 20px')
		row <= TD(truth.end, style='padding-left: 20px; padding-right: 20px')
		row <= TD(truth.match_begin, style='padding-left: 20px; padding-right: 20px')
		row <= TD(truth.match_end, style='padding-left: 20px; padding-right: 20px')
		row <= TD(truth.text, style='padding-left: 20px; padding-right: 20px')
		table <= row
	body <= table

	# Ensure directory exists
	if not os.path.exists(out):
		os.mkdir(out)

	# Get file path
	path = os.path.join(out, filename)

	# Write the file
	with open(path, 'w') as f:
		f.write(str(HTML(head + body)))

	# Open report in browser
	if not args.no_browser:
		try:
			subprocess.call(['xdg-open', os.path.abspath(path)])
		except OSError:
			print('Please open a browser on: ' + path)


def arguments_parser():
	""" Define the parser and parse arguments """

	# Main parser
	parser = argparse.ArgumentParser(description='Evaluation of videos events '
									 'regarding to the ground truth',
									 version=vplib.__version__)

	# Events file
	parser.add_argument('EVENT_FILE',
						type=str,
						help='the file containing the events')

	# Ground truth file
	parser.add_argument('TRUTH_FILE',
						type=str,
						help='the ".srt" file containing the ground truth')

	# Video file
	parser.add_argument('-V',
						dest='VIDEO_FILE',
						type=str,
						default=None,
						help='the video file')

	# Event name
	parser.add_argument('-e',
						dest='EVENT_NAME',
						type=str,
						default="anor",
						help='the name of the event to match')

	# Event name
	parser.add_argument('-a',
						dest='AMBIGUOUS',
						type=str,
						default="",
						help='the name of ambiguous events: not accounted in statistics')


	# Delay
	parser.add_argument('-d',
						dest='delay',
						default=0,
						type=int,
						help='the delay to use, default=0s.')

	# Uncompromising
	parser.add_argument('-u',
						dest='uncompromising',
						action='store_true',
						help='uncompromising: don\'t accept event before '
						'delay')

	# Tolerance
	parser.add_argument('-t',
						dest='tolerance',
						default=3,
						type=int,
						help='tolerance time (e.g. falling time), default=3s.')

	# Images
	parser.add_argument('-i',
						dest='images',
						action='store_true',
						help='extract images of events')

	# Output
	parser.add_argument('-o',
						dest='output',
						default='out',
						type=str,
						help='change output folder, default=out')

	# HTML output
	parser.add_argument('--html',
						action='store_true',
						help='output HTML report')

	# No browser
	parser.add_argument('--no-browser',
						action='store_true',
						help='don\'t try to open the browser')

	# Do not run markus again, only reevaluate results
	parser.add_argument('--results-only',
						action='store_true',
						help='recalculate the results only, do not run markus, should only be used on a pre-existant result')

	# Tolerance
	parser.add_argument('--start-timestamp',
						dest='start_timestamp',
						default=-1,
						type=int,
						help='The start time stamp of the video. Some videos might start with a time stamp that is given by the recording time. By default, detect from the video file name.')

	return parser.parse_args()


def main():
	"""Main function, do the job"""

	# Global scope for args
	global args

	# Parse the arguments
	args = arguments_parser()

	# Get video info
	video = video_info(args.VIDEO_FILE)

	# Get the array of events
	events = read_events(args.EVENT_FILE, video.start_timestamp if video else 0)

	# Get the array of truths
	truths = read_truths(args.TRUTH_FILE)

	# Evaluate the events regarding to the ground truth
	evaluation, logs = evaluate(events, truths)

	# Extract images
	if args.images:
		extract_images(events, truths, args.VIDEO_FILE, out=args.output)
		video_thumbnail(args.VIDEO_FILE, args.output)

	# Compute the statistics on the evaluation
	stats = statistics(evaluation, video)

	# Save stats
	pickle.dump(evaluation, open(os.path.join(args.output, "evals.pkl"), 'w'))
	pickle.dump(stats, open(os.path.join(args.output, "stats.pkl"), 'w'))

	# If an HTML report is desired
	if args.html:
		generate_html(stats, logs, (events, truths), out=args.output)
	# Otherwise
	else:
		# Print the report
		print(format_report(stats))


if __name__ == "__main__":
	main()
