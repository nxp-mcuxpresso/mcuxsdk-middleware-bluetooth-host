# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import dictutils as du
from cerberus import Validator
import numpy as np


class MyValidator(Validator):
    def _validate_group(self, group, field, value):
        """ Test the oddity of a value.

        The rule's arguments are validated against this schema:
        {'type': 'list'}
        """
        pass

    def _validate_hasshape(self, hasshape, field, value):
        """ Test if it is numpy.ndarray with a specific shape.

        The rule's arguments are validated against this schema:
        {'type': 'list'}
        """
        if not isinstance(value, np.ndarray):
            self._error(field, "Must be a numpy.ndarray")

        mode = self.document_path[0]
        shape = []
        for s in hasshape:
            if isinstance(s, int):
                shape.append(s)
            elif isinstance(s, str):
                if du.rtest(self.root_document, 'Local', s):  # version 1
                    s = self.root_document['Local'][s]
                elif du.rtest(self.root_document, mode, 'cfg', s):  # version 2
                    s = self.root_document[mode]['cfg'][s]
                else:
                    self._error(field, "Could not derive shape")
                    return
                shape.append(s)
        if value.shape != tuple(shape):
            self._error(field, "Incorrect shape")

    def _validate_hastype(self, hastype, field, value):
        """ Test if it is numpy.ndarray with a specific type.

        The rule's arguments are validated against this schema:
        {'type': 'string'}
        """
        if not isinstance(value, np.ndarray):
            self._error(field, "Must be a numpy.ndarray")
        type = None
        if hastype == 'real':
            type = np.float_
        elif hastype == 'natural':
            type = np.int_
        elif hastype == 'complex':
            type = np.complex_
        if value.dtype.type != type:
            self._error(field, "Incorrect type")

    def _validate_isnatural(self, isnatural, field, value):
        """ Test if it is a natural number.

        The rule's arguments are validated against this schema:
        {'type': 'boolean'}
        """
        if isnatural and not (isinstance(value, int) and value >= 0):
            self._error(field, "Must be a natural number")

    def _validate_isreal(self, isreal, field, value):
        """ Test if it is a natural number.

        The rule's arguments are validated against this schema:
        {'type': 'boolean'}
        """
        if isreal and not isinstance(value, float):
            self._error(field, "Must be a real number")

    def _validate_isrealpos(self, isrealpos, field, value):
        """ Test if it is a natural number.

        The rule's arguments are validated against this schema:
        {'type': 'boolean'}
        """
        if isrealpos and not (isinstance(value, float) and value >= 0):
            self._error(field, "Must be a real positive number")
