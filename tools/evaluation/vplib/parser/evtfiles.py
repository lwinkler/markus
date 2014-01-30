# -*- coding: utf-8 -*-

"""
An Event parser

Created 2014-01-29 Fabien Dubosson
"""

import os
import sys
from vplib.time import Time
from collections import namedtuple

Event = namedtuple('Event', 'id time')


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
        return False

    # Prepare results array
    events = []

    # Parse lines
    for i, line in enumerate(lines):
        # If it is not a line containig an event time
        if '-->' not in line:
            continue

        # Get the first time text
        time_text, _ = line.strip().split('-->')

        # Transform it to time
        time = Time(text=time_text)

        ident = int(lines[i+3].strip().split('\t')[0])

        # Prepare the event
        event = Event(id=ident, time=time)

        # Save the event
        events.append(event)

    return events