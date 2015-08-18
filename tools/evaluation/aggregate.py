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
    if args.LIST_FILE:
        evals = []
        with open (args.LIST_FILE, "r") as myfile:
            while 1:
                line = myfile.readline()
                if not line:break
                evals += [line.replace('\n', '')]
    else:
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


def generate_html(path, datas, dirname='analysis', filename='summary.html'):
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
    	if 'Video duration' in s:
		vid += s['Video duration'][0]
    total = statistics(Evaluation(**tot), Video(duration=vid))

    '''
    # Results section (alternative code: vertical, not fully working)
    # header
    body <= H2('Results')
    table = TABLE(border=1, style='border-collapse: collapse;')
    row = TR(style='background: lightgray;')
    row <= TH('Statistics')
    for k in first[2]:
        row <= TH(k)
    table <= row

    # Totals
    row = TH('Overall')
    row <= TD()
    for k in first[2]:
        if k in total:
            v, f = total[k]
        if f == '%s' and str(v) == '-' * 20:
            row <= TD("")
        else:
	    row <= TD(f % v)
    table <= row

    # Detail
    for col, _, s in datas:
        row = TR()
        row <= TH(A(B(col.split('_')[0]), href=os.path.join(col, dirname, 'report.html')))
        row <= TD(IMG(src=os.path.join(col, dirname, 'thumbnail.jpg'), width=60))
        for k in first[2]:
            # row <= TD(B(k))
            v, f = s[k]
            if f == '%s' and str(v) == '-' * 20:
                row <= TD("")
            else:
                row <= TD(f % v)
        table <= row
        '''

    # Results section
    body <= H2('Results')
    table = TABLE(border=1, style='border-collapse: collapse;')
    row = TR(style='background: lightgray;')
    row <= TH('Statistics')
    row <= TH('Overall')
    for col in datas:
        row <= TH(A(B(col[0].split('_')[0]), href=os.path.join(col[0], dirname, 'report.html')))
    table <= row
    row = TR()
    row <= TD('')
    row <= TD('')
    for col in datas:
        row <= TD(IMG(src=os.path.join(col[0], dirname, 'thumbnail.jpg'),
                      width=120))
    table <= row

    for k in first[2]:
        row = TR()
        row <= TD(B(k))
        # Total
        if k in total:
            v, f = total[k]
            if f == '%s' and str(v) == '-' * 20:
                row <= TD('')
            else:
                row <= TD(f % v)
        else:
            row <= TD('N/A')
    	# Statistics of each video
        for _, _, s in datas:
            v, f = s[k]
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
    parser.add_argument('-f',
                        dest='LIST_FILE',
                        type=str,
                        default="",
                        help='use a simple text file to list the directories to analyse')

    # No browser
    parser.add_argument('-o',
                        dest='OUTPUT_FILE',
                        type=str,
                        default="summary.html",
                        help='use a simple text file to list the directories to analyse')

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
    generate_html(rpath, datas, filename=args.OUTPUT_FILE)


if __name__ == '__main__':
    main()
