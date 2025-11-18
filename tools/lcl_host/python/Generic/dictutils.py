# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

def rget(dct, *keys, **kwargs):
    default = kwargs.get('default', None)
    try:
        for key in keys:
            dct = dct[key]
        return dct
    except (KeyError, TypeError):
        return default


def rset(dct, *keys, **kwargs):
    value = kwargs.get('value', None)
    try:
        for key in keys[:-1]:
            dct = dct[key]
        dct[keys[-1]] = value
        return True
    except (KeyError, TypeError):
        return False


# this function creates the (nested) dict key(s) and sets a default (if the key does not exist)
def rsetdefault(dct, *keys, value):
    try:
        for n, key in enumerate(keys, 1):
            if isinstance(dct, dict):
                if n < len(keys):
                    dct.setdefault(key, {})
                    dct = dct[key]
                else:
                    dct.setdefault(key, value)
            else:
                return False
        return True
    except (KeyError, TypeError):
        return False


def rtest(dct, *keys):
    try:
        for key in keys:
            dct = dct[key]
        return True
    except (KeyError, TypeError):
        return False

