# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

class MeasurementModes(object):
    ToF = 1          # Time of Flight - HADM mode 1
    MCIQ = 2         # Multi Carrier IQ (aka phase based ranging) - HADM mode 2
    Group = 16       # group ranging

    mapping = {MCIQ: 'mciq', ToF: 'tof', Group: 'group', }

    @staticmethod
    def to_str(val):
        return MeasurementModes.mapping[val]

    @staticmethod
    def all(to_str=False):
        if to_str:
            return MeasurementModes.values()
        return MeasurementModes.keys()

    @staticmethod
    def all_types(to_str=False):
        types = [MeasurementModes.MCIQ, MeasurementModes.ToF]
        if to_str:
            return list(map(MeasurementModes.to_str, types))
        return types


class ReturnCodes(object):
    info = {
        'success': ('Successful measurement', 0),
        'emptyresponse': ('Empty response from board error', 2),
        'badresponse': ('Bad response from board error', 2),
        'crc': ('crc error', 2),
        'parsing': ('Parsing error', 2),
        'validation': ('Failed while validating result-dict error', 2),
        'watchdog': ('Watchdog error', 2),
        'chanaccessfail': ('Channel access fail error', 2),
        'assertion': ('Assertion error', 2),
        'noack': ('No-acknowledgement error', 2),
        'timeout': ('Timeout error', 2),
        'unknown': ('Unknown error', 2),
        'badgain': ('RX gain too high', 2),
        'negative': ('Distance negative warning', 1),
        'toolow': ('Distance too low warning', 1),
        'toohigh': ('Distance too high warning', 1),
        'notanumber': ('Not a number', 1),
        'unexpecteduniqueid': ('Unexpected Unique ID', 2),
        'unknownuniqueid': ('Unknown Unique ID', 2),
        'swversionmismatch': ('Mismatching SW Version', 2),
    }

    def __init__(self, key):
        self.key = key
        if self.key in list(self.info.keys()):
            self.text, self.type = self.info[key]
        else:
            assert False, '%s is not a known ErrorCode key' % key

    def is_error(self):
        if self.type == 2:
            return True
        return False

    def is_warning(self):
        if self.type == 1:
            return True
        return False

    def is_valid(self):
        if self.type == 0:
            return True
        return False

    def __setattr__(self, attr, value):
        if hasattr(self, attr):
            raise Exception('Attribute %s already has a value and so cannot be written to' % attr)
        self.__dict__[attr] = value