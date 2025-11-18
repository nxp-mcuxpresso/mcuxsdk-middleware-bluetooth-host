#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

# Wrapper to record_range_measurement.py & convert_to_excel.py.
# This script allows to run HADM measurements with different configurations as listed in a separate text file.
# Results may be combined in a single excel file with one set of sheets per measurement.
# Usage: record_range_ut.py  // will make use of default testcase input file i.e. 'range_ut.txt'

import sys
import os
import shutil
import time
import logging.config
import argparse
import shlex
import traceback
import yaml
import record_range_measurement
import convert_to_excel
import UTCriteria
import re

BaseDir = os.path.dirname(os.path.realpath(__file__))
logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))


class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


class Verdict:
    def __init__(self, testname, status, failures, values=()):
        self._testname = testname
        self._status = status
        self._failures = failures
        self._values = values

    def __str__(self):
        return str((self._testname, self._status, self._failures))

    def get_status_string(status):
        return'PASS' if status else 'FAIL'

    def get_colored_status(status_string):
        return Colors.OKGREEN + 'PASS' + Colors.ENDC if status_string == 'PASS' else Colors.FAIL + 'FAIL' + Colors.ENDC

    def get_failed_criteria_names(self, verbosity):
        # Combine similar criteria using dictionary key overlap
        failed_criteria_dict = {}
        for failed_run in self._failures:
            for failed_criteria in failed_run[1]:
                failed_criteria_dict[failed_criteria[0]] = 1
        # Generate comma-separated list of failed criteria for this verdict
        failed_criteria_names = ''
        for key in failed_criteria_dict:
            failed_criteria_names += ', ' if failed_criteria_names != '' else '' + key
        return failed_criteria_names

    def to_string(self, verbosity):
        # Show only failing criteria
        if verbosity == 1:
            return " %s '%s'" % (self._status, self._testname)
        elif verbosity == 2:
            return " %s '%s' | %s" % (self._status, self._testname, self.get_failed_criteria_names(verbosity))
        elif verbosity == 3: # Values only
            return " %s '%s' | %s %s" % (self._testname, self._status, self.get_failed_criteria_names(verbosity), self._values)
        elif verbosity == 4: # CSV format
            return "%s,%s,%s" % (self._testname, self._status, self._values[0])
        else:
            return str(self)


def aggregate_verdicts(verdicts):
    all_keys = []
    all_values = []
    for verdict in verdicts:
        if verdict._values == ():
            continue
        val = verdict._values[0][1]
        # keys, values = list(zip(*val))
        val_dict = dict(val)
        for key, value in val_dict.items():
            if not isinstance(value, (list, tuple)) or len(value) <= 4:
                all_keys = list(set(all_keys) | {key})
    all_keys.sort()
    for verdict in verdicts:
        if verdict._values == ():
            continue
        val = verdict._values[0][1]
        val_dict = dict(val)
        row = [verdict._testname]
        for key in all_keys:
            if key in val_dict:
                row.append('"'+str(val_dict[key])+'"')
            else:
                row.append('')
        all_values.append(row)
    all_keys.insert(0,"TestName")
    return all_keys, all_values

