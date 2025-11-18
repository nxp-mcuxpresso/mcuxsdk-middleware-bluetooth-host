#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

import numpy as np
import sys
import os
import logging

if __name__ == '__main__':
    sys.path.append(os.path.join("..", "..", "..", "Instruments"))
    sys.path.append(os.path.join("..", "..", "..", "Generic"))


class valueArray(object):
    def __new__(cls, **kargs):
        flt = kargs.get('filter', 'jump').lower()
        if flt == "jump":
            return valueArrayJumpFilter(**kargs)
        elif flt == "kalman":
            return valueArrayKalmanFilter(**kargs)
        raise Exception("Unknown filter identifier: %s" % (kargs.get('filter', '<unspecified>')))


class valueArrayGeneric(object):
    def __init__(self, **kargs):
        self.count = None
        self.errors = None
        self.raw_lst = None
        self.data_lst = None
        self.error_lst = None

        self.clear()

        self.params = {
            'depth': 40,
            'offset': -1.25,
            'bounds_lower': 0.0,
            'bounds_upper': 20.0,
            'verbose': True,
            'fmt': '%.2fm',
            'label': 'Distance'
        }
        self.set_parameters(**kargs)

    def set_parameters(self, **kargs):
        for k, v in list(kargs.items()):
            if k in self.params.keys():
                self.params[k] = v

    def get_error_count(self):
        return self.errors

    def get_count(self):
        return self.count

    def clear(self):
        self.count = 0
        self.errors = 0
        self.raw_lst = np.empty(shape=[0])
        self.data_lst = np.empty(shape=[0])
        self.error_lst = np.empty(shape=[0])

    # value is raw (no bounding or filtering)
    def insert(self, value, isvalid, log_msg=True):
        log = logging.getLogger(__name__)
        self.count += 1  # increase the measurement count

        if len(self.data_lst) != 0:
            value_prev = self.data_lst[-1]  # use last value
        else:
            value_prev = 0.0

        value_bound = self.apply_bounds(value, limit_only=False)
        if not isvalid:
            self.errors += 1

        value_new = self.filter(value_bound, value_prev, isvalid)
        if log_msg:
            msg = '[%d] %s %s => %s' % (self.count, self.params['label'],
                                        self.params['fmt'] % value, self.params['fmt'] % value_new)
            if not isvalid:
                log.error(msg)
            elif self.params['verbose']:
                log.info(msg)

        value_new = round(value_new, 2)
        value = round(value, 2)

        self.data_lst = np.append(self.data_lst, value_new)
        self.raw_lst = np.append(self.raw_lst, value)
        self.error_lst = np.append(self.error_lst, not isvalid)
        while len(self.raw_lst) > (self.params['depth']+1):
            self.data_lst = np.delete(self.data_lst, np.s_[0])  # remove first element
            self.raw_lst = np.delete(self.raw_lst, np.s_[0])  # remove first element
            self.error_lst = np.delete(self.error_lst, np.s_[0])  # remove first element

        return isvalid

    def apply_bounds(self, value, limit_only=False):
        if np.isnan(value):
            value = self.params['bounds_lower']
            return value
        if not limit_only:
            value = (value + self.params['offset'])
        if value < self.params['bounds_lower']:
            value = self.params['bounds_lower']
        elif value > self.params['bounds_upper']:
            value = self.params['bounds_upper']
        return value

    def filter(self, value, value_prev, isvalid):
        value_new = value
        if not isvalid:
            value_new = np.nan
        return value_new

    def __len__(self):
        return len(self.data_lst)

