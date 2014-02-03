#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Aggregate results of several videos into one report.

Created 2014-02-03 Fabien Dubosson
"""

import os
import pickle
import vplib
import argparse
import subprocess
from pprint import pprint
from time import strftime, localtime
from platform import platform
from vplib.time import Time
from vplib.HTMLTags import *
from analyse_events import Evaluation, statistics


def list_analyses(path, dirname='analysis', filename='report.pkl'):
    """ Return the list of evaluations made successfully """
    # Get all folders
    evals = os.listdir(path)

    # Filter
    is_valid = lambda x: os.path.exists(os.path.join(path, x,
                                                     dirname, filename))

    # Apply the filter
    return filter(is_valid, evals)


def read_analyses(path, analyses, dirname='analysis', filename='report.pkl'):
    """ Read the evaluation results """

    # For each evaluation
    for a in analyses:
        report = os.path.join(path, a, dirname, filename)
        stats = pickle.load(open(report, 'r'))
        yield (a, stats)


def generate_html(path, evals, dirname='analysis', filename='report.html'):
    """ Generate an HTML report """

    # Create HEAD and BODY
    head = HEAD(TITLE('Report'))
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
    table <= TR(TD(B('Date')) + TD(strftime("%Y-%m-%d %H:%M:%S",
                                            localtime())))
    table <= TR(TD(B('Script Version')) + TD(vplib.__version__))
    table <= TR(TD(B('System info')) + TD(platform()))
    body <= table

    # Results section
    body <= H2('Results')
    table = TABLE(border=1, style='border-collapse: collapse;')
    evals = list(evals)
    cols = statistics(evals[0][1])
    row = TR(style='background: lightgray;')
    row <= TH('Video')
    for col in cols:
        row <= TH(col)
    table <= row
    for name, e in evals:
        row = TR()
        row <= TD(A(B(name), href=os.path.join(name, dirname, filename)))
        stats = statistics(e)
        for k in stats:
            v, f = stats[k]
            row <= TD(f % v)
        table <= row
    # Total
    row = TR(style='background: #D3D3FF;')
    row <= TD(B('Total'))
    stats = statistics(e)
    for k in stats:
        v, f = stats[k]
        row <= TD(f % v)
    table <= row
    body <= table

    # Get file path
    fpath = os.path.join(path, filename)

    # Write the file
    with open(fpath, 'w') as f:
        f.write(str(HTML(head + body)))

    # Open report in browser
    if not args.no_browser:
        try:
            subprocess.call(['xdg-open', os.path.abspath(fpath)])
        except OSError:
            print('Please open a browser on: ' + path)


def arguments_parser():
    """ Define the parser and parse arguments """

    # Main parser
    parser = argparse.ArgumentParser(description='Aggregate results on '
                                     'several videos')

    # Run path
    parser.add_argument('RUN_PATH',
                        type=str,
                        help='the run path of a finished evaluation')

    # No browser
    parser.add_argument('--no-browser',
                        action='store_true',
                        help='don\' try to open the browser')

    return parser.parse_args()


def main():
    """Main function, do the job"""

    # Global scope for args
    global args

    # Parse the arguments
    args = arguments_parser()
    rpath = args.RUN_PATH

    # Get the list of analyses
    analyses = list_analyses(rpath)

    # Read the evaluations of analyses
    evals = read_analyses(rpath, analyses)

    # Generate html report
    generate_html(rpath, evals)


if __name__ == '__main__':
    main()
