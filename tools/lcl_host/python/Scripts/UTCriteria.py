#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
# Utilities for checking record_range_measurement result against criteria
import numpy


# Center values to zero (substract mean)
def error_to_average(values):
    np_val = numpy.array(values)
    np_val = np_val - np_val.mean()
    return numpy.ndarray.tolist(numpy.round(np_val, 2))


# Standard deviation of the series, return single value
def stdev(values):
    stddev = numpy.std(values)
    return numpy.round(stddev, 2)


# 'ignore' is a tuple of values that will be ignored during criteria evaluation
class Criteria:
    def __init__(self, category, attribute, acceptance, func=None, ignore=()):
        self.category = category
        self._attribute = attribute
        self._acceptance = acceptance
        self._func = func
        self._ignore = (ignore if isinstance(ignore, tuple) else (ignore, ))

    def single_is_passed(self, value):
        if isinstance(self._acceptance, (float, int, str)):
            return value == self._acceptance
        if isinstance(self._acceptance, tuple):
            return self._acceptance[0] <= value <= self._acceptance[1]

    # Handles attribute as list or as single value
    def is_passed(self, result):
        value = result.get(self._attribute)
        if value is None:
            return False, "NOT FOUND"
        # Convert numpy array to list for homogeneous verdict printing
        if isinstance(value, numpy.ndarray):
            value = numpy.ndarray.tolist(value)
        # Filter undesired values
        preprocessed_value = value
        if isinstance(value, (list, tuple)):
            preprocessed_value = list(filter(lambda x: x not in self._ignore, value))
        # Invoke any registered function
        if self._func is not None:
            preprocessed_value = self._func(preprocessed_value)
        # Evaluate verdict for the pre-processed value
        if isinstance(preprocessed_value, (list, tuple)):
            verdict = all(self.single_is_passed(preprocessed_value[key]) for key in range(len(preprocessed_value)))
            return verdict, value
        else:
            return self.single_is_passed(preprocessed_value), preprocessed_value

    def __str__(self):
        if isinstance(self._acceptance, (list, tuple)):
            condition = ' in '
        else:
            condition = ' is '
        return str(self._attribute) + condition + ('empty' if str(self._acceptance) == '' else str(self._acceptance)) + (" " + self._func.__name__ if self._func is not None else "")


class CriteriaList:
    def __init__(self):
        self.dict = {}

    def __str__(self):
        return str([str(self.dict[key]) for key in self.dict])

    def set_criteria(self, criteria):
        self.dict[criteria.category + "." + criteria._attribute] = criteria

    def get_failures(self, result, categories):
        failures = []
        values = []
        combined_verdict = True
        for key in self.dict:
            criteria = self.dict[key]
            if categories is not None and criteria.category not in categories:
                continue
            verdict, value = criteria.is_passed(result)
            combined_verdict = combined_verdict and verdict
            if not verdict:
                failures.append((str(self.dict[key]), value))
            values.append((str(key) + ("." + criteria._func.__name__ if criteria._func is not None else ""), value))
        return combined_verdict, failures, values

    # Return a tuple of strings representing criteria to be applied to the given list of categories
    def get_criteria_per_category(self, cat_list):
        criteria = []
        for key in self.dict:
            if self.dict[key].category in cat_list:
                criteria.append(str(self.dict[key]))
        return criteria

class HadmResultVerdict(CriteriaList):
    MAX_SLOPE = 2*numpy.pi
    def __init__(self):
        super().__init__()

        self.set_criteria(Criteria('core', 'meta.error_msg', ''))
        self.set_criteria(Criteria('core', 'info.init.flags', 0))
        self.set_criteria(Criteria('core', 'info.refl.flags', 0))

        self.set_criteria(Criteria('rtp', 'mciq.result.slope_rmse', (0.00, 6.0))) # TODO: Standard expect 5.7, but need more HW tuning to reach this quality
        self.set_criteria(Criteria('rtp', 'mciq.result.slope', (-HadmResultVerdict.MAX_SLOPE, 0.07))) # expect negative slope

        self.set_criteria(Criteria('rtt', 'diag.rtt.init.nb_errors', (0, 3)))  # TODO: We expect zero here, but need to fix drift issues first
        self.set_criteria(Criteria('rtt', 'diag.rtt.refl.nb_errors', (0, 3)))  # TODO: We expect zero here, but need to fix drift issues first
        self.set_criteria(Criteria('rtt', 'tof.initiator.rssi', (-3, 3), func=error_to_average, ignore=127))
        self.set_criteria(Criteria('rtt', 'tof.reflector.rssi', (-3, 3), func=error_to_average, ignore=127))

        self.set_criteria(Criteria('rtt1', 'tof.result.std', (0.00, 6.0)))  # See if 4.0 can be reached
        self.set_criteria(Criteria('rtt1', 'tof.initiator.rssi', (-1, 1), func=stdev, ignore=127))
        self.set_criteria(Criteria('rtt1', 'tof.reflector.rssi', (-1, 1), func=stdev, ignore=127))
        self.set_criteria(Criteria('rtt1', 'md0.init.cfo', (-100, 100), func=error_to_average)) # +/- 1 ppm

        self.set_criteria(Criteria('rtt2', 'tof.result.std', (0.00, 1.6)))
        self.set_criteria(Criteria('rtt2', 'tof.initiator.rssi', (-6, 6), func=stdev, ignore=127))
        self.set_criteria(Criteria('rtt2', 'tof.reflector.rssi', (-6, 6), func=stdev, ignore=127))
        self.set_criteria(Criteria('rtt2', 'md0.init.cfo', (-150, 150), func=error_to_average)) # +/- 1.5 ppm

        self.set_criteria(Criteria('nadm_off', 'tof.result.init_nadm', (15, 15)))
        self.set_criteria(Criteria('nadm_off', 'tof.result.refl_nadm', (15, 15)))

        self.set_criteria(Criteria('nadm_on', 'tof.result.init_nadm', (0, 4)))
        self.set_criteria(Criteria('nadm_on', 'tof.result.refl_nadm', (0, 4)))

        self.set_criteria(Criteria('algoCDE', 'mciq.result.CDE_dqi', (0.96, 1.0)))
        self.set_criteria(Criteria('algoRADE', 'mciq.result.RADE_dqi', (0.93, 1.0)))
