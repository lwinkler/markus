#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
from time import strftime, localtime
from platform import platform
from vplib.time import Time
from vplib.HTMLTags import *
from vplib.parser import subrip
from vplib.parser import evtfiles # TODO: evtfile should probably be replaced by subrip
from collections import namedtuple, OrderedDict

# Evaluation metrics
Evaluation = namedtuple('Evaluation',
                        'det '
                        'pos '
                        'tp '
                        'fp '
                        'fn '
                        'dups ')

# Video metrics
Video = namedtuple('Video', 'duration')

# Truth tuple
Truth = namedtuple('Truth', 'id begin end match_begin match_end is_valid')

# Global arguments, will be overwritten at runtime
args = None


def match_times(begin, end):
    """ Return the time which must be used to check if the event occurs """
    # If uncompromising, impose the begin time to be at least d seconds after
    if args.uncompromising:
        m_begin = begin + Time(seconds=args.delay) - \
            Time(seconds=args.tolerance)
    # Otherwise, accept alert arriving before delay
    else:
        m_begin = begin - Time(seconds=args.tolerance)

    # End time adjustements
    m_end = end + Time(seconds=args.delay) + Time(seconds=args.tolerance)

    # Return matching times
    return m_begin, m_end


def read_events(file_path):
    """Read the events file"""
    return evtfiles.parse(file_path)


def read_truths(file_path):
    """Read the ground truth file"""

    # Read the file
    entries = subrip.parse(file_path)

    # Prepare the truths array
    truths = []

    # Parse each subtitle
    for entry in entries:

        # Look if this is a fall
        fall = is_valid(entry.text)

        # If is is garbage skip
        if not fall or fall is None:
            continue

        # Match times
        match_begin, match_end = match_times(entry.begin, entry.end)

        # Otherwise add to the truths array
        truths.append(Truth(id=entry.number,
                            begin=entry.begin,
                            end=entry.end,
                            match_begin=match_begin,
                            match_end=match_end,
                            is_valid=fall))

    return truths


def evaluate(events, truths):
    """Evaluate the events on the ground truth"""

    # We use text files to store false positives, false negatives and true positives
    # TODO: In the future the generated files should respect an existing format. E.g. *.ass or *.csv or pickle
    fid_fp = open(args.output + '/FP.txt','w')
    fid_fn = open(args.output + '/FN.txt','w')
    fid_tp = open(args.output + '/TP.txt','w')
    
    # Variable to log event
    log_event = []
    log_truth = []

    # Matched
    matched_events = dict()
    matched_truths = dict()

    # Stats
    fp = 0
    fn = 0
    dups = 0
    inhib = 0

    # For each event
    for event in events:
        # Search for a matching truth
        for i, truth in enumerate(truths):

            # Test for matching
            if event.time >= truth.match_begin and \
               event.time <= truth.match_end:
                # Keep track of matched ground truth
                if event.id not in matched_events:
                    matched_events[event.id] = []
                else:
                    dups += 1 # several events for one gt
                if truth.id not in matched_truths:
                    matched_truths[truth.id] = []
                else:
                    inhib += 1 # several gt for one event
                matched_events[event.id].append(truth)
                matched_truths[truth.id].append(event)

        # Log event
        log_event.append((event, matched_events[event.id] if event.id in matched_events else [], event.id in matched_events))

        if not event.id in matched_events:
            fp += 1
            fid_fp.write('%s %s\n' % (event.time.milis, event.time_end.milis))

    for truth in truths:
        log_truth.append((truth, matched_truths[truth.id] if truth.id in matched_truths else [], truth.id in matched_truths))
        if not truth.id in matched_truths:
	    fid_fn.write('%s %s\n' % (truth.begin.milis, truth.end.milis))
	    fn += 1

    tp = len(matched_truths)
    print "fp %d fn %d tp %d. dups %d inh %d" % (fp, fn, tp, dups, inhib)

    # Prepare evaluation results
    results = Evaluation(tp=tp,
                         fp=fp,
                         fn=fn,
                         dups=dups,
                         det=len(events),
                         pos=len(truths))
    fid_fp.close()
    fid_fn.close()
    fid_tp.close()

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
    stats['Total ground truth'] = (e.pos, '%2d')
    stats['Total detected'] = (e.det, '%2d')
    stats['Total correct detections'] = (e.tp, '%2d')
    stats['Total false alarms'] = (e.fp, '%2d')
    stats['Total missed'] = (e.fn, '%2d')
    stats['Total duplicates'] = (e.dups, '%2d')

    # Confusion matrix statistics
    stats['Statistics'] = ('-' * 20, '%s')
    if not e.pos == 0:
        stats['Detected'] = ((float(e.tp) / e.pos) * 100, '%3.2f%%')
    else:
        stats['Detected'] = ('N/A', '%s')

    # Recall and precision based on measures of ilids
    recall_bias = 0.5
    recall      = float(e.tp) / (e.tp + e.fn)
    precision   = float(e.tp) / (e.tp + e.fp)
    f1          = (recall_bias + 1) * recall * precision / (recall + recall_bias * precision)
    stats['Recall']    = (recall    * 100, '%3.2f%%')
    stats['Precision'] = (precision * 100, '%3.2f%%')
    stats['F1 score']  = (f1 * 100, '%3.2f%%')

    # old measure used for fall detection
    # if not e.tp == 0:
    #     stats['Precision'] = (float(e.tp) / (e.tp + e.fp) * 100, '%3.2f%%')
    #     stats['F1 score'] = (2 * float(e.tp) / (2 * e.tp + e.fp + e.fn) * 100,
    # else:
    #     stats['Precision'] = ('N/A', '%s')
    #     stats['F1 score'] = ('N/A', '%s')

    # Video statistics
    if video is not None:
        stats['Video related'] = ('-' * 20, '%s')
        stats['Video duration'] = (v.duration, '%s')
        stats['False alarm rate'] = (float(e.fp * 60 * 60 * 1000) /
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

    # Delay
    parser.add_argument('-r',
                        dest='ratio',
                        default=1,
                        type=float,
                        help='the time ratio to apply')

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

        b_stamp = "%s" % entry.begin
        b_time = Time(text=b_stamp, sep_ms=',')
        b_time.milis *= args.ratio
        e_stamp = "%s" % entry.end
        e_time = Time(text=e_stamp, sep_ms=',')
        e_time.milis *= args.ratio
        f.write('%d\n%s --> %s\n' % (entry.number, b_time, e_time))
        f.write('%s\n' % entry.text)

    f.close()


if __name__ == "__main__":
    main()
