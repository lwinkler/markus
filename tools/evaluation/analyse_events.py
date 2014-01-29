#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Evaluation of videos events regarding to the ground truth

This python module evaluate a file containing video events in comparison with
the ground truth of the same video.

Example:
    To analyse an events file with its ground truth:
        $ python ./analyse_events.py events_file.evts ground_truth.srt

Created 2014-01-29 Fabien Dubosson
"""

import argparse
from pprint import pprint
from vplib.time import Time
from vplib.parser import subrip
from vplib.parser import evtfiles
from collections import namedtuple

# Evaluation metrics
Evaluation = namedtuple('Evaluation',
                        'total '
                        'truepos '
                        'falsepos '
                        'falseneg ')

# Truth tuple
Truth = namedtuple('Truth', 'begin end match_begin match_end is_fall')

# Global arguments, will be overwriten an runtime
args = None


def isFall(text):
    """ Function to check if a subtitle is a fall """
    if text[0:4] == 'anor':
        return True
    elif text[0:4] == 'norm':
        return False
    else:
        return None


def match_times(begin, end):
    # If uncompromising, impose the begin time to be at least d seconds after
    if args.u:
        m_begin = begin + Time(seconds=args.d) - Time(seconds=args.t)
    # Otherwise, accept alert arriving before delay
    else:
        m_begin = begin - Time(seconds=args.t)

    # End time adjustements
    m_end = end + Time(seconds=args.d) + Time(seconds=args.t)

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
        fall = isFall(entry.text)

        # If is is garbage skip
        if not fall or fall is None:
            continue

        # Match times
        match_begin, match_end = match_times(entry.begin, entry.end)

        # Otherwise add to the truths array
        truths.append(Truth(begin=entry.begin,
                            end=entry.end,
                            match_begin=match_begin,
                            match_end=match_end,
                            is_fall=fall))

    return truths


def evaluate(events, truths):
    """Evaluate the events on the ground truth"""

    # Matched
    matched = [False] * len(truths)

    # Stats
    total = 0
    tp = 0
    fp = 0
    fn = 0

    # For each event
    for event in events:

        # Increase total counter
        total += 1

        good = False

        # Search for a matching truth
        for i, truth in enumerate(truths):

            # Test for matching
            if event.time >= truth.match_begin and \
               event.time <= truth.match_end:
                good = True
                matched[i] = True
                break

        if good:
            tp += 1
        else:
            fp += 1

    fn = len(filter(lambda x: x, matched))

    return Evaluation(truepos=tp,
                      falseneg=fn,
                      falsepos=fp,
                      total=total)


def statistics(results):
    """Compute the statistics on the evaluation"""
    # Prepare the statistic dictionnary
    stats = {}

    stats['total'] = results.total
    stats['p_det'] = (results.truepos / results.total) * 100.0
    stats['p_fa'] = (results.falsepos / results.total) * 100.0
    stats['p_mis'] = (results.falseneg / results.total) * 100.0

    return stats


def format_report(stats):
    """Display the statistics"""

    fmt = ''
    fmt += 'Total events  : %(total)d\n'
    fmt += 'Detection     : %(p_det)3.2f%%\n'
    fmt += 'False alarm   : %(p_fa)3.2f%%\n'
    fmt += 'Missdetection : %(p_mis)3.2f%%'

    return fmt % stats


def arguments_parser():
    """ Define the parser and parse arguments """

    # Main parser
    parser = argparse.ArgumentParser(description='Evaluation of videos events '
                                     'regarding to the ground truth')

    # Events file
    parser.add_argument('EVENT_FILE',
                        type=str,
                        help='the file containing the events')

    # Ground truth file
    parser.add_argument('TRUTH_FILE',
                        type=str,
                        help='the ".srt" file containing the ground truth')

    # Delay
    parser.add_argument('-d',
                        default=10,
                        type=int,
                        help='the delay to use, default=10s.')

    # Uncompromising
    parser.add_argument('-u',
                        action='store_true',
                        help='uncompromising: Don\'t accept event before '
                        'delay')

    # Tolerance
    parser.add_argument('-t',
                        default=1,
                        type=int,
                        help='tolerance time (for falling time), default=1s.')

    return parser.parse_args()


def main():
    """Main function, do the job"""

    # Parse the arguments
    global args
    args = arguments_parser()

    # Get the array of events
    events = read_events(args.EVENT_FILE)

    # Get the array of truths
    truths = read_truths(args.TRUTH_FILE)

    # Evaluate the events regarding to the ground truth
    evaluation = evaluate(events, truths)

    # Compute the statistics on the evaluation
    stats = statistics(evaluation)

    # Print the report
    print(format_report(stats))


if __name__ == "__main__":
    main()
