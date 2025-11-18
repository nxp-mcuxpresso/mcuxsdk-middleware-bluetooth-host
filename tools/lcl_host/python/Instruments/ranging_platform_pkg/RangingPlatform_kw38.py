# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import time
from .RangingPlatform import RangingPlatformBase


class RangingPlatform_kw38(RangingPlatformBase):
    # DefaultConfig is applied during initialization when defaults are requested
    DefaultConfig = [
        'system verbosity 44'
    ]
    SerialPortIdList = [(0xd28, 0x204), (0x1366, 0x1015)]  # DAPLink, JLink

    def __init__(self, **kwargs):
        kwargs.setdefault('format', '4')
        kwargs.setdefault('RangeEstimator',
                         {'channel_reconstruct': '2D', 'ranging_engine': 'Music_cpp'})
        super(self.__class__, self).__init__(**kwargs)
        self.AlgorithmExec = False
        self.estimator_config('re.TOL2', 0.99, 'Music')
        self.prev_error_msg = ''

    # nxp boards have fixed addresses
    def set_own_address(self, val):
        pass

    @property
    def mag_scaling(self):
        return 1.0/2048.0

    @property
    def cfo_scaling(self):
       return 1 #1953.125  # [Hz/value] Scaling value for the reported CFO-number in the Result-dict to obtain the CFO-value in Hz

    def get_max_baudrate(self):
        max_baudrate = None
        if self.ifc.vid == 0x1366 and self.ifc.pid == 0x1015:  # Increase speed on JLINK
            max_baudrate = 460800  # 921600 not working  for kw38(reason unknown, related to UART driver)
        return max_baudrate

    def range_measurement_pre_process(self, result):
        # convert unit of timing parameters
        for mode in self._platform_config['active_modes']:
            fields = [f'{mode}.cfg.t_intra', f'{mode}.cfg.t_inter', f'{mode}.cfg.t_intra_antenna']
            for field in fields:
                if field in result:
                    result[field] = round(1.0e-6 * float(result[field]), 6)
        if result['meta.error_msg'] == 'emptyresponse':
            # no response received, possibly because board reset and is using default baudrate again
            self.board_initialize()
            self.wait_on_connection()
        elif result['meta.error_msg'] == 'notconnected' and self.prev_error_msg == 'notconnected':
            # workaround for the central not correctly initiating scanning occasionally
            self.write_configuration(['system reset'])
            self.board_initialize()
            self.wait_on_connection()
        self.prev_error_msg = result['meta.error_msg']
        super(self.__class__, self).range_measurement_pre_process(result)
