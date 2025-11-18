# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import sys
import os
import logging.config
import time
import argparse
import platform
import numpy as np
import re
#import traceback

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))
logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))

from ranging_platform_pkg.utils import platform_connect, platform_measurement, inject_cfo
from ranging_platform_pkg.rng_platform_types import MeasurementModes
from stringutils import str_to_bool
from dirutils import ensure_dir

def list_str(values):
    return values.split(',')

# Convert input filepath to filename base prefix
# 'records/BRCM_$R_$TS/' => records/BRCM_init_20220519T175013/BRCM_init_20220519T175013
# 'records/BRCM_$R/' => records/BRCM_init/BRCM_init
# 'records/BRCM_$R' => records/BRCM_init
# 'records/UT/UT_1' => records/UT/UT_1
# '' => ./records/20220519T175013/20220519T175013
def build_filepath(filepath, csrole, blerole):
    timestamp = time.strftime("%Y%m%dT%H%M%S")
    fsave = filepath
    if filepath:
        filepath = os.path.splitext(filepath)[0]  # remove extension if present
        folder_path, file_prefix = os.path.split(filepath)
        #folder_path = os.path.split(filepath)[0]  # ='records'
        folder_name = os.path.basename(os.path.normpath(folder_path))
    else:
        folder_path = './records/' + timestamp
        file_prefix = timestamp
        folder_name = timestamp
    if file_prefix == '':
        # Reuse folder name as file basename
        filepath = folder_path + '/' + folder_name
    else:
        filepath = folder_path + '/' + file_prefix

    # Replace predefined tokens in filepath ($R is role, $TS is timestamp)
    role = csrole + ('' if blerole == 'none' else '_' + blerole)
    filepath = re.sub(r'\$R', role, filepath)
    filepath = re.sub(r'\$TS', timestamp, filepath)

    return filepath

