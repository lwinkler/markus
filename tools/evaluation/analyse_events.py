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
from vplib.parser import subrip
from vplib.parser import evtfiles


def read_events(file_path):
    """Read the events file"""
    events = evtfiles.parse(file_path)
    return events


def read_truths(file_path):
    """Read the ground truth file"""
    entries = subrip.parse(file_path)
    return entries


def evaluate(events, truths):
    """Evaluate the events on the ground truth"""
    pass


def statistics(results):
    """Display the statistics"""
    pass


def format_report(stats):
    """Display the statistics"""
    pass


def arguments_parser():
    """ Define the parser and parse arguments """

    # Main parser
    parser = argparse.ArgumentParser(description='Evaluation of videos events '
                                     'regarding to the ground truth')

    # Events file
    parser.add_argument('EVENT_FILE',
                        help='the file containing the events')

    # Ground truth file
    parser.add_argument('TRUTH_FILE',
                        help='the ".srt" file containing the ground truth')

    return parser.parse_args()


def main():
    """Main function, do the job"""

    # Parse the arguments
    args = arguments_parser()
    assert args.EVENT_FILE
    assert args.TRUTH_FILE

    # Get the array of events
    events = read_events(args.EVENT_FILE)
    assert events, "There is no events parsed"
    pprint(events)

    # Get the array of truths
    truths = read_truths(args.TRUTH_FILE)
    assert truths, "There is no ground truths parsed"
    pprint(truths)

    # Evaluate both arrays
    results = evaluate(events, truths)

    # Compute the statistics
    stats = statistics(results)

    # Print the report
    print(format_report(stats))


if __name__ == "__main__":
    main()
