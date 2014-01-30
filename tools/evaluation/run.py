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

    def run(self):
        self._run_markus()
        self._copy_srt()

    def _run_markus(self):
        # Prepare the command
        cmd = [abs_markus,
               self.abs_project,
               '-nc',
               '-pInput.class=VideoFileReader',
               #'-pInput.file=' + self.abs_video,
               '-pInput.file=/home/fabien/dev/videoprotector/markus/in/input.mp4',
               '-o' + os.path.join(run_path, self.eval_name)]

        # Run the command
        markus = subprocess.Popen(cmd)
        markus.communicate()

    def _copy_srt(self):
        # Get base name
        base = os.path.splitext(os.path.basename(self.abs_video))[0]
        orig = os.path.splitext(self.abs_video)[0] + '.srt'
        dest = os.path.join(run_path, self.eval_name, base + '.srt')

        print(orig)
        print(dest)

        # Check if file exist
        if os.path.exists(orig):
            shutil.copy(orig, dest)

def run(test):
    test.run()


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

    # Video directory
    parser.add_argument('-o',
                        type=str,
                        default=None,
                        dest='output',
                        help='output directory')

    return parser.parse_args()


def main():
    # Define some global variables
    global args, orig_path, run_path, abs_markus

    # Parse arguments
    args = arguments_parser()

    # Register some paths
    orig_path = os.getcwd()
    abs_markus = os.path.abspath('../../markus')
    abs_videos_dir = os.path.abspath(args.VIDEO_DIR)

    # Run path
    if args.output is None:
        run_path = 'run_' + strftime('%Y_%m_%d_%H_%M_%S', localtime())
    else:
        run_path = args.output

    # Verify run path and create dir
    if os.path.exists(run_path):
        print('Run path already exists')
        exit(1)
    else:
        os.makedirs(run_path)

    # Get the files
    video_names = ['0081_20130821_AxisM3204_P1_anormal_50.mp4']
    video_files = [os.path.join(abs_videos_dir, v) for v in video_names]

    # Prepare the evaluations
    evals = [Evaluation(args.PROJECT_FILE, v) for v in video_files]

    # Define a pool of workers
    pool = Pool(8)
    pool.map(run, evals)


if __name__ == "__main__":
    main()
