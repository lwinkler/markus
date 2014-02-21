#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Run the evaluation of an algorithm on a set of videos.

Created 2014-01-30 Fabien Dubosson
"""

import os
import shutil
import argparse
import subprocess
from multiprocessing import Pool
from time import strftime, localtime

# Arguments
args = None

# Original path
abs_orig = None

# Absolute path to markus
abs_markus = None

# Run path
run_path = None


def is_tool(name):
    """ Check if a tool exists """
    try:
        devnull = open(os.devnull, 'w')
        subprocess.check_call(['which', name], stdout=devnull,
                              stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError:
        return False
    return True


def list_videos(path, ext=['.avi', '.mp4']):
    """ List all the videos names in a directory """
    files = os.listdir(path)
    names = [os.path.splitext(os.path.basename(f)) for f in files]
    videos = filter(lambda n: n[1] in ext, names)
    return [v[0] + v[1] for v in videos]


class Evaluation():
    """ An evaluation of a specific project on a specific video """
    def __init__(self, project, video):
        """ Initalize the evaluation parameters """
        self.abs_project = os.path.abspath(project)
        self.abs_video = os.path.abspath(video)
        self.eval_name = os.path.basename(video).split('_')[0]
        self.eval_path = os.path.join(run_path, self.eval_name)

    def run(self):
        """ Run the evaluation """
        # self._run_markus()
        self._copy_srt()
        self._run_analyse()

    def _run_markus(self):
        """ Run markus """
        # Prepare the command
        cmd = [abs_markus,
               self.abs_project,
               '-nc',
               '-pInput.class=VideoFileReader',
               '-pInput.file=' + self.abs_video,
               '-o' + self.eval_path]

        # Run the command
        markus = subprocess.Popen(cmd)
        markus.communicate()

    def _copy_srt(self):
        """ Copy the ground truth file """
        # Get base name
        orig = os.path.splitext(self.abs_video)[0] + '.srt'
        dest = os.path.join(self.eval_path, 'ground_truth.srt')

        # Check if file exist and copy it
        if os.path.exists(orig):
            shutil.copy(orig, dest)

    def _run_analyse(self):
        """ Run the analysis of detected events """
        # Hack for Python 3 distros
        if is_tool('python2'):
            cmd = ['python2']
        else:
            cmd = []

        # Prepare the command
        cmd += [os.path.join(abs_orig, 'analyse_events.py'),
                os.path.join(self.eval_path, 'event.srt'),
                os.path.join(self.eval_path, 'ground_truth.srt'),
                '-V', self.abs_video,
                '-o', os.path.join(self.eval_path, 'analysis'),
                '-d', str(args.delay),
                '-t', str(args.tolerance),
                '-i',
                '--html',
                '--no-browser']

        if args.uncompromising:
            cmd += ['-u']

        # Run the command
        analyse = subprocess.Popen(cmd)
        analyse.communicate()


def run_aggregate(run_path):
    """ Aggregate the results of all evaluation sets """
    # Hack for Python 3 distros
    if is_tool('python2'):
        cmd = ['python2']
    else:
        cmd = []

    # Prepare the command
    cmd += [os.path.join(abs_orig, 'aggregate.py'), run_path]

    if args.no_browser:
        cmd += ['--no-browser']

    # Run the command
    aggregate = subprocess.Popen(cmd)
    aggregate.communicate()


def arguments_parser():
    """ Define the parser and parse arguments """

    # Main parser
    parser = argparse.ArgumentParser(description='Evaluation of videos events '
                                     'regarding to the ground truth')

    # Project file
    parser.add_argument('PROJECT_FILE',
                        type=str,
                        help='the markus project to test')

    # Video directory
    parser.add_argument('VIDEO_DIR',
                        type=str,
                        help='the path to the videos directory')

    # set file
    parser.add_argument('-s',
                        dest='set',
                        default=None,
                        type=str,
                        help='the file listing videos that must be used')

    # Output directory
    parser.add_argument('-o',
                        dest='output',
                        default=None,
                        type=str,
                        help='output directory')

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

    # No browser
    parser.add_argument('--no-browser',
                        action='store_true',
                        help='don\' try to open the browser')

    return parser.parse_args()


def run(test):
    """ Dummy function needed to parallelize processes """
    test.run()


def main():
    """ Main function, do the job """
    # Specify global variables
    global args
    global abs_orig, abs_markus
    global run_path

    # Parse arguments
    args = arguments_parser()

    # Register some paths
    abs_orig = os.path.abspath(os.path.dirname(__file__))
    abs_markus = os.path.abspath(os.path.join(abs_orig, '../../markus'))
    abs_videos_dir = os.path.abspath(args.VIDEO_DIR)

    # Define a run path
    if args.output is None:
        # Based on date-time if not specified
        run_path = 'run_' + strftime('%Y_%m_%d_%H_%M_%S', localtime())
    else:
        # If it is precised by the user, use it
        run_path = args.output

    # Verify the run path and create the dir
    if os.path.exists(run_path):
        print('The run path already exists')
        # exit(1)
    else:
        os.makedirs(run_path)

    # Get the video names
    if args.set is not None:
        # If specified, use the file list
        with open(args.set, 'r') as f:
            video_names = f.read().splitlines()
    else:
        # Otherwise use all videos from the videos folder
        video_names = list_videos(abs_videos_dir)

    # transform video to filepaths
    video_files = [os.path.join(abs_videos_dir, v) for v in video_names]

    # Prepare the evaluations
    evals = [Evaluation(args.PROJECT_FILE, v) for v in video_files]

    # Run the evaluations in parallel
    pool = Pool()
    pool.map(run, evals)

    # Run the aggregation at the end
    run_aggregate(run_path)


if __name__ == "__main__":
    main()
