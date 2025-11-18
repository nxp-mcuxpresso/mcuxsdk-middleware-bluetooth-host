#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

import numpy as np
import sys
import os
from location_estimator_pkg import LocationEstimator
from valueArray import valueArray

class locationArray:
    def __init__(self, **kargs):
        self.le = LocationEstimator(**kargs['loc_est_cfg'])

        self.items = []
        for n, id in enumerate(self.le.anchorID):
            x = valueArray(**kargs['array_cfg'])
            x.set_parameters(label='distance({})'.format(id))
            self.items.append(x)

    def get_border_geometry(self):
        return self.le.borderRect

    def get_obstacle_geometry(self):
        return self.le.obstacleRect

    def get_anchor_ids(self):
        return self.le.anchorID.tolist()

    def get_anchor_positions(self):
        return self.le.anchorPosition.tolist()

    def get_xlimits(self):
        return self.le.get_xlimits()

    def get_ylimits(self):
        return self.le.get_ylimits()

    def set_parameters(self, **kargs):
        for item in self.items:
            item.set_parameters(**kargs)

    def get_activity(self, depth=0):
        cnt = 0.0
        if depth == 0:
            for item in self.items:
                if item.get_count():
                    cnt += float(item.get_error_count()) / float(item.get_count())
        else:
            for item in self.items:
                cnt += float(item.get_error_count())
            cnt /= float(depth)
        inactivity = cnt / float(len(self.items))
        return 1.0 - inactivity

    def get_error_info(self):
        info = []
        for item in self.items:
            info.extend(item.get_error_info())
        return info

    def get_count(self):
        cnt = 0
        for item in self.items:
            cnt += item.get_count()
        return cnt

    def clear(self):
        for item in self.items:
            item.clear()

    def insert(self, value, returncode, boardnumber, delta_t=None, log_msg=True):
        for n, id in enumerate(self.le.anchorID):
            if boardnumber == id:
                if delta_t is not None:
                    self.items[n].set_parameters(flt_ts=delta_t)
                return self.items[n].insert(value, returncode, log_msg)

    def update(self):
        distances = np.zeros((len(self.items),1))
        dist_var = np.zeros(len(self.items))
        delta_t = np.zeros(len(self.items))
        for n, id in enumerate(self.le.anchorID):
            if len(self.items[n].data_lst):
                distances[n] = self.items[n].data_lst[-1]
                if 'LocationEstimator_KF' in str(self.le.__class__):
                    if hasattr(self.items[n], 'data_var_lst'):
                        dist_var[n] = self.items[n].data_var_lst[-1]
                    else:
                        dist_var[n] = 1
                    delta_t[n] = self.items[n].params['flt_ts']
        if 'LocationEstimator_KF' in str(self.le.__class__):
            return self.le.do(distances.squeeze(), np.mean(delta_t), dist_var)
        else:
            return self.le.do(distances)

if __name__ == '__main__':
    sys.path.append(os.path.join("..", "..", "..", "Instruments"))
    sys.path.append(os.path.join("..", "..", "..", "Generic"))