def main(argv):
    log = logging.getLogger(__name__)
    log.setLevel(logging.DEBUG)
    log.info('Working on %s' % sys.platform)

    parser = argparse.ArgumentParser(description='Automate ranging measurements with several configurations and combine results.')
    parser.add_argument('--output', action='store', type=str, help='folder name within "records" directory (timestamp used otherwise)')
    parser.add_argument('--testfile', action='store', type=str, default='range_ut_hal.yml', help='testcase input files (comma separated)')
    parser.add_argument('--statfile', action='store_true', help='generate file with combined statistics across all testcases')
    parser.add_argument('--target', action='store', type=str, default='', help='specify an output target (xlsx, cxlsx; default: None)')
    parser.add_argument('--verbosity', action='store', type=int, default=0, help='verbosity of test results (1=do not show failure condition, 2=show failed criteria only); default: show all')
    parser.add_argument('--runall', action='store_true', help='run all tests, including the one marked as "skip"')
    parser.add_argument('--n_reps', action='store', type=int, help='override the --n parameter in all test cases with a new value')
    parser.add_argument('--ant_type', action='store', type=int, default=0, help='antenna type to be used')
    parser.add_argument('--comport', action='append', type=str, default=[],
                        help='add one or two comports to connect to (Initiator and Reflector respectively)')
    parser.add_argument('--board', action='store', type=str, default='kw45', help='board type (E.g. kw36, kw38, kw45, kw47)')
    args = parser.parse_args()

    # Load and check input test files
    testfiles = args.testfile.split(',')
    testcases = []
    for testfile in testfiles:
        try:
            with open(testfile, "r") as stream:
                try:
                    tests_sub = yaml.load(stream, Loader=yaml.FullLoader)
                    assert isinstance(tests_sub, dict), 'YAML file error: must be a dict'
                    testcases = testcases + tests_sub['testcases']
                except yaml.YAMLError as e:
                    print(e)
                    exit(1)
        except IOError as e:
            print(e)
            exit(1)

    # override the --n value in all test cases if --n_reps arg is present and > 0
    if args.n_reps:
        if args.n_reps > 0:
            testcase_dicts = testcases
            for t_dict in testcase_dicts:
                t_dict['params'] = re.sub('-n [0-9]+', '-n ' + str(args.n_reps), t_dict['params'])
    
    # Output folder will be records/<output> or records/<timestamp> or absolute path if specified
    if args.output is not None:
        if os.path.isabs(args.output):
            # Absolute path is provided
            filepath = args.output
        else:
            # Relative path is provided: relates to records folder
            filepath = 'records/' + args.output
    else:
        timestamp = time.strftime("%Y%m%dT%H%M%S")
        filepath = 'records/' + timestamp

    # Remove output folder if it exists
    try:
        shutil.rmtree(filepath)
    except OSError as e:
        if isinstance(e, FileNotFoundError):
            pass
        else:
            print("Output directory cleanup: %s - %s." % (e.filename, e.strerror))
            exit(1)

    # Create output folder
    os.makedirs(os.path.normpath(filepath), exist_ok=True)

    # Build output file name
    f_dir, folder = os.path.split(filepath)
    filenamebase = filepath + "/" + folder

    # Log to file
    fh = logging.FileHandler(filenamebase + ".log", mode='w')
    fh.setLevel(logging.DEBUG)
    log.addHandler(fh)

    # Iterate on testcase description file
    hadmVerdict = UTCriteria.HadmResultVerdict()
    verdicts = []
    global_verdict = True
    global_pass = 0
    global_fail = 0
    global_skip = 0

    for test in testcases:
        # test execution must be skipped
        skip = 'skip' in test and test['skip'].lower() == 'y' and not args.runall
        # test verdict must be ignored (but test still executed)
        ignore = 'skip' in test and test['skip'].lower() == 'i' and not args.runall
        testname = str(test['id']) + ' ' + test['name']
        if skip:
            verdicts.append(Verdict(testname, 'SKIP', ''))
            global_skip += 1
            continue

        fullfilename = filenamebase + "_" + str(test['id'])
        cmd_args = test['params'] + ' --filepath ' + fullfilename + ' --diagnose'
        cmd_args = cmd_args + " --dutconfig 'param ant_type " + str(args.ant_type) + "'"
        if len(args.comport) == 1:
            cmd_args = cmd_args + " --comport " + str(args.comport[0])
        if len(args.comport) == 2:
            cmd_args = cmd_args + " --comport " + str(args.comport[0]) + ' ' + " --comport " + str(args.comport[1])
        if str(args.board) != "":
            cmd_args = cmd_args + " --board " + str(args.board)
        args_list = shlex.split(cmd_args)
        log.info("***************************************************************************************************")
        log.info("Run test_" + str(test['id']) + " " + test['name'] + ' ' + str(args_list))
        all_results =[]
        try:
            all_results = record_range_measurement.main(args_list)
        except Exception as e:
            # Catch any exception and log it in order to continue execution
            print(traceback.format_exc())
            all_results = str(e)

        # Evaluate verdict
        test_verdict = True
        if isinstance(all_results, (int, str)):
            # A script execution error has occurred
            verdicts.append(Verdict(testname, 'ERROR', 'Recording failure: ' + str(all_results)))
            test_verdict = False
            global_fail += 1
        elif ignore:
            verdicts.append(Verdict(testname, 'SKIP', ''))
            global_skip += 1
        else:
            meas_iter = 0
            failures = []
            values = []
            for result in all_results:
                # Ignore measures showing UART CRC error
                if result['meta.error_msg'] != 'crc':
                    verdict, fails, value = hadmVerdict.get_failures(result, test.get('cat'))
                else:
                    verdict, fails, value = True, [], []
                test_verdict = test_verdict and verdict
                if not verdict:
                    failures.append((meas_iter, fails))
                values.append((meas_iter, value))
                meas_iter += 1
            verdicts.append(Verdict(testname, Verdict.get_status_string(test_verdict), failures, values))
            if test_verdict:
                global_pass += 1
            else:
                global_fail += 1
        # Global verdict accumulates individual test verdicts
        global_verdict = global_verdict and test_verdict

    # Print verdict
    log.info("********************************************************")
    log.info("Verdict: (testcase, iteration, [failing criteria])")
    [log.info('[TEST]' + verdict.to_string(args.verbosity)) for verdict in verdicts]
    log.info("Global Verdict: %s (pass %d / fail %d / skip %d / total %d)" %
             (Verdict.get_colored_status(Verdict.get_status_string(global_verdict)),
              global_pass, global_fail, global_skip, global_pass + global_fail + global_skip))
    log.info("********************************************************")

    # Generate stats file by merging all results
    if args.statfile is not None:
        filename = filenamebase +'_stats.csv'
        f = open(filename, "wb")
        if f is None:
            raise Exception("save_stats: failed saving file to %s" % filename)
        else:
            all_keys, all_values = aggregate_verdicts(verdicts)
            f.write(','.join(all_keys).encode('utf-8') + '\n'.encode('utf-8'))
            for v in all_values:
                f.write(','.join(v).encode('utf-8') + '\n'.encode('utf-8'))
        f.close()

    # Convert results
    if args.target in ('xlsx', 'cxlsx'):
        cmd_args = "--pattern " + filepath + "/*.npz" + (" --combined" if args.target == 'cxlsx' else "")
        args_list = shlex.split(cmd_args)
        convert_to_excel.main(args_list)

    # Generate zip file
    # Create zip file outside zipped folder and relocate to it once done
    # zip filename is made of folder name and script name
    zipfile = f_dir + '/' + folder
    log.info("Zipping results in %s..." % zipfile)
    shutil.make_archive(zipfile, 'zip', filepath)
    shutil.move(zipfile + '.zip', f_dir + '/' + folder)
    log.info("Finished")
    fh.close()

if __name__ == '__main__':
    main(sys.argv)
