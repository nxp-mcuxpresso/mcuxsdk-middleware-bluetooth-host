#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------
#
# Copyright 2021, 2023 NXP
# 
# NXP Proprietary
#

import sys
import os
import numpy as np
import logging.config
import time
import json
import argparse
from queue import Queue
from threading import Thread

curdir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(curdir, "..", "common"))
sys.path.append(os.path.join(curdir, "..", "..", "Instruments"))
sys.path.append(os.path.join(curdir, "..", "..", "Generic"))
logging.config.fileConfig(os.path.join(curdir, "..", "..", 'logging.ini'))

from ranging_platform_pkg import RangingPlatform
from ranging_platform_pkg.utils import platform_connect, platform_measurement, inject_cfo
from ranging_platform_pkg.rng_platform_types import MeasurementModes
from valueArray import valueArray
from sql_db import sql_db
from nesteddict import NestedDict
from stringutils import str_to_bool
from dirutils import ensure_dir

def list_str(values):
    return values.split(',')

def main(argv):

    log = logging.getLogger(__name__)
    log.info('Working on %s' % sys.platform)
    parser = argparse.ArgumentParser(description='Run measurements on RangingPlatform')
    parser.add_argument('--ws', action='store_true', help='use websocket based data transfer (file-based by default)')
    parser.add_argument('--local', action='store_true',
                        help='store settings/data in the working directory (webserver path otherwise)')
    parser.add_argument('--clear', action='store_true', help='delete settings/data in the working directory')
    parser.add_argument('--debug', action='store', type=int, default=0,
                        help='Script debug verbosity, default=0')
    parser.add_argument('--mainmode', action='store', type=list_str, default=[MeasurementModes.MCIQ],
                        help='Main_mode_type (1:RTT,2:RTP), Main_Mode_Min_Steps, Main_Mode_Max_Steps, Main_Mode_Repetition')
    parser.add_argument('--tof', action='store', type=int, default=None,
                        help='Tof enable or not (1 enable, 0 disable)')
    parser.add_argument('--comport', action='append', type=str, default=[],
                        help='add one or two comports to connect to (Initiator and Reflector respectively)')
    parser.add_argument('--dutconfig', action='append', type=str, default=[],
                        help='a string to pass to dut.write_configuration')
    parser.add_argument('--usemaxbaudrate', action='store', type=str_to_bool, default=True,
                        help='use the maximum baudrate (1; default) or the default baudrate (0).')
    parser.add_argument('--algo', action='store', type=list_str, default=[2,1],
                        help='Distance estimation algo to be run on Host side (NOT SUPPORTED YET ON HOST SIDE)(0:None, 2:SRDE) and embedded side(0:None, 1:CDE, 2:SRDE, 3:CDE+SRDE')
    parser.add_argument('-R', action='store_true', help='disable comport connection to reflector')

    args = parser.parse_args(argv)
    args.remote = []
    args.submode = None


    algo_list = [int(i) for i in args.algo]

    timestamp = time.strftime("%Y%m%dT%H%M%S")

    info_filename = './info.json'
    db_filename = './ranging.db'
    if args.clear:
        files = [info_filename, db_filename]
        for f in files:
            if os.path.exists(f):
                os.remove(f)

    if not args.local:
        info_filename = '/dev/shm/info.json'
        db_filename = '/var/www/html/data/ranging.db'

    if args.ws:
        websocket_server()
    else:
        file_server(info_filename)

    filter_params = {
        'mciq_filter': 'kalman',
        'mciq_window': 5,
        'mciq_max': 20.0,
        'mciq_min': 0.0,
        'mciq_hysteresis': 1.0,  # JumpFilter only
        'mciq_d_std': 1.0,  # Kalman only
        'mciq_a_std': 1.0,  # Kalman only
        'tof_window': 2,
        'tof_max': 20.0,
        'tof_min': 0.0,
        'tof_hysteresis': 0.0
    }
    # boolean variables must have a _bl postfix (needed for correct handling)
    # list variables must have a _lst postfix and may not be empty (needed for correct handling)
    # list items are converted to lower case

    if args.tof == 1:
        args.submode = 1
        enable_tof = True
    else:
        enable_tof = False
    config_defaults = {
        'command': '',
        'board_lst': ['kw38', 'kw45', 'kw36'],
        'board': 'kw45',
        'known_board_number_lst': list(range(8)),
        'board_number_0': 1,
        'board_number_1': 1,
        'ant_mode': 0,
        'no_tones': 80,
        'cli_verb' : 4,
        'ena_tof_bl': enable_tof,
        'raw_dist' : True,
        'plot_mciq_raw_bl': False,
        'plot_tof_bl': False,
        'plot_tof_raw_bl': False,
        'plot_likeliness_bl': False,
        'plot_likeliness_srde': False,
        'plot_likeliness_cde': False,
        'plot_dist_srde': True,
        'plot_dist_cde': True,
        'display_rssi_bl': True,
        'display_cfo_bl': True,
        'display_tof_bl': True,
        'display_velocity_bl': True,
        'display_likeliness_bl': True,
        'display_security_bl': False,
        'display_authentication_bl': False,
        'retry_delay': 1.0,
        'unit_feet_bl': False,
        'graph_width': 40,
        'mciq_offset': -0.4,
        'tof_offset': 0.0,
        'tof_margin': 10.0,
        'y_axis_min_enable': False,
        'y_axis_min': 7.0,
        'y_axis_range': 10.0,
    }


    db = sql_db(db_filename, config_defaults)

    dut = None
    board = None
    initialized = False
    info = {
        'summary': '',
        'unit': 'm',
        'status': 'connecting',
        'period': 1000,
        'graph_data': [],
        'graph_labels': [],
        'graph_bar': False
    }

    # main connection loop
    while True:
        db.reload()

        info['status'] = 'connecting'
        queue.put(json.dumps(info))

        if db.get('command') == 'dbrotate':
            log.info("Rotating DB")
            db.rotate()
            board = None  # force an update of the board list..
        db.set('command', '')

        if dut is not None:
            dut.ifc.close()

        if initialized:
            time.sleep(db.get('retry_delay'))
        initialized = True

        #This ranging plateform could be removed since it is overrided by later connect
        if db.get('board') != board:
            board = db.get('board')
            log.info("Using %s RangingPlatform" % (db.get('board')))

            dut = RangingPlatform(board=db.get('board'), usemaxbaudrate=args.usemaxbaudrate,
                                   ResultFile={'max_entries': 1},debug=args.debug)

            # update available boards
            known_board_number_lst = dut.get_boardnumber_list()
            known_board_number_lst.sort()
            db.set('known_board_number_lst', known_board_number_lst)

        port_lst = dut.get_serial_ports()
        if len(port_lst) == 0:
             log.error("Unable to find serialport.")
             continue

        log.info("Using Initiator-port: %s" % (port_lst[0]))

        board_number_lst = []
        for n in range(8):
            brd = db.get(f'board_number_{n}')
            if brd is None:
                break
            if board_number_lst.count(brd) == 0:
                board_number_lst.append(brd)

        refl_data = []
        for brd in board_number_lst:
            tmp = {}
            arr = valueArray(filter=filter_params.get('mciq_filter'),
                             verbose=False,
                             depth=db.get('graph_width'),
                             offset=db.get('mciq_offset'),
                             bounds_lower=filter_params.get('mciq_min'),
                             bounds_upper=filter_params.get('mciq_max'),
                             flt_d_std=filter_params.get('mciq_d_std'),
                             flt_a_std=filter_params.get('mciq_a_std'),
                             flt_hysteresis=filter_params.get('mciq_hysteresis'),
                             flt_window=filter_params.get('mciq_window'))
            tmp['mciq'] = arr

            arr = valueArray(filter=filter_params.get('mciq_filter'),
                            verbose=True,
                            depth=db.get('graph_width'),
                            offset=0.0,
                            # flt_window=6,
                            label='CDE Distance')

            tmp['cde_dist']= arr
            arr = valueArray(filter=filter_params.get('mciq_filter'),
                            verbose=True,
                             depth=db.get('graph_width'),
                             offset=0.0,
                            # flt_window=7,
                             label='SRDE Distance'
                             )
            tmp['srde_dist']=arr

            arr = valueArray(verbose=True,
                             depth=db.get('graph_width'),
                             offset=db.get('tof_offset'),
                             flt_hysteresis=filter_params.get('tof_hysteresis'),
                             flt_window=filter_params.get('tof_window'),
                             bounds_lower=filter_params.get('tof_min'),
                             bounds_upper=filter_params.get('tof_max'),
                             label='ToF Distance')
            tmp['tof'] = arr

            arr = valueArray(verbose=False,
                             depth=db.get('graph_width'),
                             offset=0.0,
                             label='Likeliness',
                             fmt='%.2f',
                             bounds_lower=0.0,
                             bounds_upper=1.0)
            tmp['likeliness'] = arr

            arr = valueArray(verbose=True,
                             depth=db.get('graph_width'),
                             offset=0.0,
                             label='CDE Likeliness',
                             fmt='%.2f',
                             bounds_lower=0.0,
                             bounds_upper=1.0)
            tmp['cde_likeliness']= arr

            arr = valueArray(verbose=True,
                             depth=db.get('graph_width'),
                             offset=0.0,
                             label='SRDE Likeliness',
                             fmt='%.2f',
                             bounds_lower=0.0,
                             bounds_upper=1.0)
            tmp['srde_likeliness']= arr

            refl_data.append(tmp)

            del tmp

        try:
            board_address_lst = list(map(dut.boardnumber2address, board_number_lst))
            log.debug(dut)
        except Exception as e:
            log.error(e, exc_info=False)
            log.error('Failed to retrieve remote info', exc_info=False)
            continue
        try:
            dut.enable_raw_data(0)
            #N = db.get('no_tones')

            #dut.set_freq_range_step_L(fstart=2401e6, fstop=2480e6, deltaf=(80 / N)*1e6, L=N/2)
            measmode = MeasurementModes.MCIQ
            if db.get('ena_tof_bl'):
                measmode |= MeasurementModes.ToF
                log.info('Enable measuremode MCIQ+ToF')
            else:
                log.info('Enable measuremode MCIQ')
            if len(board_address_lst) > 1:
                measmode |= MeasurementModes.Group
                log.info('Enable Groupranging')

            # role assignment process from record_ranging
            init = {'comport': 'auto', 'unique_id': 'auto'}
            refl = {'comport': 'auto', 'unique_id': 'auto'}
            if len(args.comport) == 1:
                init['comport'] = args.comport[0]
            if len(args.comport) == 2:
                refl['comport'] = args.comport[1]
            if args.R:
                refl['comport'] = 'none'
            sut = platform_connect(board, init, refl, usemaxbaudrate=args.usemaxbaudrate, cal=None,debug=args.debug)
            dut = sut['dut']
            peer = sut['peer']
            mmargs = dict()
            remotes = board_address_lst
            if len(board_address_lst) > 1:
                dut.set_remote_address(remotes, wait_on_connection=False)
                mmargs['init_boardnumber'] = None
            # Apply connection roles, will start connection activity
            time.sleep(0.1)  # Give some time to FW to process the configuration
            dut.set_csrole('Initiator')
            dut.set_role('central')
            try:
                peer.set_role('peripheral')
            except Exception:
                pass  # No reflector to configure
            # end of role assignment
            dut.set_measurement_hadm_mode(args.mainmode, args.submode)

            mmargs['rasrole'] = 'client'
            dut.set_measurement_execution_mode(**mmargs)

            args.dutconfig.insert(0, "sys verbosity %d" % db.get('cli_verb'))  # CLI output
            if args.dutconfig:
                dut.write_configuration(args.dutconfig)
            if db.get('display_rssi_bl') or db.get('display_cfo_bl') :
                dut.write_configuration(["parameter debug %d" % 4 ])
            if db.get('board') in ['kw36', 'kw45', 'kw38']:
                dut.write_configuration(['parameter capabilities ant_mode %d' % db.get('ant_mode')])
            if algo_list[1] != 0:  # Configure embedded algo
                dut.write_configuration(['misc rtp_algo %d' % algo_list[1]])

        except:
            log.error('Failed to configure board')
            continue

        connected = dut.wait_on_connection()
        if not connected:
            raise Exception('Wireless connection not ready!')

        sample_idx = 0
        no_errors = 0
        results_prev = None
        info['status'] = 'active'
        # ranging loop
        while True:
            # update configuration
            db.reload()

            if db.isanymodified():
                log.info('Reinitializing board (a parameter was modified)')
                break

            unit = 'm'
            unit_mult = 1.0
            if db.get('unit_feet_bl'):
                unit = 'ft'
                unit_mult = 3.28
            info['unit'] = unit
            if db.get('y_axis_min_enable'):
                info['y_axis_min'] = db.get('y_axis_min')
            info['graph_data'] = []
            info['graph_bar'] = False
            info['graph_labels'] = []
            info['colors'] = []
            info['summary'] = ''

            try:
                #reflectors = []
                #reflectors.append(dut.BoardNumber)
                overlay = NestedDict( {})
                # Process measurements results
                results = dut.run_range_measurement(execAlgo = False,overlay=overlay, rangeCmdTimeout=4.0,debug=args.debug)
            except:
                log.error('run_range_measurement failed')
                break

            isvalid = results[0]['meta.error_msg'] == ''
            # determine activity
            if not isvalid:
                no_errors += 1
                log.error('Measurement error msg: ' + results[0]['meta.error_msg'])
                dut.wait_on_connection()
            else:
                no_errors = 0

            if('mciq.result.srderr'  in results[0]):
                isvalid_srde = results[0]['mciq.result.srderr'] == 0
                if not isvalid_srde:
                    no_errors += 1
                    log.error('Measurement SRDE error msg: ' + str(results[0]['mciq.result.srderr']))
                    dut.wait_on_connection()
                else:
                    no_errors = 0
            else:
                results[0]['mciq.result.srderr'] = 0
            sample_idx += 1

            if len(results) != len(board_number_lst):
                # TODO: handle exception
                continue
            if (results[0]['mciq.result.srderr'] == 0) and (results[0]['meta.error_msg'] == ''):
                for n, result in enumerate(results):
                    if results_prev is not None:
                        diff = result['meta.profiling.t_reference'] - results_prev[n]['meta.profiling.t_reference']
                        info['period'] = int(diff * 1000)  # period is in milliseconds
                        refl_data[n]['mciq'].set_parameters(flt_ts=diff, flt_delta_t=result.get('mciq.cfg.t_inter', 500e-6))
                    results_prev = results

                    label_postfix = ''
                    label_prefix = ''
                    if len(board_number_lst) > 1:
                        label_postfix = f' - {result.get("meta.reflector.board_nr", 0)}'
                        label_prefix = f'Board {result.get("meta.reflector.board_nr", 0)} - '

                    # refl_data[n]['mciq'].insert(result.get('mciq.result.distance', np.nan), isvalid)
                    # refl_data[n]['likeliness'].insert(result.get('mciq.result.likeliness', np.nan), isvalid)


                    #a good thing should be to set distance and likliness parsing string in global format or in database for easy change
                    #for he moment oonly embedded algo is parsed
                    # if  algo_list[1] == 0:
                    #     refl_data[n]['mciq'].insert(result.get('mciq.result.distance', np.nan), isvalid)
                    #     refl_data[n]['likeliness'].insert(result.get('mciq.result.likeliness', np.nan), isvalid)
                    # elif algo_list[1] == 1:
                    #     refl_data[n]['mciq'].insert(result.get('mciq.result.distance', np.nan), isvalid)
                    #     refl_data[n]['likeliness'].insert(result.get('mciq.result.likeliness', np.nan), isvalid)
                    #     refl_data[n]['cde_dist'].insert(result.get('mciq.result.CDE_distance', np.nan), isvalid)
                    #     refl_data[n]['cde_likeliness'].insert(result.get('mciq.result.CDE_dqi', np.nan), isvalid)
                    # elif algo_list[1] == 2:

                    # refl_data[n]['mciq'].insert(result.get('mciq.result.distance', np.nan), isvalid)
                    refl_data[n]['mciq'].insert(result.get('mciq.result.CDE_distances', np.nan), isvalid)
                    refl_data[n]['likeliness'].insert(result.get('mciq.result.likeliness', np.nan), isvalid)
                    refl_data[n]['cde_dist'].insert(result.get('mciq.result.CDE_distance', np.nan), isvalid)
                    refl_data[n]['cde_likeliness'].insert(result.get('mciq.result.CDE_dqi', np.nan), isvalid)
                    refl_data[n]['srde_dist'].insert(result.get('mciq.result.SRDE_distance', np.nan), isvalid)
                    refl_data[n]['srde_likeliness'].insert(result.get('mciq.result.SRDE_dqi', np.nan), isvalid)
                    # else :
                    #     refl_data[n]['mciq'].insert(result.get('mciq.result.distance', np.nan), isvalid)
                    #     refl_data[n]['likeliness'].insert(result.get('mciq.result.likeliness', np.nan), isvalid)

                    refl_info = {}
                    refl_info['distance'] = unit_mult * refl_data[n]['mciq'].raw_lst[-1]
                    refl_info['distance_srde'] = unit_mult * refl_data[n]['srde_dist'].raw_lst[-1]
                    refl_info['distance_cde'] = unit_mult * refl_data[n]['cde_dist'].raw_lst[-1]
                    refl_info['tof_distance'] = None
                    # Default y_axis_range then retrive specified one if any
                    info['y_axis_range'] = 15.0
                    info['y_axis_range'] = db.get('y_axis_range')
                    refl_info['rssi'] = {}
                    refl_info['rssi']['init'] = result.get('info.init.sync_rssi', np.nan)
                    refl_info['rssi']['refl'] = result.get('info.refl.sync_rssi', np.nan)
                    refl_info['likeliness'] = round(refl_data[n]['likeliness'].raw_lst[-1], 2)
                    refl_info['likeliness_srde'] = round(refl_data[n]['srde_likeliness'].raw_lst[-1], 2)
                    refl_info['likeliness_cde'] = round(refl_data[n]['cde_likeliness'].raw_lst[-1], 2)
                    refl_info['cfo'] = result.get('info.init.sync_cfo',  result.get('info.refl.sync_cfo', np.nan))

                    no_samples = len(refl_data[n]['mciq'])
                    if db.get('ena_tof_bl'):
                        refl_data[n]['tof'].insert(result.get('tof.result.distance', np.nan), isvalid)
                        refl_info['tof_distance'] = unit_mult * refl_data[n]['tof'].data_lst[-1]
                        no_samples = len(refl_data[n]['tof'])

                    summary_lst = []

                    if db.get('plot_dist_srde') and refl_data[n]['srde_dist'] is not np.nan and refl_info['distance_srde'] is not np.nan:
                         summary_lst.append(f'SRDE Distance: {refl_info["distance_srde"]}{unit}')
                    if refl_data[n]['srde_likeliness'] is not np.nan and refl_info['likeliness_srde'] is not np.nan:
                        summary_lst.append(f'SRDE Likeliness: {100.0 * refl_info["likeliness_srde"]:03.0f}%')

                    if db.get('plot_dist_cde') and refl_data[n]['cde_dist'] is not np.nan and refl_info['distance_cde'] is not np.nan:
                        summary_lst.append(f'CDE Distance: {refl_info["distance_cde"]}{unit}')
                    if refl_data[n]['cde_likeliness'] is not np.nan and refl_info['likeliness_cde'] is not np.nan:
                        summary_lst.append(f'CDE Likeliness: {100.0 * refl_info["likeliness_cde"]:03.0f}%')




                    # if refl_info['distance'] is not None and (refl_data[n]['srde_dist'] is np.nan and refl_data[n]['cde_dist'] is np.nan):
                   # summary_lst.append(f'Distance: {refl_info["distance"]:.1f}{unit}')
                    if db.get('display_tof_bl') and refl_info['tof_distance'] is not None:
                        summary_lst.append(f'ToF Distance: {refl_info["tof_distance"]:.1f}{unit}')
                    if db.get('display_rssi_bl') and refl_info["rssi"]["init"] is not np.nan and refl_info["rssi"]["refl"] is not np.nan:
                        summary_lst.append(f'RSSI = {refl_info["rssi"]["init"]:03d}dBm/{refl_info["rssi"]["refl"]:03d}dbm')
                    if db.get('display_velocity_bl') and hasattr(refl_data[n]['srde_dist'], 'vel_lst'):
                        summary_lst.append(f'Velocity SRDE: {refl_data[n]["srde_dist"].vel_lst[-1]:03.1f}m/s')
                    if db.get('display_likeliness_bl') and refl_info['likeliness'] is not None and (refl_data[n]['cde_likeliness'] is np.nan and refl_data[n]['srde_likeliness'] is np.nan):
                        summary_lst.append(f'Likeliness: {100.0 * refl_info["likeliness"]:03.0f}%')
                    if refl_info['distance'] is not None and refl_info['tof_distance'] is not None:
                        if db.get('display_security_bl'):
                            margin = np.abs(refl_info['distance'] - refl_info['tof_distance'])
                            if margin < db.get('tof_margin'):
                                summary_lst.append('Security: High')
                                if db.get('display_authentication_bl') and (refl_info['distance'] < 3):
                                    summary_lst.append('Authenticated: Yes')
                                else:
                                    summary_lst.append('Authenticated: No')
                            else:
                                summary_lst.append('Security: Low')
                                if db.get('display_authentication_bl'):
                                    summary_lst.append('Authenticated: No')
                    if refl_info['cfo'] is not None and db.get('display_cfo_bl'):
                        summary_lst.append(f'CFO: {refl_info["cfo"]/1000:3.2f}kHz')
                    info['summary'] += label_prefix + ', '.join(summary_lst) + '\n'



                    if n == 0:
                        info['graph_data'].append(np.arange(sample_idx - no_samples, sample_idx + 1).tolist())
                        info['graph_labels'].append('Measurement#')

                    #this draw mciq each time , does same as plot_mciq_raw_bl
                    # info['graph_data'].append(np.around(refl_data[n]['mciq'].data_lst[:no_samples] * unit_mult, 2).tolist())
                    # info['graph_labels'].append(f'Distance{label_postfix}')



                    if db.get('plot_likeliness_srde') and np.around(refl_datarefl_data[n]['srde_likeliness'].raw_lst[:no_samples] * db.get('y_axis_range'), 2).tolist() is not np.nan :
                        info['graph_data'].append(
                            np.around(refl_data[n]['srde_likeliness'].raw_lst[:no_samples] * db.get('y_axis_range'), 2).tolist())
                        info['graph_labels'].append(f'SRDE Likeliness{label_postfix}')
                    if db.get('plot_likeliness_cde') and np.around(refl_data[n]['cde_likeliness'].raw_lst[:no_samples] * db.get('y_axis_range'), 2).tolist() is not np.nan:
                        info['graph_data'].append(
                            np.around(refl_data[n]['cde_likeliness'].raw_lst[:no_samples] * db.get('y_axis_range'), 2).tolist())
                        info['graph_labels'].append(f'CDE Likeliness{label_postfix}')

                    if db.get('plot_dist_srde') and np.around(refl_data[n]['srde_dist'].raw_lst[:no_samples] * unit_mult, 2).tolist() is not np.nan:
                        if db.get('raw_dist'):
                            info['graph_data'].append(
                                np.around(refl_data[n]['srde_dist'].raw_lst[:no_samples] * unit_mult, 2).tolist())
                        else :
                            info['graph_data'].append(
                                np.around(refl_data[n]['srde_dist'].data_lst[:no_samples] * unit_mult, 2).tolist())
                        info['colors'].append("red")
                        info['graph_labels'].append(f'SRDE Distance{label_postfix}')
                    if db.get('plot_dist_cde') and refl_data[n]['cde_dist'].raw_lst[:no_samples] is not np.nan:
                        if db.get('raw_dist'):
                            info['graph_data'].append(
                                np.around(refl_data[n]['cde_dist'].raw_lst[:no_samples] * unit_mult, 2).tolist())
                        else:
                            info['graph_data'].append(
                                np.around(refl_data[n]['cde_dist'].data_lst[:no_samples] * unit_mult, 2).tolist())
                        info['colors'].append("green")
                        info['graph_labels'].append(f'CDE Distance{label_postfix}')


                    if db.get('plot_mciq_raw_bl'):
                        info['graph_data'].append(np.around(refl_data[n]['mciq'].raw_lst[:no_samples] * unit_mult, 2).tolist())
                        info['graph_labels'].append(f'MCIQ-Raw{label_postfix}')
                    if db.get('plot_tof_bl') and len(refl_data[n]['tof']):
                        info['graph_data'].append(np.around(refl_data[n]['tof'].data_lst[:no_samples] * unit_mult, 2).tolist())
                        info['colors'].append("blue")
                        info['graph_labels'].append(f'ToF-Distance{label_postfix}')
                    if db.get('plot_tof_raw_bl') and len(refl_data[n]['tof']):
                        info['graph_data'].append(np.around(refl_data[n]['tof'].raw_lst[:no_samples] * unit_mult, 2).tolist())
                        info['colors'].append("black")
                        info['graph_labels'].append(f'ToF-Distance-Raw{label_postfix}')
                    if db.get('plot_likeliness_bl'):
                        info['graph_data'].append(np.around(refl_data[n]['likeliness'].raw_lst[:no_samples] * db.get('y_axis_range'), 2).tolist())
                        info['graph_labels'].append(f'Likeliness{label_postfix}')
                info['graph_data'] = list(zip(*info['graph_data']))
                queue.put(json.dumps(info))


