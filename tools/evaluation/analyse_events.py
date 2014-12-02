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
from vplib.parser import evtfiles
from collections import namedtuple, OrderedDict

# Evaluation metrics
Evaluation = namedtuple('Evaluation',
                        'det '
                        'pos '
                        'tp '
                        'fp '
                        'fn '
                        'dups ')

# Video metrics
Video = namedtuple('Video', 'duration')

# Truth tuple
Truth = namedtuple('Truth', 'id begin end match_begin match_end is_valid')

# Global arguments, will be overwriten an runtime
args = None


def is_tool(name):
    """ Check if a tool exists """
    try:
        devnull = open(os.devnull, 'w')
        subprocess.check_call(['which', name], stdout=devnull,
                              stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError:
        return False
    return True


def is_valid(text):
    """ Function to check if a subtitle is a fall """
    return args.EVENT_NAME in text
    # if text[0:4] == 'anor':
        # return True
    # elif text[0:4] == 'norm':
        # return False
    # else:
        # return None


def video_info(video):
    """ Function to get information about the video """
    # If no video
    if video is None:
        return None

    try:
        # Run avprobe to get info. Note: This replaces ffprobe 
        ff = subprocess.Popen(['avprobe', video], stderr=subprocess.PIPE)
        # Get back results
        ffout = str(ff.communicate())
        # Search the duration in the output
        dval = re.search('Duration: ([^,]*),', ffout)
        # Return the informations
        return Video(duration=Time(text=dval.group(1), sep_ms='.'))
    except subprocess.CalledProcessError:
        return None


def extract_images(events, truths, video, out='out'):
    """ Extract thubmnails images of events and ground truths """
    # If no video
    if video is None:
        return

    # Ensure directory is existing
    path = os.path.join(out, 'images')
    if not os.path.exists(path):
        os.makedirs(path)

    if is_tool('avconv'):
        cmd = 'avconv'
    else:
        cmd = 'ffmpeg'

    # The function to extract a precise image
    def extract(kind, time, name):
        subprocess.call([cmd, '-ss', str(time), '-i', str(video),
                         '-frames:v', '1',
                         os.path.join(path, str(kind) + '_' + str(name) +
                                      '.jpg'), '-y'], stderr=subprocess.PIPE)

    # Extract events
    for event in events:
        extract('event', event.time, event.id)

    # Extract ground truth
    for truth in truths:
        extract('truth', (truth.begin + truth.end) / 2, truth.id)


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

    # Variable to log event
    log = []

    # Matched
    matched = [False] * len(truths)

    # Stats
    total_det = 0
    tp = 0
    fp = 0
    fn = 0
    dups = 0

    # For each event
    for event in events:

        # Increase total counter
        total_det += 1

        good = False
        match_gt = None

        # Search for a matching truth
        for i, truth in enumerate(truths):

            # Test for matching
            if event.time >= truth.match_begin and \
               event.time <= truth.match_end:
                # Keep track of matched groun truth
                match_gt = truth

                # If the ground truth is not matched yet
                if not matched[i]:
                    good = True
                    matched[i] = True
                    break
                # Otherwise this is a duplicate
                else:
                    good = None
                    break

        # Log event
        log.append((event, match_gt, good))

        if good is None:
            dups += 1
        elif good:
            tp += 1
        else:
            fp += 1

    # Compute false negative
    fn = len(filter(lambda x: not x, matched))

    # Prepare evaluation results
    results = Evaluation(tp=tp,
                         fp=fp,
                         fn=fn,
                         dups=dups,
                         det=total_det,
                         pos=len(truths))

    return (results, log)


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

    if not e.tp == 0:
        stats['Precision'] = (float(e.tp) / (e.tp + e.fp) * 100, '%3.2f%%')
        stats['F1 score'] = (2 * float(e.tp) / (2 * e.tp + e.fp + e.fn) * 100,
                                '%3.2f%%')
    else:
        stats['Precision'] = ('N/A', '%s')
        stats['F1 score'] = ('N/A', '%s')

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

        # Extract value and format
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


def generate_html(stats, log, data, out='out', filename='report.html'):
    """ Generate an HTML report """

    # Get datas
    events, truths = data

    # Create HEAD and BODY
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
    table <= TR(TD(B('Date')) + TD(strftime("%Y-%m-%d %H:%M:%S",
                                            localtime())))
    table <= TR(TD(B('Script Version')) + TD(vplib.__version__))
    table <= TR(TD(B('System info')) + TD(platform()))
    body <= table

    # Statisticts section
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
    table <= TR(TH('ID') + TH('Thumbnail') + TH('Time') + TH('Matched'),
                style='background: lightgray;')
    for (event, truth, good) in log:

        if good is None:
            bg = "#FFF4D3"
        elif good:
            bg = "#DCFFD3"
        else:
            bg = "#FFD4D3"

        row = TR(style='background: ' + bg + ';')
        if args.images:
            row <= TD(A(B(event.id),
                        href="./images/event_" + str(event.id) + ".jpg"))
            row <= TD(CENTER(IMG(src="./images/event_" + str(event.id) +
                                 ".jpg",
                                 width='100',
                                 CLASS='images')))
        else:
            row <= TD(B(event.id))
        row <= TD(event.time)
        if truth is not None and args.images:
            row <= TD(A(truth.id,
                        href="./images/truth_" + str(truth.id) + ".jpg"))
        else:
            row <= TD(truth.id if truth is not None else '')
        table <= row
    body <= table

    # Ground truth
    body <= H2('Ground truth')
    table = TABLE(border=1, style='border-collapse: collapse;')
    header = TR(style='background: lightgray;')
    header <= TH('ID')
    header <= TH('Thumbnail')
    header <= TH('Matched')
    header <= TH('Begin')
    header <= TH('End')
    header <= TH('Match begin')
    header <= TH('Match end')
    table <= header
    for truth in truths:
        matches = [i.id for (i, t, g) in log if truth is not None and t is not
                   None and truth.id == t.id]

        if not matches:
            bg = "#FFD4D3"
        elif len(matches) == 1:
            bg = "#DCFFD3"
        else:
            bg = "#FFF4D3"

        row = TR(style='background: ' + bg + ';')
        if args.images:
            row <= TD(A(B(truth.id),
                        href="./images/truth_" + str(truth.id) + ".jpg"))
            row <= TD(CENTER(IMG(src="./images/truth_" + str(truth.id) +
                                 ".jpg",
                                 width='100',
                                 CLASS='images')))
        else:
            row <= TD(B(truth.id))
        cell = TD()
        comma = ''
        for match in matches:
            cell <= comma
            cell <= A(match, href="./images/event_" + str(match) + ".jpg")
            comma = ', '
        row <= cell
        row <= TD(truth.begin, style='padding-left: 20px; '
                  'padding-right: 20px')
        row <= TD(truth.end, style='padding-left: 20px; '
                  'padding-right: 20px')
        row <= TD(truth.match_begin, style='padding-left: 20px; '
                  'padding-right: 20px')
        row <= TD(truth.match_end, style='padding-left: 20px; '
                  'padding-right: 20px')
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

    # Delay
    parser.add_argument('-d',
                        dest='delay',
                        default=10,
                        type=int,
                        help='the delay to use, default=10s.')

    # Uncompromising
    parser.add_argument('-u',
                        dest='uncompromising',
                        action='store_true',
                        help='uncompromising: don\'t accept event before '
                        'delay')

    # Tolerance
    parser.add_argument('-t',
                        dest='tolerance',
                        default=1,
                        type=int,
                        help='tolerance time (e.g. falling time), default=1s.')

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
                        help='don\' try to open the browser')

    # Do not run markus again, only reevaluate results
    parser.add_argument('--results-only',
                        action='store_true',
                        help='recalculate the results only, do not run markus, should only be used on a pre-existant result')

    return parser.parse_args()


def main():
    """Main function, do the job"""

    # Global scope for args
    global args

    # Parse the arguments
    args = arguments_parser()

    # Get the array of events
    events = read_events(args.EVENT_FILE)

    # Get the array of truths
    truths = read_truths(args.TRUTH_FILE)

    # Evaluate the events regarding to the ground truth
    evaluation, log = evaluate(events, truths)

    # Get video info
    video = video_info(args.VIDEO_FILE)

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
        generate_html(stats, log, (events, truths), out=args.output)
    # Otherwise
    else:
        # Print the report
        print(format_report(stats))


if __name__ == "__main__":
    main()
