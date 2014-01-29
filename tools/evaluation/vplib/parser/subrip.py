# -*- coding: utf-8 -*-

"""
A SubRip parser

Created 2014-01-29 Fabien Dubosson
"""

import os
import sys
from vplib.time import Time
from collections import namedtuple

Subtitle = namedtuple('Subtitle', 'number begin end text')


def parse(file_path):
    """ Parse the given subrip file """

    # Verify the existence of the file
    if not os.path.isfile(file_path):
        return []

    # Read the file
    with open(file_path, 'r') as f:
        lines = f.readlines()

    # Parse the lines
    return _parse_lines(lines)


def _parse_lines(lines):
    """ Parse the given subrip lines """

    # Check input
    if len(lines) == 0:
        return False

    # Prepare results array
    entries = []

    # Parse lines
    while lines:

        # Get subtitle number
        nbr = int(lines.pop(0).strip())

        # Get subtitles timestamps
        b_stamp, e_stamp = lines.pop(0).strip().split('-->')
        b_time = Time(text=b_stamp)
        e_time = Time(text=e_stamp)

        # Get text
        text = ""
        line = lines.pop(0)
        while line.strip():
            text += line
            line = lines.pop(0)

        # Prepare entry
        entry = Subtitle(number=nbr,
                         begin=b_time,
                         end=e_time,
                         text=text)

        # Save entries
        entries.append(entry)

    return entries