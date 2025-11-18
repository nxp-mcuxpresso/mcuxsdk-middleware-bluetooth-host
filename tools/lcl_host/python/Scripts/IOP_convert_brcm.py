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

class IOP_convert_brcm():
    def __init__(self, **kwargs):
        pass

    def convert(self, iopfile, ioprole, data):

        brcm_data = json.load(iopfile)

        if (ioprole == 'init'):
            role_key = 'initiator'
        if (ioprole == 'refl'):
            role_key = 'reflector'

        if data.get('meta.has_tof', False):
            brcm_tod = numpy.asarray(brcm_data[role_key]['pes']['HADM_TOD_TOA'])
            brcm_tof_offset = numpy.asarray(brcm_data[role_key]['pes']['TOD_TOA_OFFSET'])
            brcm_tof = numpy.round((1000 / 24) * (brcm_tod + brcm_tof_offset / 256))
            data['tof'][role_key]['d'] = numpy.asarray(brcm_tof).reshape((len(brcm_tod), 1))
            data['tof'][role_key]['rssi'] = numpy.asarray(brcm_data[role_key]['pes']['RTT_RSSI'])
        if data.get('meta.has_mciq', False):
            brcm_i = numpy.asarray(brcm_data[role_key]['tes']['FIL_OP_RE'])
            brcm_i = numpy.asarray(brcm_data[role_key]['tes']['FIL_OP_RE']).reshape((len(brcm_i), 1))
            brcm_q = numpy.asarray(brcm_data[role_key]['tes']['FIL_OP_IM'])
            brcm_q = numpy.asarray(brcm_data[role_key]['tes']['FIL_OP_IM']).reshape((len(brcm_q), 1))
            data.mciq[role_key]['i'] = brcm_i
            data.mciq[role_key]['q'] = brcm_q
            data.mciq[role_key]['tqi'] = numpy.zeros(len(brcm_i), dtype=int).reshape((len(brcm_i), 1))