def main(argv):
    def func_connect(arg):
        board = arg
        if not args.point2point:
            dut.set_measurement_mode(np.bitwise_or.reduce(np.array(args.measmode)), refl_boardnumber=board, init_boardnumber=args.init_brd)
            return None
        else:
            t1 = time.time()
            connected = sut['Initiator'].set_remote_address(sut['Initiator'].boardnumber2address(board), wait_on_connection=True)
            t2 = time.time()
            duration = round((t2 - t1) * 1000.0, 2)
            if not connected:
                log.error('Applying remote address failed @(remote = %d)' % board)
                return False
            log.info('Applying remote address @(remote = %d) => (duration = %.0fms)' % (board, duration))
            return True

    log = logging.getLogger(__name__)
    log.info('Working on %s' % sys.platform)

    parser = argparse.ArgumentParser(description='Run measurements on RangingPlatform')
    parser.add_argument('-n', action='store', type=int, default=16, help='number of measurements to execute')
    parser.add_argument('--useprocrepeat', type=int, default=0, help='Rely on Channel Sounding Procedure Repeat to reduce measurement latency (0=disabled, 1=enabled)')
    parser.add_argument('--debug', action='store', type=int, default=0,
                        help='Script debug verbosity, default=0')
    parser.add_argument('--target', action='append', type=str, default=[],
                        help='add one or more output targets (npz, mat, txt, csv, xlsx, json; default: npz, txt, csv)')
    parser.add_argument('--mainmode', action='store', type=list_str, default=[MeasurementModes.MCIQ],
                        help='Main_mode_type (1:RTT,2:RTP), Main_Mode_Min_Steps, Main_Mode_Max_Steps, Main_Mode_Repetition')
    parser.add_argument('--submode', action='store', type=int, default=None,
                        help='Sub mode type: 1-mode1(RTT), 2-mode2(RTP), 3-Mode3(RTP+RTT).')
    parser.add_argument('--comport', action='append', type=str, default=[],
                        help='add one or two comports to connect to (Initiator and Reflector respectively)')
    parser.add_argument('--dutconfig', action='append', type=str, default=[],
                        help='a string to pass to dut.write_configuration')
    parser.add_argument('--filepath', action='store', type=str, default='',
                        help='storage location of the result-file(s). $R and $TS may be used to insert role and timestamp in filepath.')
    parser.add_argument('--usemaxbaudrate', action='store', type=str_to_bool, default=True,
                        help='use the maximum baudrate (1; default) or the default baudrate (0).')
    parser.add_argument('--testmode', action='store_true',
                        help='Performs HADM test measurement. Requires all boards to be connected via USB (default: disabled).')
    parser.add_argument('--noras', action='store_true',
                        help='Do not use Ranging Service to collect peer data. Requires all boards to be connected via USB (default: RAS enabled).')
    parser.add_argument('--algo', action='store', type=list_str, default=[1,5],
                        help='Raw data to be exported on Host (0:No, 1:Yes), distance algo to be run embedded side(0:None, 1:CDE, 5:CDE+RADE')
    parser.add_argument('--diagnose', action='store_true',
                        help='Compute a diagnostic for each measurement (default: disabled).')
    parser.add_argument('--csrole', action='store', type=str, default="none",
                        help='init | refl: in testmode, disable one comport connection and specify role of connected board. In RAS mode, role of client.')
    parser.add_argument('--blerole', action='store', type=str, default="none",
                        help='central | peripheral: BLE role of the DUT. Only for connected mode.')
    parser.add_argument('-R', action='store_true', help='disable comport connection to reflector')
    parser.add_argument('--injectCFO', action='store', type=list_str, default=None,
                        help='inject CFO <null|medium|max>,<+|->')
    parser.add_argument('--plot', action='store', type=list_str, default=[0,10,0,200,0,25],
                        help='Plot ranging measurements ([plot_mode, y_lim (m), x-type, snapshot_nb_points,slide_mode,time_lim]. Plot mode: 0:No plotting, 1:Realtime + History block plotting, 2: History block plotting, default: 0, x-type: 0:time index, 1:time in seconds, slide_en: 0:static, 1:sliding display rade_trk, 2: sliding display rade_raw+rade_trk, time_lim: time span of sliding display).')

    parser.add_argument('--board', action='store', type=str, default='kw45', help='board type (E.g. kw36, kw38, kw45, kw47)')

    # DEPRECATED
    #parser.add_argument('--reverse_order', action='store_true', help='reverse the order in which parameters are executed')
    #parser.add_argument('--board', action='store', type=str, default='kw45', help='board type (E.g. kw36, kw38, kw45)')
    #parser.add_argument('--init_brd', action='store', type=int, default=None,
    #                    help='specify initiator board number; not be used in combination with --point2point (e.g. --init_brd=12)')
    #parser.add_argument('--remote', action='append', type=int, default=[],
    #                    help='add one or more remote board numbers to iterate over')
    #parser.add_argument('--antrefl', action='store', type=int, default=0,
    #                    help='Remote antenna type  0 - single on-board(ANT20), 1 - dual on-board (ANT20/30), 2 - single (EXT2), 3 - dual (EXT1/2)')
    #parser.add_argument('--antinit', action='store', type=int, default=0,
    #                    help='Initiator antenna type  0 - single on-board (ANT20), 1 - dual on-board (ANT20/30), 2 - single (EXT2), 3 - dual (EXT1/2)')
    #parser.add_argument('--point2point', action='store_true',
    #                    help='Enable point2point connection (support for multiple connections otherwise)')
    #parser.add_argument('--unique_id', action='append', type=str, default=[],
    #                    help='add one or two unique_ids to connect to (Initiator and Reflector respectively)')
    #parser.add_argument('--calibrate', action='store', type=float, default=None,
    #                    help='Enable calibration procedure at X meters (default: disabled).')

    args = parser.parse_args(argv)
    # ------------------------------------------------------------------------------------------------------------------
    # Setup
    # ------------------------------------------------------------------------------------------------------------------
    board_type = args.board  # default is 'kw45' if not provided
    calibrate = None
    pause = False # Pause script before launching range/test command
    args.init_brd = None
    args.remote = []
    args.csrole = args.csrole.lower()
    args.blerole = args.blerole.lower()
    algo_list = [int(i) for i in args.algo]
    if algo_list[0] == 0: # No algo ran on Host
        # optimal mode to obtain embedded distance as fast as possible
        execAlgo = False
        verbosity = 0 # Reduce CLI output to minimum
        if algo_list[1] == 0:
            log.error("At least one algorithm environment (Host or Embedded) needs to be configured")
            return 1
    else:
        verbosity = 124  # 4=MeasurementInfo, 8=MeasurementData, 16=MeasurementDebug, 32=BoardInfo, 64=Profiling
        execAlgo = True
    rangeCmdTimeout = 4.0
    # testmode implies no RAS
    if args.testmode:
        args.noras = True
        if args.blerole != 'none':
            log.error("--blerole and --testmode options are incompatible.")
            return 2
    if len(args.target) == 0:
        args.target = ['npz', 'txt', 'csv']

    filepath = build_filepath(args.filepath, args.csrole, args.blerole)
    ensure_dir(filepath)

    # Log to file
    fh = logging.FileHandler(filepath + "_main.log", mode='w')
    fh.setLevel(logging.INFO)
    log.addHandler(fh)

    # Log script parameters
    log.info("Script parameters: " + str(sys.argv[1:]))

    dut_descr = {'comport': 'auto', 'unique_id': 'auto'}
    peer_descr = {'comport': 'auto', 'unique_id': 'auto'}
    if len(args.comport) == 1:
        dut_descr['comport'] = args.comport[0]
    if len(args.comport) == 2:
        # dut_descr must be updated too
        dut_descr['comport'] = args.comport[0]
        peer_descr['comport'] = args.comport[1]
    if args.R:
        peer_descr['comport'] = 'none'
    #if len(args.unique_id) == 1:
    #    init['unique_id'] = args.unique_id[0]
    #if len(args.unique_id) == 2:
    #    refl['unique_id'] = args.unique_id[1]
    #if args.calibrate is not None:
    #    print("Calibration mode enabled")
    #    if args.R:
    #        log.error("Please don't use -R option when running calibration")
    #        return 1
    #    # overrides arguments for calibration procedure (no antenna diversity, no multi-connection, nb samples >= 256)
    #    if args.n < 256:
    #        args.n = 256
    #    args.antrefl = 0
    #    args.antinit = 0
    #    if len(args.remote) > 1:
    #        log.error("Please don't specify more than one peripheral when running calibration")
    #        return 1
    if args.injectCFO is not None:
        print("CFO injection mode enabled")
        if not args.noras:
            log.error("Both devices need to be connected to be able to force some CFO")
            return 3

    if args.testmode:
        args.dutconfig.insert(0, "system debug 1")  # Activate DTEST debug by default in test mode
        if args.csrole == 'refl':
            peer_descr['comport'] = 'none'
            execAlgo = False
            # Large timeout when DUT is reflector in testmode, allowing operator to start initiator manually
            rangeCmdTimeout = 60.0
        if args.csrole == 'init':
            peer_descr['comport'] = 'none'
            execAlgo = False
            pause = True

    try:
        sut = platform_connect(board_type, dut_descr, peer_descr, debug=args.debug, verbosity=verbosity,
                               usemaxbaudrate=args.usemaxbaudrate, cal=calibrate)
        dut = sut['dut']
        peer = sut['peer']

        #dut.set_antenna_mode(init=args.antinit, refl=args.antrefl)
        mmargs = dict()
        dut.set_diagnose(args.diagnose)
        #remotes = list(map(dut.boardnumber2address, args.remote))
        #if not args.point2point:
        #    dut.set_remote_address(remotes, wait_on_connection=False)
        #    mmargs['init_boardnumber'] = args.init_brd

        # Apply CS role to dut
        if args.csrole == 'refl':
            dut.set_csrole('Reflector')
        else:
            # if csrole is not specified, set to Initiator
            dut.set_csrole('Initiator')

        # In test mode/no RAS, we need to control the second device (auxdut) in sync with the dut
        if args.noras:
            if args.testmode and args.csrole != "none":
                auxdut = None
            else:
                auxdut = sut['peer']
                # Apply CS role to auxdut (opposite to dut)
                if args.csrole == 'refl':
                    auxdut.set_csrole('Initiator')
                else:
                    auxdut.set_csrole('Reflector')
        else:
            auxdut = None

        # Add debugging options (first in the list so that it can be overwritten on CLI)
        #args.dutconfig.insert(0, "parameter debug 1")
        if algo_list[0] != 0:
            args.dutconfig.insert(0, "parameter debug 4")  # Activate Event debug via HCI message

        if args.dutconfig:
            dut.write_configuration(args.dutconfig)

        # Apply connection roles, will start connection activity
        if not args.testmode:
            #if algo_list[1] != 0: # Configure embedded algo
            dut.write_configuration(['misc rtp_algo %d' % algo_list[1]])
            # Default BLE Role for DUT is central
            if args.blerole == 'none':
                args.blerole = 'central'
            try:
                peer_unique_id = peer.UniqueId
            except Exception:
                peer_unique_id = None
            dut.set_role(args.blerole, peer_unique_id)
            try:
                peer.set_role('peripheral' if args.blerole == 'central' else 'central', dut.UniqueId)
            except Exception:
                pass  # No reflector to configure (-R is used)
            connected = dut.wait_on_connection()
            if not connected:
                raise Exception('Wireless connection not ready!')

        dut.set_measurement_hadm_mode(args.mainmode, args.submode)
        mmargs['rasrole'] = 'client'
        mmargs['testmode'] = args.testmode
        dut.set_measurement_repetition((args.useprocrepeat == 1), args.n)
        dut.set_measurement_execution_mode(**mmargs)

        if auxdut is not None:
            auxdut.set_measurement_hadm_mode(args.mainmode, args.submode)
            mmargs['rasrole'] = 'server'
            mmargs['testmode'] = args.testmode
            auxdut.set_measurement_execution_mode(**mmargs)
            if args.dutconfig:
                auxdut.write_configuration(args.dutconfig)

            if args.injectCFO is not None:
                inject_cfo(dut, auxdut, args.injectCFO)

    except Exception as e:
        log.error(e)
        raise e

    if False:
        log.info('========== System info ====================================\n'
                 + 'Hostname           : ' + platform.node() + '\n'
                 + repr(dut).rstrip('\n'))
        log.info('===========================================================')

    parameters = []
    # Per measure parameters, logged in .txt/.csv file
    p = {
        'name': 'meas_nr',
        'range': list(range(args.n)),
        'fmt': '%02d'
    }
    parameters.append(p)

    #if args.reverse_order:
    #    parameters.reverse()
    # ------------------------------------------------------------------------------------------------------------------
    # Run measurements
    # ------------------------------------------------------------------------------------------------------------------
    AllResults = platform_measurement(parameters, dut=dut, filepath=filepath, targets=args.target, auxdut=auxdut,
                                      execAlgo=execAlgo, pause=pause, rangeCmdTimeout=rangeCmdTimeout, plotConfig=args.plot,  algos=algo_list, debug=args.debug)
    # ------------------------------------------------------------------------------------------------------------------
    # Teardown
    # ------------------------------------------------------------------------------------------------------------------
    dut.disconnect()
    log.removeHandler(fh)
    fh.close()

    return AllResults


if __name__ == '__main__':
    # +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # THE MAIN FUNCTIONALITY OF THIS TEST-FUNCTION
    # +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    main(sys.argv[1:])

