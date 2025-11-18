# Copyright 2022-2023 NXP
# 
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))

import json
import numpy
from nesteddict import NestedDict

class IOP_convert_nordic():
    def __init__(self, **kwargs):
        pass

    def convert(self, iopfile, ioprole, data):

        iop_data = json.load(iopfile)

        if (ioprole == 'init'):
            role_key = 'initiator'
        if (ioprole == 'refl'):
            role_key = 'reflector'

        iop_i = []
        iop_q = []
        iop_tof = []
        iop_rssi = []
        for result in iop_data:
            for step in result['data']['steps']:
               # print(step)
                if data.get('meta.has_tof', False) and step['step_mode'] == 1:
                    iop_tof.append(step['tox_tox_ns'])
                    iop_rssi.append(step['packet_rssi'])
                if data.get('meta.has_mciq', False) and step['step_mode'] == 2:
                    iop_i.append(step['tones'][0]['pct'][0])
                    iop_q.append(step['tones'][0]['pct'][1])
        if data.get('meta.has_tof', False):
            data['tof'][role_key]['d'] = numpy.asarray(iop_tof).reshape((len(iop_tof), 1))
            data['tof'][role_key]['rssi'] = numpy.asarray(iop_rssi).reshape((len(iop_rssi), 1))
        if data.get('meta.has_mciq', False):
            data.mciq[role_key]['i'] = numpy.asarray(iop_i).reshape((len(iop_i), 1))
            data.mciq[role_key]['q'] = numpy.asarray(iop_q).reshape((len(iop_q), 1))
            data.mciq[role_key]['tqi'] = numpy.zeros(len(iop_i), dtype=int).reshape((len(iop_i), 1))
