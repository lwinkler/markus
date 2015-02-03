# -*- coding: utf-8 -*-

"""
An Event parser

Created 2014-01-29 Fabien Dubosson
"""

import os
import sys
from vplib.time import Time
from collections import namedtuple

Event = namedtuple('Event', 'id time time_end')


def parse(file_path):
    """ Parse the given events file """

    # Verify the existence of the file
    if not os.path.isfile(file_path):
        return []

    # Read the file
    with open(file_path, 'r') as f:
        lines = f.readlines()

    # Parse the lines
    return _parse_lines(lines)


def _parse_lines(lines):
    """ Parse the given events lines """

    # Check input
    if len(lines) == 0:
        return []

    # Prepare results array
    events = []

    # Parse lines
    for i, line in enumerate(lines):
        # If it is not a line containig an event time
        if '-->' not in line:
            continue

        # Get the first time text
        time_text, time_end = line.strip().split('-->')

        # Transform it to time
        time = Time(text=time_text, sep_ms=',')
        time_end = Time(text=time_end, sep_ms=',')
        # Changed this line to extract ident from the timestamp of the subtitle
        # ident = int(lines[i + 3].strip().split('\t')[0])
        ident = time.milis

        # Prepare the event
        event = Event(id=ident, time=time, time_end=time_end)

        # Save the event
        events.append(event)

    return events
