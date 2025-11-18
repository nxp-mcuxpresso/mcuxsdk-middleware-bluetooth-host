# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------


import sys, os
import glob
import logging.config
import argparse
import re
import numpy as np
import yaml

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))
logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))

from nesteddict import NestedDict
from ranging_platform_pkg.resultfile import ResultFile
from ranging_platform_pkg.RangingPlatform import RangingPlatform
from ranging_platform_pkg.rng_platform_types import MeasurementModes
from stringutils import str_to_bool


def load_txt(filename, **kwargs):
    overlay = NestedDict(kwargs.get('overlay', {}))
    dut = RangingPlatform(board=kwargs.get('board'), format=kwargs.get('format'))
    dut.set_measurement_mode(kwargs.get('mode', MeasurementModes.MCIQ))
    result_lst = []
    parameters = {}
    with open(filename, 'rb') as fh:
        block = ''
        lines_lst = fh.readlines()
        for cnt, line in enumerate(lines_lst):
            line = line.decode()
            line = line.strip("\r\t ")
            m = re.match(r'^=== (.*) ===', line)
            if m is None:
                block += line
            if m is not None or (cnt + 1) == len(lines_lst):
                if block.strip() != '':
                    results = dut.run_range_measurement(response=block, overlay=overlay)
                    result_lst.extend(results)
                    block = ''
            if m is not None:
                # parameters of next block
                pms = m.group(1).split(',')
                parameters = {}
                for p in pms:
                    tmp = p.replace(' ', '').replace(':', '=').split('=')
                    if len(tmp) == 2:
                        try:
                            parameters[tmp[0].strip()] = int(tmp[1], 0)
                        except:
                            parameters[tmp[0].strip()] = tmp[1].strip()
                for k, v in parameters.items():
                    overlay[f'meta.testcase.{k}'] = v
    return result_lst

def main(argv):
    log = logging.getLogger(__name__)
    log.info('Working on %s' % sys.platform)

    parser = argparse.ArgumentParser(description='convert measurement results of RangingPlatform')
    parser.add_argument('--pattern', action='store', type=str, default='./input/*.txt', help='files to convert (txt, npz), use * as wildcard; default: ./input/*.txt')
    parser.add_argument('--output', action='store', type=str, help='output folder; default follows input directory')
    parser.add_argument('--target', action='append', type=str, default=[], help='add target output types (txt, npz, mat, csv, xlsx')
    parser.add_argument('--board', action='store', type=str, default='kw38', help='board type (E.g. at86rf215, kw36); applicable when using txt as input')
    parser.add_argument('--format', action='store', type=str, default='4', help='output format type (3, 4); applicable when using txt as input')
    parser.add_argument('--measmode', action='append', type=int, default=[], help='add one or more measurement modes 1 - MCIQ, 2 - ToF; applicable when using txt as input')
    parser.add_argument('--normalized', action='store', type=str_to_bool, default=True, help='remove all unknown fields if True (default)')
    parser.add_argument('--sizeable', action='store', type=str_to_bool, default=True, help='include all fields marked sizeable if True (default)')
    parser.add_argument('--merge', action='store_true', help='merge all input files')
    parser.add_argument('--overlay_file', action='store', type=str, default=None, help='specify overlay YAML definition file; applicable when using txt as input')

    args = parser.parse_args(argv)
    if len(args.measmode) == 0:
        args.measmode = MeasurementModes.MCIQ
    files = glob.glob(args.pattern)
    no_files = len(files)
    if no_files == 0:
        logging.log(logging.FATAL, 'No files match pattern (%s)' % (args.pattern))
        return
    log.info("Found %d files using input pattern %s.." % (no_files, args.pattern))

    active_modes = []
    for m in MeasurementModes.all_types():
        mode = MeasurementModes.to_str(m)
        if args.measmode & m:
            active_modes.append(mode)
    rf = ResultFile()
    rf.initialize(active_modes)

    all_data = []
    for file in files:
        rf.clear()
        f_dir, f_bnm = os.path.split(file)
        f_base, f_ext = os.path.splitext(f_bnm)
        if args.output is None:
            f_out = os.path.join(f_dir, f_base)
        else:
            f_out = os.path.join(args.output, f_base)
        if f_ext == '.txt':
            overlay = {}
            if args.overlay_file:
                try:
                    fh = open(args.overlay_file, 'r')
                    overlay = yaml.safe_load(fh)
                except:
                    log.error(f'Failed loading: {args.overlay_file} (overlay YAML)')
                    return
            data = load_txt(file, overlay=overlay, board=args.board, format=args.format, mode=np.bitwise_or.reduce(np.array(args.measmode)))
            rf.extend(data)
        elif f_ext == '.npz':
            rf.load(file)
            data = list(rf)
        else:
            log.error('Failed loading: %s, unsupported extension: %s' % (file, f_ext))
            continue
        all_data.extend(data)
        if args.merge:
            log.info('Loaded %s' % (file))
            continue
        for ext in args.target:
            filename = f'{f_out}.{ext}'
            try:
                rf.save(filename, normalized=args.normalized, sizeable=args.sizeable)
            except:
                log.error('Failed saving %s (target = %s)' % (filename, ext))
            else:
                log.info('Saved %s (target = %s)' % (filename, ext))

    if args.merge:
        rf.clear()
        rf.extend(all_data)
        f_out = f_dir if args.output is None else args.output
        for ext in args.target:
            filename = os.path.join(f_out, f'merged.{ext}')
            try:
                rf.save(filename, normalized=args.normalized, sizeable=args.sizeable)
            except:
                log.error('Failed saving %s (target = %s)' % (filename, ext))
            else:
                log.info('Saved %s (target = %s)' % (filename, ext))
    log.info("Finished")

if __name__ == '__main__':
    main(sys.argv[1:])