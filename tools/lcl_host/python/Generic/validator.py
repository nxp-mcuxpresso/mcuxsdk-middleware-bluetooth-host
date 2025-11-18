# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

from abc import ABC, abstractmethod
import numpy as np

class Validator(ABC):
    def __set_name__(self, owner, name):
        self.private_name = f'_{name}'

    def __get__(self, obj, objtype=None):
        return getattr(obj, self.private_name)

    def __set__(self, obj, value):
        self.validate(value)
        setattr(obj, self.private_name, value)

    @abstractmethod
    def validate(self, value):
        pass

class OneOf(Validator):
    def __init__(self, *options):
        self.options = options

    def validate(self, value):
        if value not in self.options:
            raise ValueError(f'Expected {value!r} to be one of {self.options!r}')

class Number(Validator):
    def __init__(self, minvalue=None, maxvalue=None):
        self.minvalue = minvalue
        self.maxvalue = maxvalue

    def validate(self, value):
        if not isinstance(value, (int, float)):
            raise TypeError(f'Expected {value!r} to be an int or float')
        if self.minvalue is not None and value < self.minvalue:
            raise ValueError(f'Expected {value!r} to be at least {self.minvalue!r}')
        if self.maxvalue is not None and value > self.maxvalue:
            raise ValueError(f'Expected {value!r} to be no more than {self.maxvalue!r}')

class OddNumber(Number):
    def validate(self, value):
        super().validate(value)
        if (value % 2) == 0:
            raise ValueError(f'Expected {value!r} to be odd')

class EvenNumber(Number):
    def validate(self, value):
        super().validate(value)
        if (value % 2) != 0:
            raise ValueError(f'Expected {value!r} to be even')

class Array(Validator):
    def __init__(self, minsize=None, maxsize=None, ndim=1):
        self.minsize = minsize
        self.maxsize = maxsize
        self.ndim    = ndim

    def validate(self, value):
        if not isinstance(value, np.ndarray):
            raise TypeError(f'Expected {value!r} to be an numpy.ndarray')
        if self.minsize is not None and len(value) < self.minsize:
            raise ValueError(f'Expected the length of value to be no smaller than {self.minsize!r}')
        if self.maxsize is not None and len(value) > self.maxsize:
            raise ValueError(f'Expected the length of value to be no larger than {self.maxsize!r}')
        if self.ndim is not None and len(value.shape) != self.ndim:
            raise ValueError(f'Expected value to have {self.ndim!r} dimensions')

class String(Validator):
    def __init__(self, minsize=None, maxsize=None):
        self.minsize = minsize
        self.maxsize = maxsize

    def validate(self, value):
        if not isinstance(value, str):
            raise TypeError(f'Expected {value!r} to be an str')
        if self.minsize is not None and len(value) < self.minsize:
            raise ValueError(f'Expected {value!r} to be no smaller than {self.minsize!r}')
        if self.maxsize is not None and len(value) > self.maxsize:
            raise ValueError(f'Expected {value!r} to be no larger than {self.maxsize!r}')

class Instance(Validator):
    def __init__(self, *options):
        self.options = options

    def validate(self, value):
        if not isinstance(value, self.options):
            raise TypeError(f'Expected {value!r} to be an instance of {self.options!r}')