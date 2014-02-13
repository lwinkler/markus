# -*- coding: utf-8 -*-

"""
A SubRip parser

Created 2014-01-29 Fabien Dubosson
"""

import os
import codecs
from vplib.time import Time
from collections import namedtuple

Subtitle = namedtuple('Subtitle', 'number begin end text')


def parse(file_path):
    """ Parse the given subrip file """

    # Verify the existence of the file
    if not os.path.isfile(file_path):
        return []

    # Read the header
    with open(file_path, 'rb') as f:
        header = f.read(4)

    # Supported encodings
    encodings = [(codecs.BOM_UTF32, 'utf-32'),
                 (codecs.BOM_UTF16, 'utf-16'),
                 (codecs.BOM_UTF8, 'utf-8')]

    # Check if one of the encoding is present
    encoding = None
    hasBom = False
    for h, e in encodings:
        if header.startswith(h):
            encoding = e
            hasBom = True
            break

    # Read the files
    with codecs.open(file_path, 'r', encoding) as f:
        # If there is an encoding, consume the first character
        if hasBom:
            f.read(1)
        # Then read lines
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
        b_time = Time(text=b_stamp, sep_ms=',')
        e_time = Time(text=e_stamp, sep_ms=',')

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
