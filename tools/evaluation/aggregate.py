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
from time import strftime, localtime
from platform import platform
from vplib.time import Time
from vplib.HTMLTags import *
from analyse_events import Evaluation, Video, statistics


def list_analyses(path, dirname='analysis', checkname='evals.pkl'):
    """ Return the list of evaluations made successfully """
    # Get all folders
    evals = os.listdir(path)

    # Filter
    is_valid = lambda x: os.path.exists(os.path.join(path, x,
                                                     dirname, checkname))

    # Apply the filter
    return filter(is_valid, evals)


def read_analyses(path, analyses, dirname='analysis'):
    """ Read the evaluation results """
    # For each evaluation
    for a in analyses:
        evals_path = os.path.join(path, a, dirname, 'evals.pkl')
        stats_path = os.path.join(path, a, dirname, 'stats.pkl')
        evals = pickle.load(open(evals_path, 'r'))
        stats = pickle.load(open(stats_path, 'r'))
        yield (a, evals, stats)


def generate_html(path, datas, dirname='analysis', filename='report.html'):
    """ Generate an HTML report """
    # Prepare datas
    datas = sorted(list(datas))

    if len(datas) == 0:
        print('No results to aggregate')
        exit(1)

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

    # Compute total
    tot = dict()
    first = datas[0]
    for k in vars(first[1]):
        for _, e, _ in datas:
            if not k in tot:
                tot[k] = 0
            tot[k] += vars(e)[k]
    vid = Time()
    for _, _, s in datas:
        vid += s['Video duration'][0]
    total = statistics(Evaluation(**tot), Video(duration=vid))

    # Results section
    body <= H2('Results')
    table = TABLE(border=1, style='border-collapse: collapse;')
    row = TR(style='background: lightgray;')
    row <= TH('Statistics')
    for col in datas:
        row <= TH(A(B(col[0].split('_')[0]), href=os.path.join(col[0], dirname, filename)))
    row <= TH('Overall')
    table <= row
    row = TR()
    row <= TD('')
    for col in datas:
        row <= TD(IMG(src=os.path.join(col[0], dirname, 'thumbnail.jpg'),
                      width=120))
    row <= TD('')
    table <= row
    for k in first[2]:
        row = TR()
        row <= TD(B(k))
        for _, _, s in datas:
            v, f = s[k]
            row <= TD(f % v)
        if k in total:
            v, f = total[k]
            row <= TD(f % v)
        else:
            row <= TD('N/A')

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
    datas = read_analyses(rpath, analyses)

    # Generate html report
    generate_html(rpath, datas)


if __name__ == '__main__':
    main()
