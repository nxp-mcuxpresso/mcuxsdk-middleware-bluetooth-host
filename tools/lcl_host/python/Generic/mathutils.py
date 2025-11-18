# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

def iswholecomplexnumber(val):
    if isinstance(val, complex):
        return val.real.is_integer() and val.imag.is_integer()
    return False


def iswholenumber(val, real_only=True):
    if isinstance(val, int):    # check for numeric type
        return True

    if isinstance(val, complex):
        if real_only:
            return False
        else:
            return val.real.is_integer() and val.imag.is_integer()

    if isinstance(val, float):
        return val.is_integer()

    return False

def is1Darray(val):
    # Returns True, when:
    # * val is a 1D-array (a "Nx1 array")
    # * val is a scalar (a "1x1 array")
    import numpy as np
    if isinstance(val, np.ndarray):
        if val.squeeze().ndim == 1 or val.squeeze().ndim == 0:
            return True
    return False

# FIXME: When N=1, I would expect the same result as with is1Darray(val), this is however not the case,
#  because a scalar is threathed as a 1D-array in one case, but not in the other.
def isNDarray(val, N=2):
    # Returns True, when:
    # * val is a N dimensional-array (a "NxMxKx... array")
    import numpy as np
    if isinstance(val, np.ndarray):
        if val.squeeze().ndim == N:
            return True
    return False