class valueArrayJumpFilter(valueArrayGeneric):
    def __init__(self, **kargs):
        super(self.__class__, self).__init__(**kargs)

        params = {
            'flt_hysteresis': 2.0,
            'flt_window': 5,
            'flt_warnings': True,
            'flt_ts': 0.5}
        self.params.update(params)

        for key in kargs.keys():
            if key in self.params.keys():
                self.params[key] = kargs.get(key, None)

    def filter(self, value, value_prev, isvalid):
        log = logging.getLogger(__name__)
        value_new = value
        if not isvalid:
            value_new = value_prev
        else:
            if len(self.data_lst) >= self.params['flt_window'] > 0:
                avg_data = np.mean(self.data_lst[-self.params['flt_window']:])
                jmp_data = np.abs(avg_data - value)
                if self.params['flt_hysteresis'] == 0.0:
                    value_new = np.mean(np.append(self.data_lst[-self.params['flt_window']:], value))
                elif jmp_data > self.params['flt_hysteresis']:
                    log.info('[%d]: Jump detected of %s on average %s %s' % (
                        self.count, self.params['fmt'] % jmp_data, self.params['label'], self.params['fmt'] % avg_data))
                    if avg_data > value:
                        value_new = avg_data - self.params['flt_hysteresis']
                    else:
                        value_new = avg_data + self.params['flt_hysteresis']
                value_new = self.apply_bounds(value_new, limit_only=True)
        self.data = [value_new]
        return value_new

class valueArrayKalmanFilter(valueArrayGeneric):
    def __init__(self, **kargs):
        from tracking_pkg.Tracking_Kalman import Tracking_Kalman
        super(self.__class__, self).__init__(**kargs)

        self.vel_lst = 0.0
        self.data_var_lst = 0.0
        self.vel_var_lst = 0.0

        params = {
            'flt_delta_f': 1e6,
            'flt_delta_t': 500e-6,
            'flt_d_std': 1.0,
            'flt_a_std': 1.0,
            'flt_ts': 0.5}
        self.params.update(params)

        for key in kargs.keys():
            if key in self.params.keys():
                self.params[key] = kargs.get(key, None)

        self.kf = Tracking_Kalman(ts=self.params['flt_ts'], adaptive=True,
                                  alpha=Tracking_Kalman.calc_alpha(delta_f=self.params['flt_delta_f'], delta_t=self.params['flt_delta_t']),
                                  d_std=self.params['flt_d_std'], a_std=self.params['flt_a_std'])

    def set_parameters(self, **kargs):
        for k, v in list(kargs.items()):
            if k in self.params.keys():
                self.params[k] = v
                if k in ['flt_delta_f', 'flt_delta_t']:
                    self.kf.alpha = self.kf.calc_alpha(delta_f=self.params['flt_delta_f'], delta_t=self.params['flt_delta_t'])

    #TODO: this is very similar to the valueArrayGeneric.insert. Is there a way to combine?
    def insert(self, value, isvalid, log_msg=True):
        log = logging.getLogger(__name__)
        self.count += 1  # increase the measurement count

        if len(self.data_lst) != 0:
            value_prev = self.data_lst[-1]  # use last value
        else:
            value_prev = 0.0

        value_bound = self.apply_bounds(value, limit_only=False)
        if not isvalid:
            self.errors += 1

        value_new, dist_var, vel, vel_var = self.filter(value_bound, value_prev, isvalid)
        if log_msg:
            msg = '[%d] %s %s => %s' % (self.count, self.params['label'],
                                        self.params['fmt'] % value, self.params['fmt'] % value_new)
            if not isvalid:
                log.error(msg)
            elif self.params['verbose']:
                log.info(msg)

        value_new = round(value_new, 2)
        value = round(value, 2)

        self.data_lst = np.append(self.data_lst, value_new)
        self.raw_lst = np.append(self.raw_lst, value)
        self.error_lst = np.append(self.error_lst, not isvalid)
        self.vel_lst = np.append(self.vel_lst, vel)
        self.data_var_lst = np.append(self.data_var_lst, dist_var)
        self.vel_var_lst = np.append(self.vel_var_lst, vel_var)
        while len(self.raw_lst) > self.params['depth']:
            self.data_lst = np.delete(self.data_lst, np.s_[0])  # remove first element
            self.raw_lst = np.delete(self.raw_lst, np.s_[0])  # remove first element
            self.error_lst = np.delete(self.error_lst, np.s_[0])  # remove first element
            self.vel_lst = np.delete(self.vel_lst, np.s_[0])  # remove first element
            self.data_var_lst = np.delete(self.data_var_lst, np.s_[0])  # remove first element
            self.vel_var_lst = np.delete(self.vel_var_lst, np.s_[0])  # remove first element

        return isvalid

    def filter(self, value, value_prev, isvalid):
        value_new = value
        if not isvalid:
            value_new = np.nan
        return self.kf.do(value_new, ts=self.params['flt_ts'])

