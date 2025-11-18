# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

#  Content   : This file implements digital filters. The difference with scipy.signal filter modules, it that these
#              only work on arrays. Iterative processing is not possible, since the state is not persistent. This
#              set of classes should have this functionality.

from abc import ABC, abstractmethod
from validator import *
from reprmixin import ReprMixin
import numpy as np


class DigitalFilter(ABC, ReprMixin):
    @abstractmethod
    def reset(self):
        pass

    @abstractmethod
    def do(self, *args, **kwargs):
        pass

    def step(self, *args, **kwargs):
        return self.do(*args, **kwargs)        # A convenience funcion for Matlab-users similar to dsp.step

    def filter(self, *args, **kwargs):
        return self.do(*args, **kwargs)        # A convenience funcion for Matlab-users similar to dfilt.filter or mfilt.filter


class MedianFilt(DigitalFilter):
    # ------------------------------------------------------------------------------
    #                CLASS ATTRIBUTES - SHARED BETWEEN ALL INSTANCES
    # ------------------------------------------------------------------------------
    order = OddNumber(minvalue=1)  # TODO: at least the validation happens, but is it a problem that the property is shared among all instances?

    def __init__(self, order=5):
        self.order = order
        self.state = np.zeros(self.order)

    def do(self, x):
        assert len(x.shape) == 1, 'Expected the x to have a single dimension, but is has %d dimensions instead' % len(x.shape)
        y = np.empty(x.shape)
        for k, x in enumerate(x):
            self.state = np.concatenate(([x], self.state[0:-1]))
            y[k] = np.median(self.state)
        return y

    def reset(self):
        self.state = np.zeros(self.order)


class FIRDecimator(DigitalFilter):
    # ------------------------------------------------------------------------------
    #                CLASS ATTRIBUTES - SHARED BETWEEN ALL INSTANCES
    # ------------------------------------------------------------------------------
    Numerator = Array(minsize=1, ndim=1)  # TODO: at least the validation happens, but is it a problem that the property is shared among all instances?
    DecimationFactor = Number(minvalue=1)

    def __init__(self, num, decimfactor):
        self.Numerator = num
        self.DecimationFactor = decimfactor
        self.state = np.zeros(len(self.Numerator))

    def do(self, x):
        assert len(x.shape) == 1, 'Expected the x to have a single dimension, but is has %d dimensions instead' % len(x.shape)

        # FIXME: The implementation is blunt and does not exploit the polyphase decomposition.
        y = np.empty(x.shape)
        for k, x in enumerate(x):
            self.state = np.concatenate(([x], self.state[0:-1]))
            y[k] = np.dot(self.state, self.Numerator)
        y = y[0::self.DecimationFactor]
        return y

    def reset(self):
        self.state = np.zeros(len(self.Numerator))

