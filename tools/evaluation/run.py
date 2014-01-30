#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Created 2014-01-30 Fabien Dubosson
"""

import os
import shutil
import argparse
import subprocess
from multiprocessing import Pool
from time import strftime, localtime

# Absolute path to markus
abs_markus = None

# Absolute path to analyse_event.py
abs_analyse = None

# Original path
orig_path = None

# Run path
run_path = None

# Arguments
args = None


class Evaluation():
    """
    An evaluation of a specific project on a specific video
    """
    def __init__(self, project, video):
        self.abs_project = os.path.abspath(project)
        self.abs_video = os.path.abspath(video)
        self.eval_name = os.path.basename(video).split('_')[0]
        self.eval_path = os.path.join(run_path, self.eval_name)

    def run(self):
        self._run_markus()
        self._copy_srt()
        self._run_analyse()

    def _run_markus(self):
        # Prepare the command
        cmd = [abs_markus,
               self.abs_project,
               '-nc',
               '-pInput.class=VideoFileReader',
               #'-pInput.file=' + self.abs_video, # TODO
               '-pInput.file=/home/fabien/dev/videoprotector/markus/in/input.mp4',
               '-pFileterPython9.script=../../modules2/FilterPython/analyse.py',
               '-o' + self.eval_path]

        # Run the command
        markus = subprocess.Popen(cmd)
        markus.communicate()

    def _copy_srt(self):
        # Get base name
        orig = os.path.splitext(self.abs_video)[0] + '.srt'
        dest = os.path.join(self.eval_path, 'ground_truth.srt')

        # Check if file exist
        if os.path.exists(orig):
            shutil.copy(orig, dest)

    def _run_analyse(self):
        # Python 3 hack
        if args.python3:
            cmd = ['python2']

        # Prepare the command
        cmd += [abs_analyse,
                os.path.join(self.eval_path, 'event.srt'),
                os.path.join(self.eval_path, 'ground_truth.srt'),
                '-V', self.abs_video,
                '-o', os.path.join(self.eval_path, 'analysis'),
                '--html',
                '-i']

        analyse = subprocess.Popen(cmd)
        analyse.communicate()


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
                        type=str,
                        default='sets/all.txt',
                        dest='set',
                        help='the file listing videos that must be used')

    # Output directory
    parser.add_argument('-o',
                        type=str,
                        default=None,
                        dest='output',
                        help='output directory')

    # Python3 hack
    parser.add_argument('-p',
                        dest='python3',
                        action='store_true',
                        help='hack for python3 distributions')

    return parser.parse_args()


# Dummy function needed to parallelize processes
def run(test):
    test.run()


def main():
    # Define some global variables
    global args, orig_path, run_path, abs_analyse, abs_markus

    # Parse arguments
    args = arguments_parser()

    # Register some paths
    orig_path = os.getcwd()
    abs_analyse = os.path.abspath('./analyse_events.py')
    abs_markus = os.path.abspath('../../markus')
    abs_videos_dir = os.path.abspath(args.VIDEO_DIR)

    # Define a run path
    if args.output is None:
        run_path = 'run_' + strftime('%Y_%m_%d_%H_%M_%S', localtime())
    else:
        # If it is precised by the user, use it
        run_path = args.output

    # Verify run path and create dir
    if os.path.exists(run_path):
        print('Run path already exists')
        exit(1)
    else:
        os.makedirs(run_path)

    # Get the video files
    with open(args.set, 'r') as f:
        video_names = f.read().splitlines()
    video_files = [os.path.join(abs_videos_dir, v) for v in video_names]

    # Prepare the evaluations
    evals = [Evaluation(args.PROJECT_FILE, v) for v in video_files]

    # Define a pool of workers
    pool = Pool(8)

    # Run the evaluations in parallel
    pool.map(run, evals)


if __name__ == "__main__":
    main()