def websocket_server():

    def serve_messages():
        log = logging.getLogger(__name__)
        while True:
            msg = queue.get()
            # log.info(msg)
            s.send_message_to_all(msg)

    from websocket_server import WebsocketServer
    s = WebsocketServer(port=81)
    server_thread = Thread(target=s.run_forever)
    server_thread.daemon = True
    server_thread.start()
    serve_msg_thread = Thread(target=serve_messages)
    serve_msg_thread.daemon = True
    serve_msg_thread.start()


def file_server(filename):

    def store_messages():
        log = logging.getLogger(__name__)
        while True:
            msg = queue.get()
            # log.info(msg)
            try:
                with open(filename, 'wb') as fh:
                    fh.write(msg.encode())
                    fh.flush()
            except:
                log.error("Failed writing %s" % filename)

    store_msg_thread = Thread(target=store_messages)
    store_msg_thread.daemon = True
    store_msg_thread.start()


queue = Queue()

if __name__ == '__main__':
    main(sys.argv[1:])




    # log = logging.getLogger(__name__)
    # log.info('Working on %s' % sys.platform)
    # parser = argparse.ArgumentParser(description='Run measurements on RangingPlatform')
    # parser.add_argument('--ws', action='store_true', help='use websocket based data transfer (file-based by default)')
    # parser.add_argument('--local', action='store_true', help='store settings/data in the working directory (webserver path otherwise)')
    # parser.add_argument('--clear', action='store_true', help='delete settings/data in the working directory')
    # parser.add_argument('-R', action='store_true', help='disable comport connection to reflector')
    # args = parser.parse_args(sys.argv[1:])
    #
    # info_filename = './info.json'
    # db_filename = './ranging.db'
    # if args.clear:
    #     files = [info_filename, db_filename]
    #     for f in files:
    #         if os.path.exists(f):
    #             os.remove(f)
    #
    # if not args.local:
    #     info_filename = '/dev/shm/info.json'
    #     db_filename = '/var/www/html/data/ranging.db'
    #
    # if args.ws:
    #     websocket_server()
    # else:
    #     file_server(info_filename)
    # main(db_filename,args)