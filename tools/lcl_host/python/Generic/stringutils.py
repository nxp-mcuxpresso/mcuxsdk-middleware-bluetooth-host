# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------


# originally used textwrap.wrap, but was found to be slow
def wsplit(s, width=1, lpad='', rpad=''):
    return [lpad + s[i:i + width] + rpad for i in range(0, len(s), width)]


def xslice(s, start=0, stop=None, step=1, width=1):
    r = ''
    if stop is None:
        s = s[start:]
    else:
        s = s[start:stop]
    while s:
        r += s[:width]
        s = s[step:]
    return r


def str_to_bool(s):
    """Convert string to bool """
    idx = ['true', 'false', 'yes', 'no', '1', '0'].index(s.lower())
    if idx == -1:
        raise ValueError('Need bool; got %r' % s)
    return True if (idx % 2) == 0 else False
