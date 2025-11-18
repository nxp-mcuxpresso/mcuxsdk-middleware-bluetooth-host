#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# All rights reserved
# (C) Copyright 2020, Imec.nl
#

import argparse
import logging
import sys
import os
import subprocess

VERSION = '0.0.1'
FORMAT = '%(asctime)-15s %(levelname)s %(filename)s:%(lineno)d %(message)s'
m_description = "This script is used to test the MUSIC-algorithm stability and execution time."
m_epilog = "run_test_controlled_multipath v{:s}, Copyright (c) 2020, IMEC <info@imec-nl.nl>".format(VERSION)
assert_on_fail = False

def handle_arguments():
    parser = argparse.ArgumentParser(description=m_description, epilog=m_epilog)
    parser.add_argument('-s', '--runsp', action='store_true', default=False, help='Run single-path test (not to be combined with two-path test)')
    parser.add_argument('-t', '--runtp', action='store_true', default=False, help='Run two-path test (not to be combined with single-path test)')
    parser.add_argument('-c', '--clean-pipenv', action='store_true', default=False, help='Run the step that cleans up the virtual-env linked to the pipfile')
    parser.add_argument('-a', '--assert-on-fail', action='store_true', default=False, help='Stop execution at any process that fails')
    parser.add_argument('-b', '--boards', nargs='?', type=str, default='[nrf52833,kw36]', help='Board names, seperated by a comma, example: [kw36,at86rf215]')
    parser.add_argument('-r', '--repeats', nargs='?', type=int, default=30, help='Specifies the number of measurements per parameter combination')
    parser.add_argument('-ads', '--attdirect-sp', nargs='?', type=str, default='[ATTENUATION_MAX]', help='Attenuation range in direct path (single-path test)')
    parser.add_argument('-acs', '--attcommon-sp', nargs='?', type=str, default='range(5,83,2)', help='Attenuation range in common path (single-path test)')
    parser.add_argument('-cs', '--cforange-sp', nargs='?', type=str, default='range(-50000,50001,50000)', help='CFO-range (single-path test)')
    parser.add_argument('-adt', '--attdirect-tp', nargs='?', type=str, default='range(0,26,2)', help='Attenuation range in direct path (two-path test)')
    parser.add_argument('-act', '--attcommon-tp', nargs='?', type=str, default='range(0,83,2)', help='Attenuation range in common path (two-path test)')
    parser.add_argument('-ct', '--cforange-tp', nargs='?', type=str, default='range(-50000,50001,50000)', help='CFO-range (two-path test)')
    return parser.parse_args()


def log_header(message):
    assert len(message) <= 70, "log_header() allows max 70 characters"
    logging.info("")
    logging.info("############################################################################")
    logging.info("###{:^70}###".format(message))
    logging.info("############################################################################")


def clean_pipenv(working_dir):
    pipenv_steps = ["clean", "sync", "check"]
    for step in pipenv_steps:
        process_args = ["pipenv", step]
        if assert_on_fail:
            subprocess.run(process_args, shell=False, check=assert_on_fail, cwd=working_dir)
        else:
            process_rv = subprocess.run(process_args, shell=False, check=assert_on_fail, cwd=working_dir,
                                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding=sys.stdout.encoding)
            logging.info(process_rv.stdout)


def run_in_pipenv(cmd, params=[], working_dir=None):
    pipenv_run = ["pipenv", "--bare", "run"]
    process_args = []
    process_args.extend(pipenv_run)
    process_args.extend([cmd])
    process_args.extend(params)
    if assert_on_fail:
        subprocess.run(process_args, shell=False, check=assert_on_fail, cwd=working_dir)
    else:
        process_rv = subprocess.run(process_args, shell=False, check=assert_on_fail, cwd=working_dir,
                                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding=sys.stdout.encoding)
        logging.info(process_rv.stdout)


if __name__ == "__main__":
    # set the default log level
    logging.basicConfig(level=logging.INFO, stream=sys.stdout, format=FORMAT)

    this_file_dir = os.path.dirname(os.path.realpath(__file__))
    pipfile_dir = os.path.dirname(this_file_dir)  # assume pipfile is one level up from this script

    # handle all command line options
    args = handle_arguments()
    assert_on_fail = args.assert_on_fail
    assert args.repeats > 0
    repeat_str = str(args.repeats)
    assert args.runsp != args.runtp, "Either --runsp or --runtp should be True"
    if args.runsp:
        path_count = 'single'
        attdirect = args.attdirect_sp
        attcommon = args.attcommon_sp
        cforange = args.cforange_sp
    else:  # must be runtp now (because of earlier assert)
        path_count = 'two'
        attdirect = args.attdirect_tp
        attcommon = args.attcommon_tp
        cforange = args.cforange_tp

    log_header("PYTHON ENVIRONMENT")
    if args.clean_pipenv:
        clean_pipenv(working_dir=pipfile_dir)
    run_in_pipenv(cmd="python", params=["--version"], working_dir=pipfile_dir)
    run_in_pipenv(cmd="pip", params=["--version"], working_dir=pipfile_dir)

    log_header("SUMMARY OF USED PARAMETERS")
    logging.info("Boards                                             : {}".format(args.boards))
    logging.info("Number of repeats                                  : {}".format(args.repeats))
    logging.info("Attenuation range in direct path ({}-path test): {}".format(path_count, attdirect))
    logging.info("Attenuation range in common path ({}-path test): {}".format(path_count, attcommon))
    logging.info("CFO-range ({}-path test)                       : {}".format(path_count, cforange))

    log_header("SET ENVIRONMENT")
    logging.error('TBD envSettings.sh')

    log_header("ENABLE ALL BOARD")
    run_in_pipenv(cmd="python", working_dir=pipfile_dir,
                  params=["./Scripts/power_cycle_boards.py",
                          "--board", "all",
                          "--action", "cycle",
                          "--config", "./Verification/Regression/test_controlled_multipath.yaml"])

    log_header("FLASH FIRMWARE")
    run_in_pipenv(cmd="python", working_dir=pipfile_dir,
                  params=["./Instruments/flashtool_pkg/flashtool.py", "--pattern=../Jenkins/flashtool.kw36.config"])

    log_header("COMPILE C-CODE")
    run_in_pipenv(cmd="python", working_dir=pipfile_dir, params=["../python/Scripts/compile_algorithm_c_code.py"])

    log_header("TEST ALGORITHM STABILITY AND METRICS")
    pytest_params = []
    pytest_params.extend(["-m", "pytest"])
    pytest_params.extend(["-s", "-v", "--maxfail=100",
                        "Verification/Regression/test_controlled_multipath.py",
                        "-k", "test_{}_path".format(path_count),
                        "--junit-xml=controlled_{}_path.xml".format(path_count)])
    if args.runsp:
        pytest_params.extend(["--measmode=[All]"])
    pytest_params.extend(['--board=' + args.boards,
                          '--repeats=' + repeat_str,
                          '--attdirect=' + attdirect,
                          '--attcommon=' + attcommon,
                          '--cforange=' + cforange])
    run_in_pipenv(cmd="python", working_dir=pipfile_dir, params=pytest_params)
