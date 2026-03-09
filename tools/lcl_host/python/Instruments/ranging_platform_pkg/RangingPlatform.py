# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

from importlib import import_module
from ComPort import ComPort as ComPortCls
import dictutils as du
import os
import re
import numpy as np
import pandas as pd
import time
import yaml
from xtimer import xTimer
from .parser import RangingPlatformParser
from .resultfile import ResultFile
from .rng_platform_types import *
import traceback


class RangingPlatform(object):
    def __new__(cls, **kwargs):
        identifier = kwargs.get('board', 'unspecified')
        prefix = 'RangingPlatform'
        modname = f'{prefix}_{identifier}'
        try:
            imported_module = import_module('.' + modname, package=__package__)
            return getattr(imported_module, modname)(**kwargs)
        except ModuleNotFoundError:
            raise Exception("Unknown %s identifier: %s" % (prefix, identifier))


class RangingPlatformBase:
    ROLES = ['initiator', 'reflector']
    # DefaultConfig is applied during initialization when defaults are requested
    DefaultConfig = []
    SerialPortIdList = []
    DefaultBaudrate = 115200
    Name = ''
    csrole = 'Initiator'
    testmode = False
    proc_repeat_mode = False

    @property
    def connected(self):
        if self.ifc is None:
            return False
        else:
            return self.ifc.connected()

    # Boardnumber was initially obtained from a descriptor file.
    # This is disabled to ease maintenance of tests setup
    @property
    def BoardNumber(self):
        return 1
        #return self.uniqueid2boardnumber(self.UniqueId)

    # id_lst is a list of vid/pid pairs (tuple)
    # serial_lst is a list of serial numbers (strings), this is an optional filter
    def get_serial_ports(self, id_lst=[], serial_lst=[]):
        if len(id_lst) == 0:
            id_lst = self.SerialPortIdList
        return ComPortCls.get_serial_ports(id_lst=id_lst, serial_lst=serial_lst)

    def boardnumber2address(self, boardnumber):
        info = self.BoardInfo[self.BoardInfo.number == boardnumber].address.tolist()
        if len(info) == 0:
            return None
        return int(info[0], 0)

    def boardnumber2uniqueid(self, boardnumber):
        info = self.BoardInfo[self.BoardInfo.number == boardnumber].uniqueid.tolist()
        if len(info) == 0:
            return None
        return info[0]

    def uniqueid2boardnumber(self, unique_id):
        if not isinstance(unique_id, str):
            return None
        info = self.BoardInfo[self.BoardInfo['uniqueid'].str.lower() == unique_id.lower()].number.tolist()
        if len(info) == 0:
            return None
        return info[0]

    def uniqueid2address(self, unique_id):
        if not isinstance(unique_id, str):
            return None
        info = self.BoardInfo[self.BoardInfo['uniqueid'].str.lower() == unique_id.lower()].address.tolist()
        if len(info) == 0:
            return None
        return int(info[0], 0)

    def address2boardnumber(self, address):
        info = self.BoardInfo[self.BoardInfo['address'].str.lower() == '0x%012x' % address].number.tolist()
        if len(info) == 0:
            return None
        return info[0]

    def address2uniqueid(self, address):
        info = self.BoardInfo[self.BoardInfo['address'].str.lower() == '0x%012x' % address].uniqueid.tolist()
        if len(info) == 0:
            return None
        return info[0]

    def get_boardnumber_list(self):
        return self.BoardInfo.loc[:, 'number'].tolist()

    def get_swinfo(self, parameter):
        name = None
        if 'default' in self.SwInfo.keys():
            name = 'default'
        if self.SwVersion in self.SwInfo.keys():
            name = self.SwVersion
        if getattr(self, parameter, None):
            return getattr(self, parameter)
        return du.rget(self.SwInfo, name, parameter, default=None)

    def platform_config(self, key, value, update=True):
        if key in self._platform_config:
            self._platform_config[key] = value
        if not update:
            return
        if self._platform_config['mode'] & MeasurementModes.Group:  # groupranging
            init_boardnumber = self._platform_config['init_boardnumber']
            refl_boardnumbers = self._platform_config['remote_boardnumbers']
            if init_boardnumber is None:
                init_boardnumber = self.BoardNumber
            if refl_boardnumbers.count(init_boardnumber):
                refl_boardnumbers.remove(init_boardnumber)
                refl_boardnumbers.append(self.BoardNumber)
        elif self._platform_config['init_boardnumber'] is None and self._platform_config['refl_boardnumber'] is None:  # point-2-point
            init_boardnumber = self.BoardNumber
            refl_boardnumbers = self._platform_config['remote_boardnumbers']
        else:  # multiconnect
            init_boardnumber = self._platform_config['init_boardnumber']
            refl_boardnumbers = [self._platform_config['refl_boardnumber']]
            if init_boardnumber is None:
                init_boardnumber = self.BoardNumber
        self._platform_response['init_boardnumber'] = init_boardnumber
        self._platform_response['refl_boardnumbers'] = refl_boardnumbers

        active_modes = []
        for m in MeasurementModes.all_types():
            mode = MeasurementModes.to_str(m)
            if self._platform_config['mode'] & m:
                active_modes.append(mode)
        self._platform_config['active_modes'] = active_modes
        self.resultfile.initialize(active_modes)

    def set_diagnose(self, diagnose):
        self.diagnose = diagnose

    def __init__(self, **kwargs):
        self.bleRole = 'none'
        self.ifc = ComPortCls(debug=kwargs.get('debug', 0))
        self.SupportedFeatures = []
        self.AlgorithmExec = True
        self.Calibrate = False
        self.Timeout = 4.0
        self.UniqueId = None
        self.SwVersion = None
        self.diagnose = False

        self._platform_config = {'remote_boardnumbers': [], 'init_boardnumber': None, 'refl_boardnumber': None,
                                 'mode': None, 'active_modes': [], 'range_cmd': None,
                                 'circle_of_trust': None, 'percentage_carriers': None}
        self._platform_response = {'init_boardnumber': None, 'refl_boardnumbers': [], 'marker': '[DONE]'}

        filepath = os.path.dirname(os.path.realpath(__file__))
        filename = 'swinfo.{}.yaml'.format(kwargs.get('board'))
        if not os.path.exists(os.path.join(filepath, filename)):
            filename = 'swinfo.default.yaml'
        self.SwInfo = {}
        try:
            with open(os.path.join(filepath, filename), 'r') as stream:
                self.SwInfo = yaml.safe_load(stream)
        except:
            raise Exception('Failed opening SwInfo YAML file')
        self.BoardInfo = pd.read_csv(os.path.join(filepath, 'boardinfo.{}.csv'.format(kwargs.get('board'))),
                                     dtype={'uniqueid': str, 'number': np.int32, 'address': str, 'info': str})
        self.UseMaxBaudrate = kwargs.get('usemaxbaudrate', True)
        self.Calibrate = kwargs.get('cal', None)

        self.resultfile = ResultFile(**kwargs.get('ResultFile', {}))
        self.parser = RangingPlatformParser(input_format=kwargs.get('format', '3'), debug=kwargs.get('debug', 0), log=kwargs.get('log', None))

        self.set_measurement_mode(MeasurementModes.MCIQ)

    def __del__(self):
        self.disconnect()

    def disconnect(self):
        self.ifc.close()

    def connect(self, port):
        self.ifc.open(port=port, baudrate=self.DefaultBaudrate)
        assert self.connected, 'Could not connect to serial-port %s' % port

        self.board_initialize()

        # retry mechanism
        for _i in range(3):
            self.ifc.read()

            self.UniqueId = self.get_unique_id()
            if self.BoardNumber is not None:  # check on retry level
                break
        assert self.UniqueId is not None, 'Unable to retrieve UniqueId from board'
        assert self.BoardNumber is not None, 'Unknown UniqueId'

    def wait_on_connection(self):
        if self.testmode:
            return True
        timeout = max(1, len(self._platform_config['remote_boardnumbers'])) * self.Timeout + time.time()
        while timeout > time.time():
            val = self.read_configuration("communication connected")
            if val == '1':
                time.sleep(0.1)
                return True
            time.sleep(0.010)
        return False

    def board_initialize(self):
        target_baudrate = self.DefaultBaudrate
        max_baudrate = self.get_max_baudrate()
        if self.UseMaxBaudrate and max_baudrate is not None:
            target_baudrate = max_baudrate
        if max_baudrate == self.DefaultBaudrate:
            return

        # baudrate needs to be changed, inform the board
        baudrates = [self.DefaultBaudrate, max_baudrate]
        if self.DefaultBaudrate == target_baudrate:
            baudrates = [max_baudrate, self.DefaultBaudrate]
        cmd1 = 'abcdefghij\n'
        cmd2 = f'system baudrate {target_baudrate}\n'
        self.ifc.apply_settings(baudrate=baudrates[0])
        self.ifc.write_with_response(cmd1)
        time.sleep(0.200) #wait for the device to wake up if needed
        self.ifc.write_with_response(cmd2)
        self.ifc.apply_settings(baudrate=baudrates[1])
        resp = self.ifc.write_with_response(cmd1)
        if resp.find('command not found') != -1:
            return True
        return False

    def initialize(self, port=None, configuration=None, defaults=True, role='initiator'):
        if port or not self.ifc.connected():
            self.connect(port)

        self.SwVersion = self.get_sw_version()

        self.get_supported_features()

        if defaults:
            self.defaults(reload=False)

        self.set_own_address(None)

        if self.Calibrate is not None:
            self.set_calibration_mode()

        if configuration is not None:
            self.write_configuration(configuration)

        if self.SupportedFeatures.count('output_format'):
            of = self.read_configuration('system output_format')
            if isinstance(of, str):
                self.parser = RangingPlatformParser(input_format=of)

        self.get_platform_parameters()

    def __repr__(self, level=0):
        prefix = '\t' * level
        s = '%s%s:\n' % (prefix, self.__class__.__name__)
        prefix += '\t'
        if self.connected:
            s += '%sSerial-Port        : %s\n' % (prefix, self.ifc.s.port)
            s += '%sUnique-Id          : %s\n' % (prefix, self.UniqueId)
            s += '%sBoard Number       : %s\n' % (prefix, str(self.BoardNumber))
            s += '%sSW-version         : %s\n' % (prefix, self.SwVersion)
        else:
            s += '%sSerial-Port        : NOT CONNECTED\n' % prefix
            s += '%sUnique-Id          : ???\n' % prefix
            s += '%sBoard Number       : ???\n' % prefix
            s += '%sSW-version         : ???\n' % prefix
        return s

    __str__ = __repr__

    # after writing a configuration the platform will respond with a single line
    def write_configuration(self, conf):
        if self.connected:
            # self.ifc.flush()
            response = ''
            for cmd in conf:
                response += self.ifc.write_with_response('%s\n' % cmd, expected='\n')
            return response
        else:
            return None

    def read_configuration(self, identifier):
        if self.connected:
            msg = self.ifc.write_with_response('%s\n' % identifier, expected='\n')
            msg = msg.replace(': ', ' = ')  # support for YAML response
            idx = msg.find(' = ')
            if idx != -1:  # check for proper response
                msg = msg[idx + 3:]
                return msg.strip()  # remove all trailing whitespace, including newline
            else:
                return None
        else:
            return None

    def defaults(self, reload=True):
        # loading factory settings will also restore verbosity/verbose settings resulting in possible
        # delayed messages. to prevent this the DefaultConfig is done asap.
        self.write_configuration(['system factory'] + self.DefaultConfig)
        if reload:
            self.get_platform_parameters()

    def enable_raw_data(self, val):
        pass

    # determine supported features of board and retrieve relevant parameters
    def get_supported_features(self):
        time.sleep(0.1)
        feature_dct = {
            'ant_mode': r'^ant_mode.*$',
            'mciq': r'^mode_type.*$',
            'tof': r'^mode_type.*$',
            'baudrate': r'^baudrate.*$',
            'multipleconnections': r'^remote.*address>\]\*.*$',
            'grouprange': r'^grouprange.*$',
            'output_format': r'^output_format.*$',
        }
        end_of_rsp = '-------------end----------------\n'
        resp = []
        resp.extend(self.ifc.write_with_response('help\n', expected=end_of_rsp).split('\n'))
        resp.extend(self.ifc.write_with_response('communication\n', expected=end_of_rsp).split('\n'))
        resp.extend(self.ifc.write_with_response('system\n', expected=end_of_rsp).split('\n'))
        resp.extend(self.ifc.write_with_response('parameter\n', expected=end_of_rsp).split('\n'))
        resp.extend(self.ifc.write_with_response('parameter capabilities\n', expected=end_of_rsp).split('\n'))
        for line in resp:
            for k, v in list(feature_dct.items()):
                if re.match(v, line) is not None:
                    self.SupportedFeatures.append(k)

    def get_platform_parameters(self):
        if self.SupportedFeatures.count('tof'):
            val = 25.0  # fallback
            self.platform_config('circle_of_trust', val, update=False)
            val = 85  # fallback
            self.platform_config('percentage_carriers', val, update=False)

    def set_measurement_repetition(self, useprocrepeat, repetitions):
        self.proc_repeat_mode = useprocrepeat
        if useprocrepeat:
            self.write_configuration(['parameter repeat %d' % (int(repetitions))])

    def set_measurement_hadm_mode(self, mainmodeArray, submode):
        # mainmodeArray = [main mode type, min, max, rep]
        mainModeDefault = [MeasurementModes.MCIQ, 4, 8, 1]
        if len(mainmodeArray) < 2:
            mainmodeArray.append(mainModeDefault[1])
        if len(mainmodeArray) < 3:
            mainmodeArray.append(mainModeDefault[2])
        if len(mainmodeArray) < 4:
            mainmodeArray.append(mainModeDefault[3])
        mode = int(mainmodeArray[0])

        self.write_configuration(['parameter main_mode_nb %d %d %d' % (int(mainmodeArray[1]), int(mainmodeArray[2]), int(mainmodeArray[3]))])
        self.write_configuration(['parameter mode_type %d %s' % (mode, '' if submode is None else str(submode))])
        # Encode mode according to MeasurementModes.XXX bits

        if submode is not None:
            mode |= int(submode)
        self.platform_config('mode', mode)

    # Legacy API, deprecated
    def set_measurement_mode(self, mode=MeasurementModes.MCIQ, **kwargs):
        if (mode & MeasurementModes.ToF) and not self.SupportedFeatures.count('tof'):
            assert False, 'ToF Measurement mode is not supported by this board'
        if (mode & MeasurementModes.Group) and not self.SupportedFeatures.count('grouprange'):
            assert False, 'Group Measurement mode is not supported by this board'

        #remotes = ''
        #if kwargs.get('refl_boardnumber', False) and not (mode & MeasurementModes.Group):
        #    refl_address = self.boardnumber2address(kwargs.get('refl_boardnumber'))
        #    remotes = '0x%x' % refl_address
        #init_address = self.boardnumber2address(kwargs.get('init_boardnumber'))
        #if init_address is None:
        #    init_address = self.boardnumber2address(self.BoardNumber)
        #if remotes != '' or (mode & MeasurementModes.Group):
        #    remotes += ' 0x%x' % init_address
        #if remotes != '' and self.SupportedFeatures.count('multipleconnections') == 0:
        #    assert False, 'Dynamically setting the Initiator/Reflector is not supported by this board (use point2point)'
        #self.platform_config('init_boardnumber', kwargs.get('init_boardnumber'), update=False)
        #self.platform_config('refl_boardnumber', kwargs.get('refl_boardnumber'), update=False)

    def set_measurement_execution_mode(self, **kwargs):
        if kwargs.get('testmode', False):
            self.testmode = True
            # Test mode
            if self.csrole == 'reflector':
                cmd = 'test rx\n'
            else:
                cmd = 'test tx\n'
        else:
            self.testmode = False
            # Connected mode
            if kwargs.get('rasrole', '') == 'server':
                cmd = ''  # Do not send any command, just expect local result
            else:
                cmd = 'range\n'

        self.platform_config('range_cmd', cmd, update=False)

    def set_remote_address(self, values, wait_on_connection=True):
        if not isinstance(values, list):
            values = [values]
        self.platform_config('remote_boardnumbers', list(map(self.address2boardnumber, values)))
        remotes = '0x0'
        if len(values):
            remotes = ' '.join(['0x%x' % x for x in values])
        if self.SupportedFeatures.count('multipleconnections'):
            self.write_configuration(['communication remote %s' % remotes])
        else:
            self.write_configuration(['communication reflector %s' % remotes])
        if wait_on_connection:
            return self.wait_on_connection()

    def set_own_address(self, val):
        if val is None:
            val = self.BoardNumber
        self.write_configuration(['communication own 0x%x' % val])

    def store_parameters(self):
        self.write_configuration(['system store'])

    def set_circle_of_trust(self, val):
        val = float(val)
        assert val > 0.0
        self.write_configuration(['misc trust_circle %d' % val])
        self.platform_config('circle_of_trust', val)

    def set_percentage_carriers(self, val):
        val = int(val)
        assert 0 < val < 101
        self.write_configuration(['misc percentage_carriers %d' % val])
        self.platform_config('percentage_carriers', val)

    # API kept for backward compatibility with external applications
    def set_freq_range_step_L(self, fstart=2400e6, fstop=2479e6, deltaf=1e6, L=None):
        pass

    def set_tx_pwr(self, val):
        self.write_configuration(['parameter tx_pwr %d' % int(val)])

    def set_xtal_trim(self, val):
        assert 0 <= int(val) < 128
        self.write_configuration(['misc xtal_trim %d' % int(val)])

    def set_dc_comp(self, val):
        assert int(val) in (0, 3)
        self.write_configuration(['parameter capabilities dc_comp %d' % int(val)])

    def set_calibration_mode(self):
        # Set Tof and MCIQ zero-distance calibration to 0. Don't make it permanent
        self.write_configuration(['system calibrate 0 0'])

    def board_initialize(self):
        pass

    # remote_addr is optionally provided in case the role is central.
    # If not provided, the central will attempt to connect to the first NXP_WR uuid (useful when using -R option, i.e. remote addr is not available)
    def set_role(self, role, remote_addr):
        self.bleRole = role
        if role.lower() == 'central':
            if remote_addr is not None:
                self.write_configuration(['communication remote {}'.format(remote_addr)])
            self.write_configuration(['communication role central'])
        elif role.lower() == 'none':
            self.write_configuration(['communication role none'])
        else:
            self.write_configuration(['communication role peripheral'])

    def set_csrole(self, role):
        if role.lower() == 'reflector':
            self.csrole = 'reflector'
        else:
            self.csrole = 'initiator'

        self.write_configuration(['param role {}'.format(self.csrole)])

    def get_unique_id(self):
        return self.read_configuration('system unique_id')

    def get_sw_version(self):
        return self.read_configuration('system version')

    # process the received result dict. e.g. convert phase/mag to IQ, convert CFO values to SI units
    # validation only checks the mode data fields (e.g. mciq, tof)
    def range_measurement_pre_process(self, result):
        # purge mode-data and set has_<mode>
        for mode in MeasurementModes.all_types(to_str=True):
            if mode in self._platform_config['active_modes']:
                result[f'tof.cfg.radius_cot'] = self._platform_config['circle_of_trust']
                result[f'meta.has_{mode}'] = True
            else:
                result.pop(mode, None)
                result[f'meta.has_{mode}'] = False

        # set timing of modes
        try:
            for mode in self._platform_config['active_modes']:
                if mode != 'mciq':
                    continue
                n_pairs = result.get(f'{mode}.cfg.n_ap', 1)
                formula = self.get_swinfo('t_intra')
                result.setdefault(f'{mode}.cfg.t_intra', formula[0] + n_pairs * formula[1])
                formula = self.get_swinfo('t_inter')
                result.setdefault(f'{mode}.cfg.t_inter', formula[0] + n_pairs * formula[1])
                result.setdefault(f'{mode}.cfg.t_intra_antenna', self.get_swinfo('t_antenna'))  # FIXME: match names
        except KeyError:
            pass

        tof_scaling = self.get_swinfo('tof_scaling')
        for role in self.ROLES:
            try:
                result[f'tof.{role}.time'] = result.get(f'tof.{role}.d') * tof_scaling
            except (KeyError, TypeError, ValueError):
                pass

        mag_scaling = self.get_swinfo('mag_scaling')
        phase_scaling = self.get_swinfo('phase_scaling')

        for role in self.ROLES:
            try:
                result[f'mciq.{role}.iq'] = result.get(f'mciq.{role}.mag') * np.exp(1j * result.get(f'mciq.{role}.phase') * phase_scaling)
            except (KeyError, TypeError, ValueError):
                pass
        for role in self.ROLES:
            try:
                result[f'mciq.{role}.iq'] = result.get(f'mciq.{role}.i') + 1j * result.get(f'mciq.{role}.q')
            except (KeyError, TypeError, ValueError):
                pass
        for role in self.ROLES:
            try:
                result[f'mciq.{role}.iq'] = mag_scaling * result[f'mciq.{role}.iq']
            except (KeyError, TypeError):
                pass

        cfo_scaling = self.get_swinfo('cfo_scaling')
        platform = self.__class__.__name__.split('_')[1]
        for role in self.ROLES:
            result[f'meta.{role}.platform'] = platform
            # apply cfo scaling
            for mode in self._platform_config['active_modes']:
                result[f'{mode}.{role}.cfo'] = cfo_scaling * result.get(f'{mode}.{role}.cfo', 0)
            # convert unique_id to board_nr
            uniqueid = result.get(f'meta.{role}.unique_id', '0')
            result[f'meta.{role}.unique_id'] = uniqueid
            result[f'meta.{role}.board_nr'] = 0
            boardnr = self.uniqueid2boardnumber(uniqueid)
            if boardnr is not None:
                result[f'meta.{role}.board_nr'] = boardnr
            #elif result['meta.error_msg'] == '':
            #    result['meta.error_msg'] = 'unknownuniqueid'

        # Various checks on received response if available
        if result['meta.error_msg'] == '':
            #if result.get('meta.initiator.sw_version') != result.get('meta.reflector.sw_version'):
            #    result['meta.error_msg'] = 'swversionmismatch'
            if result.get('hadm.sts', 0) != 0x00:
                result['meta.error_msg'] = 'HADM event report error'

            # check if response is from an expected board
            #if self._platform_response['init_boardnumber'] is not None:
            #    if self._platform_response['init_boardnumber'] != result['meta.initiator.board_nr']:
            #        result['meta.error_msg'] = 'unexpecteduniqueid'
            #if len(self._platform_response['refl_boardnumbers']):
            #    if self._platform_response['refl_boardnumbers'].count(result['meta.reflector.board_nr']) != 1:
            #        result['meta.error_msg'] = 'unexpecteduniqueid'

    # Build a map stepId->localIdx where localIdx applies to mode0, pkt(tof) or tone(mciq) arrays
    def build_hadm_step_map(self, result):
        mode0_cnt = 0
        tof_cnt = 0
        mciq_cnt = 0
        stepNb = result.get('hadm.stp.nb', 0)
        result['hadm.stp.m0_idx'] = []
        result['hadm.stp.tof_idx'] = []
        result['hadm.stp.mciq_idx'] = []
        for stepId in range(0, stepNb):
            if result.get('hadm.stp.modes')[stepId] == 0:
                result['hadm.stp.m0_idx'].append(stepId)
                mode0_cnt += 1
            if (result.get('hadm.stp.modes')[stepId] == 1) or (result.get('hadm.stp.modes')[stepId] == 3):
                result['hadm.stp.tof_idx'].append(stepId)
                tof_cnt += 1
            if (result.get('hadm.stp.modes')[stepId] == 2) or (result.get('hadm.stp.modes')[stepId] == 3):
                result['hadm.stp.mciq_idx'].append(stepId)
                mciq_cnt += 1

    # validation checks all fields
    def range_measurement_post_process(self, result):
        self.resultfile.validate(result, append=False)

    def run_range_command(self, **kwargs):
        if kwargs.get('response') is None:
            if kwargs.get('timeout', 4.0) > 4.0:
                print(">> DUT waiting for other device to start.")
            if kwargs.get('pause', False):
                print(">> Press <enter> to start measurement.")
                input()
            meas_nr = kwargs.get('overlay')['meta.testcase.meas_nr']
            # In connected mode, we use CS procedure repeat to trigger the measurement N times,
            # so for subsequent iterations, read unsolicited response over serial link
            if self.testmode or meas_nr == 0 or not self.proc_repeat_mode:
                resp = self.ifc.write_with_response(self._platform_config['range_cmd'],
                                                    expected=self._platform_response['marker'],
                                                    timeout=kwargs.get('timeout', 2.0),
                                                    flush=False)
            else:
                resp = self.ifc.read(expected=self._platform_response['marker'],
                                     timeout=kwargs.get('timeout', 2.0))
        else:
            resp = kwargs.get('response')
        results = self.parser.do(resp)
        for n, result in enumerate(results):
            result.update('', kwargs.get('overlay', {}))
        self.resultfile.extend(results)
        return results

    def process_range_result(self, task_tmr, results, **kwargs):
        execAlgo = kwargs.get('execAlgo', True)

        for n, result in enumerate(results):
            # Catch any error at measurement so that an issue would only affect current result
            try:
                self.range_measurement_pre_process(result)
                task_tmr.checkpoint('t_pre_process')

                if execAlgo and result['meta.error_msg'] == '':
                   self.resultfile.validate(result, pre=True)

                # If no error found during measurement, perform post processing
                if execAlgo and result['meta.error_msg'] == '':
                    self.build_hadm_step_map(result)
                    self.distance_bounding_calculation(result)
                    task_tmr.checkpoint('t_calc_tof')
                    if not self.AlgorithmExec:  # only when the distance calc was not done
                       self.distance_calculation(result)
                    task_tmr.checkpoint('t_calc_mciq')
                    self.range_measurement_post_process(result)
                    task_tmr.checkpoint('t_post_process')

                # profiling fields are NOT validated. timing is only available after everything is executed. Validation
                # should be included in the timing (chicken/egg problem).
                result['meta.profiling.t_total'] = 0.0
                result['meta.profiling.t_reference'] = round(time.time(), 6)
                for task in list(task_tmr.cur_time.keys()):
                    result[f'meta.profiling.{task}'] = round(float(task_tmr.cur_time[task]), 6)
                    result['meta.profiling.t_total'] += result[f'meta.profiling.{task}']
                result['meta.profiling.t_total'] = round(result['meta.profiling.t_total'], 6)
                task_tmr.restart()
            except Exception as e:
                # Catch any exception and log it in order to continue execution
                print(traceback.format_exc())
        return results

    def run_range_measurement(self, **kwargs):
        task_tmr = xTimer(average=False, logger=None)
        results = self.run_range_command(**kwargs)
        task_tmr.checkpoint('t_measurement')
        results = self.process_range_result(task_tmr, results=results, **kwargs)
        return results

    @staticmethod
    def cfo_calculation_from_rttstat(rttstat, bitrate):
        # extract RTT_STAT.RTT_CFO from register value
        cfo = (rttstat & 0xFFFF)
        cfo = np.short(cfo)
        # Approximate convertion from sfix16En15: CFO_hz=RTT_CFO*bitrate/2
        cfo = cfo * 15 if (bitrate == 1) else 30
        return np.ndarray.tolist(cfo)

    def compute_diagnose_data(self, result):
        if result['meta.error_msg'] != '' or not result['meta.has_tof']:
            return

        # Compute number of errors and CFO based on HARTT result
        try:
            # Number of RTT errors
            result['diag.rtt.init.nb_errors'] = sum(1 for q in result['tof.initiator.quality'] if (q != 0))
            result['diag.rtt.refl.nb_errors'] = sum(1 for q in result['tof.reflector.quality'] if (q != 0))
        except:
            # diag key may not exist
            pass

    def distance_bounding_calculation(self, result):

        if result['meta.error_msg'] != '' or not result['meta.has_tof']:
            return

        t_verifier = result['tof.initiator.d']
        t_prover = result['tof.reflector.d']
        good_indx = (result['tof.initiator.quality'] == 0) & (result['tof.reflector.quality'] == 0)
        result['tof.result.init_nadm'] = int(np.amax(result['tof.initiator.nadm']))
        result['tof.result.refl_nadm'] = int(np.amax(result['tof.reflector.nadm']))

        dm = 0
        distance = np.nan
        successrate = 0
        tof_std = np.nan

        tof = t_verifier - t_prover    # ns unit
        n_carriers = tof.shape[0]

        tof = tof[good_indx]
        tof_valid = tof.shape[0]
        #min_carriers = int(n_carriers * float(self._platform_config['percentage_carriers']) / 100.0)
        if tof_valid > 0:
            tof_avg = np.mean(tof)/2  # ToF is RTT/2
            tof_std = np.std(tof)/2  # ToF standard deviation
            distance = tof_avg * 0.3  # meters
            if distance <= self._platform_config['circle_of_trust']:
                dm = 1
            successrate = tof_valid / float(n_carriers) * 100.0

        # Do not override result if provided by FW
        if result.get('tof.result.distance', None) is None:
            result['tof.result.distance'] = distance
            result['tof.result.successrate'] = int(successrate)

        # not provided by FW
        result['tof.result.in_cot'] = True if dm else False
        result['tof.result.std'] = tof_std * 0.3  # meters

    # using antenna_pair_list it is possible to select one specific antenna or to use all antenna_pairs!
    def distance_calculation(self, result, antenna_pair_list=list(range(8))):
        result.setdefault('mciq.result.cfo', np.nan)
        result.setdefault('mciq.result.distance', np.nan)
        result.setdefault('mciq.result.aoa', np.nan)
        result.setdefault('mciq.result.likeliness', np.nan)
        # Check that there are enough frequencies to run algorithm (hard limit in C library)
        if result.get('mciq.cfg.n_stp', 0) <= 40:
            return
        if self.AlgorithmExec:
            self.distance_calculation_embedded(result=result, antenna_pair_list=antenna_pair_list)
        else:
            self.distance_calculation_direct(result=result, antenna_pair_list=antenna_pair_list)

    def distance_calculation_embedded(self, result, antenna_pair_list):
        self.distance_calculation_direct(result=result, antenna_pair_list=antenna_pair_list)

    # Compute slope_rmse and slope per AP and compute minimal distance from those paths
    def calculate_slope_rmse(self, cr_out):
        nPairs, nTones = cr_out['IQ_A'].shape
        slope_rmse = []
        slope = []
        slope_dist = np.nan
        for pair in range(nPairs):
            combined_phase = np.angle(cr_out['IQ_A'][pair]) + np.angle(cr_out['IQ_B'][pair])
            combined_phase = combined_phase[np.where(cr_out['freq_valid'][pair] == 1)]
            combined_phase = np.unwrap(combined_phase)
            x = np.array(cr_out['chan_list'])[np.where(cr_out['freq_valid'][pair] == 1)]
            fit = np.polyfit(x, combined_phase, 1)
            slope_dist = np.nanmin((slope_dist, ((-1) * fit[0] * 3.0e8) / (4 * np.pi * 1.0e6)))
            trend_line = [fit[0]*f + fit[1] for f in x]
            error = np.subtract(combined_phase, trend_line)
            slope.append(round(fit[0], 3))
            slope_rmse.append(round(np.sqrt((np.square(error)).mean()) * 180 / np.pi, 3))
        return slope, slope_rmse, slope_dist

    def distance_calculation_direct(self, result, antenna_pair_list):
        if result['meta.error_msg'] != '' or not result['meta.has_mciq']:
            return
        zero_dist_cal = 0
        # FIXME
        #  result['meta.initiator.zdc_mciq'] + result['meta.reflector.zdc_mciq']
        #  zero_dist_cal = zero_dist_cal + 0.004  # small offset btw CDE and MUSIC
        if (self.Calibrate is not None) and (zero_dist_cal != 0):
            assert False, 'Initiator or Reflector board has returned a non-zero calibration value during calibration phase'
            return

        n_pairs = result['mciq.cfg.n_ap']
        if len(antenna_pair_list) == -1 or len(antenna_pair_list) > n_pairs:
            antenna_pair_list = range(n_pairs)

        try:
            # Populate IQ results in 2 dimensions (ant, channels)
            iq_a = result['mciq.initiator.iq'][:, antenna_pair_list]
            iq_b = result['mciq.reflector.iq'][:, antenna_pair_list]
            # Build channel list for which we got some tones (channels for step modes 2 or 3)
            chanListIq = np.fromiter([result['hadm.stp.channels'][result['hadm.stp.mciq_idx'][i]] for i in range(len(result['hadm.stp.mciq_idx']))], int)

            cr_out={}
            cr_out['IQ_A']=np.swapaxes(iq_a,0,1)
            cr_out['IQ_B']=np.swapaxes(iq_b,0,1)

            nPairs, nTones = cr_out['IQ_A'].shape
            assert (nTones == len(chanListIq))

            # Insert missing channels in order to get contiguous channel list
            chanMin = min(chanListIq)
            chanMax = max(chanListIq)
            cr_out['chan_list'] = list(range(chanMin, chanMax + 1))

            # Sort IQ vectors vs frequency in ascending order as if it was a linear sweep
            chanDict = {k: v for v, k in enumerate(chanListIq)}
            tmp_IQ_A = np.zeros(shape=(nPairs, chanMax - chanMin + 1), dtype=np.csingle)
            tmp_IQ_B = np.zeros(shape=(nPairs, chanMax - chanMin + 1), dtype=np.csingle)
            tmp_freq_valid = np.zeros(shape=(nPairs, chanMax - chanMin + 1), dtype=int)

            for pair in range(nPairs):
                # Store IQ per channel (may override previous IQ value if channel is used twice)
                for k in chanDict:
                    tmp_IQ_A[pair][k - chanMin] = cr_out['IQ_A'][pair][chanDict[k]]
                    tmp_IQ_B[pair][k - chanMin] = cr_out['IQ_B'][pair][chanDict[k]]
                    tmp_freq_valid[pair][k - chanMin] = 1
                # For invalid frequencies, we will use I&Q=0, that's the best we can do for channel reconstruction
                for k in range(0, chanMax - chanMin + 1):
                    if tmp_freq_valid[pair][k] == 0:
                        tmp_IQ_A[pair][k] = tmp_IQ_A[pair][k - 1]
                        tmp_IQ_B[pair][k] = tmp_IQ_B[pair][k - 1]

            cr_out['IQ_A'] = np.delete(cr_out['IQ_A'], pair, axis=0)
            cr_out['IQ_B'] = np.delete(cr_out['IQ_B'], pair, axis=0)
            cr_out['IQ_A'] = tmp_IQ_A
            cr_out['IQ_B'] = tmp_IQ_B
            cr_out['freq_valid'] = tmp_freq_valid

            slope, slope_rmse, slope_dist = self.calculate_slope_rmse(cr_out)

            result['mciq.result.slope_rmse'] = slope_rmse
            result['mciq.result.slope'] = slope
            result['mciq.result.distance'] = slope_dist - zero_dist_cal
        except KeyError as e:
            print(e)
            #  No IQ means that only embedded algorithm is required, not an error
            pass
