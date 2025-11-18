# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import numpy as np


def printMatrix(a):
    assert isinstance(a,  (np.ndarray, np.matrixlib.defmatrix.matrix)), 'input a must be a numpy-array of numpy-matrix'
    integer_types = (int, int,  np.int, np.int8, np.int16, np.int32, np.int64)
    if a.dtype in integer_types:
        if isvector(a):
            printArrayInt(a)
        else:
            printMatrixInt(a)
    else:
        if isvector(a):
            printArrayFloat(a)
        else:
            printMatrixFloat(a)


def printMatrixInt(a):
    print(("Matrix[%d][%d]" % (a.shape[0], a.shape[1])))
    rows = a.shape[0]
    cols = a.shape[1]
    if np.any(np.iscomplex(a)):
        for i in range(0, rows):
            for j in range(0, cols):
                print(("%d %+di" % (a[i, j].real, a[i, j].imag)))
            print()
    else:
        for i in range(0, rows):
            for j in range(0, cols):
                print(("%d" % a[i, j]))
            print()
    print()


def printArrayInt(a):
    print(("Array[%d]" % a.size))
    if np.any(np.iscomplex(a)):
        for i in range(0, a.size):
            print(("%d %+di" % (a[i].real, a[i].imag)))
    else:
        for i in range(0, a.size):
            print(("%d" % a[i].real))
    print()


def printMatrixFloat(a):
    print(("Matrix[%d][%d]" % (a.shape[0], a.shape[1])))
    rows = a.shape[0]
    cols = a.shape[1]
    if np.any(np.iscomplex(a)):
        for i in range(0, rows):
            for j in range(0, cols):
                print(("% 6.6f %+6.6fi  " % (a[i, j].real, a[i, j].imag)), end=' ')
            print()
    else:
        for i in range(0, rows):
            for j in range(0, cols):
                print(("%6.6f" % a[i, j]), end=' ')
            print()
    print()


def printArrayFloat(a):
    print(("Array[%d]" % a.size))
    if np.any(np.iscomplex(a)):
        for i in range(0, a.size):
            print(("% 6.6f %+6.6fi  " % (a[i].real, a[i].imag)), end=' ')
    else:
        for i in range(0, a.size):
            print(("%6.6f" % a[i]), end=' ')
    print()


def isvector(a):
    if a.ndim == 1:
        return True
    elif 1 in a.shape:
        return True
    else:
        return False
