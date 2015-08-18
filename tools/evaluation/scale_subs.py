#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Evaluation of a video events regarding to its ground truth
Scale a subtitle by a time ratio

Example:
	To analyse an events file with its ground truth:
		$ python ./analyse_events.py eventsIn.srt eventsOut.srt -r 1.5

Created Jan 2015 Laurent Winkler
"""

import re
import os
import vplib
import argparse
from time import strftime, localtime
from platform import platform
from vplib.time import Time
from vplib.parser import subrip
from collections import namedtuple, OrderedDict

# Global arguments, will be overwritten at runtime
args = None


def arguments_parser():
	""" Define the parser and parse arguments """

	# Main parser
	parser = argparse.ArgumentParser(description='Evaluation of videos events '
									 'regarding to the ground truth',
									 version=vplib.__version__)

	# Events file
	parser.add_argument('EVENT_FILE_IN',
						type=str,
						help='input .srt file')

	# Ground truth file
	parser.add_argument('EVENT_FILE_OUT',
						type=str,
						help='output .srt file')

	# Scaling ratio
	parser.add_argument('-r',
						dest='ratio',
						default=1,
						type=float,
						help='the time ratio to apply')

	# Delay
	parser.add_argument('-d',
						dest='delay',
						default=0,
						type=float,
						help='add a delay to the subtitles in seconds, before scaling')

	return parser.parse_args()


def main():
	"""Main function, do the job"""

	# Global scope for args
	global args

	# Parse the arguments
	args = arguments_parser()

	# Read input file
	entries = subrip.parse(args.EVENT_FILE_IN)
	f = open(args.EVENT_FILE_OUT, 'w')

	for entry in entries:
		# Scale the time of each subtitle
		b_stamp = "%s" % entry.begin
		b_time = Time(text=b_stamp, sep_ms=',')
		b_time.milis += args.delay * 1000
		b_time.milis *= args.ratio
		e_stamp = "%s" % entry.end
		e_time = Time(text=e_stamp, sep_ms=',')
		e_time.milis += args.delay * 1000
		e_time.milis *= args.ratio
		f.write('%d\n%s --> %s\n' % (entry.number, b_time, e_time))
		f.write('%s\n' % entry.text)

	f.close()


if __name__ == "__main__":
	main()
