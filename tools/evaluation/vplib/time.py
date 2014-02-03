# -*- coding: utf-8 -*-

"""
Working with time values

Created 2014-01-29 Fabien Dubosson
"""

from collections import namedtuple

# Time Tuple
TimeTuple = namedtuple("Time", "hours minutes seconds milis")


class Time:
    """A class reprenseting a Time"""

    def __init__(self, hours=0, minutes=0, seconds=0, milis=0, text=None,
                 sep=':', sep_ms='.'):
        if text:
            # Parse text values
            h, m, rest = text.split(sep)
            s, ms = rest.split(sep_ms)

            # Cast values
            hours = int(h)
            minutes = int(m)
            seconds = int(s)
            milis = int(ms)

        self.milis = milis
        self.milis += seconds * 1000
        self.milis += minutes * 60 * 1000
        self.milis += hours * 60 * 60 * 1000

    def toTuple(self):
        h, r = divmod(self.milis, 60 * 60 * 1000)
        m, r = divmod(r, 60 * 1000)
        s, ms = divmod(r, 1000)

        return TimeTuple(hours=h,
                         minutes=m,
                         seconds=s,
                         milis=ms)

    def __cmp__(self, other):
        return self.milis.__cmp__(other.milis)

    def __add__(self, other):
        return Time(milis=self.milis.__add__(other.milis))

    def __sub__(self, other):
        return Time(milis=self.milis.__sub__(other.milis))

    def __div__(self, other):
        return Time(milis=self.milis.__div__(other))

    def __repr__(self):
        fmt = "%02d:%02d:%02d.%03d"
        return fmt % self.toTuple()
