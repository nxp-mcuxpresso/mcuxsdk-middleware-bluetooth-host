#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
# Convert npz to excel file
# Usage1: convert_to_excel.py --pattern <folder>\*.npz --target {json}
# Usage2: convert_to_excel.py --target {json}  // Will detect the most recent folder and use the .npz from there

import sys
import os
import re
from pathlib import Path
import glob
import logging.config
import argparse
import traceback
from importlib import import_module
from openpyxl import Workbook
from openpyxl.chart import Reference, Series, ScatterChart, LineChart
from openpyxl.styles import Font

import math
import numpy
import json
from json import JSONEncoder


BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))
logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))

from nesteddict import NestedDict
from ranging_platform_pkg.resultfile import ResultFile,IOPConverter,natural_keys,save_hadm
from ranging_platform_pkg.rng_platform_types import MeasurementModes
from convert_range_measurement_data import load_txt


# JSON Encoder for result object
class NestedDictEncoder(JSONEncoder):
    def default(self, o):
        if isinstance(o, numpy.ndarray):
            return numpy.ndarray.tolist(o)
        elif isinstance(o, complex):
            return str(o)
        else:
            return o.__dict__


class IOPConverter(object):
    def __new__(cls, **kwargs):
        identifier = kwargs.get('name', 'unspecified')
        modname = f'IOP_convert_{identifier}'
        try:
            #imported_module = import_module('.' + modname, package=__package__)
            imported_module = import_module(modname)
            return getattr(imported_module, modname)(**kwargs)
        except ModuleNotFoundError:
            raise Exception("Unknown IOP_convert_ identifier: %s" % (identifier,))


def main(argv):

    parser = argparse.ArgumentParser(description='convert measurement results of RangingPlatform')
    parser.add_argument('--pattern', action='store', type=str, default='*.npz', help='files to convert, use * as wildcard; default: ./input/*.txt')
    parser.add_argument('--output', action='store', type=str, help='output folder; default follows input directory')
    parser.add_argument('--combined', action='store_true', help='enable combined file output (separate otherwise)')
    parser.add_argument('--target', action='append', type=str, default=[], help='add target output types (json, ...)')
    parser.add_argument('--silent', action='store_true', help='do not output information to the console')
    parser.add_argument('--iopfile', action='store', type=str, default='', help='3rd party file to convert')
    parser.add_argument('--ioptype', action='store', type=str, default=None, help='name of 3rd party converter')

    args = parser.parse_args(argv)

    log = logging.getLogger(__name__)
    if args.silent:
        log.setLevel(logging.ERROR)
    else:
        log.setLevel(logging.DEBUG)
    log.info('Working on %s' % sys.platform)

    config = {
        'input_pattern': args.pattern,
        'output_directory': args.output,
        'targets': args.target,
        'combined': args.combined
    }

    path = config['input_pattern']
    if (os.path.dirname(path) == ''):
        paths = [x for x in Path("./records").iterdir() if x.is_dir()]
        most_recent_path = sorted(paths, key=os.path.getmtime, reverse=True)[0]
        path = os.path.join(most_recent_path, path)
    files = glob.glob(path)
    no_files = len(files)
    if no_files == 0:
        log.error('No files match pattern (%s)' % (config['input_pattern']))
        return
    log.info("Found %d files using input pattern %s.." % (no_files, config['input_pattern']))

    wb = None
    measure_id_start = 1
    f_out = None
    f_base = None
    if config['combined']:
        # Create excel workbook
        wb = Workbook()
        wb.remove(wb.active)
        folder = os.path.basename(os.path.dirname(path))
        f_out = os.path.join(os.path.dirname(path), folder+'_combined_hadm')

    files.sort(key=natural_keys)
    for file in files:
        log.info("Process: " + file)

        # Generate output filename
        f_dir, f_bnm = os.path.split(file)
        f_base, f_ext = os.path.splitext(f_bnm)
        # Last element in the filename will be used as base name for the corresponding sheet in excel file
        # i.e. for TEST_1.npz, 1_0, 1_2, 1_3 ... will be generated
        f_token = f_base.split('_')[-1]
        if not config['combined']:
            if config['output_directory'] is None:
                f_out = os.path.join(f_dir, f_base)
            else:
                f_out = os.path.join(config['output_directory'], f_base)
            f_base = f_out
            f_out += "_hadm"

        # Load input file
        if f_ext == '.txt':
            data = load_txt(file, board='kw38', format='4')
        elif ['.npz', 'npy'].count(f_ext) != 0:
            try:
                rf = ResultFile()
                rf.initialize([MeasurementModes.MCIQ])
                rf.load(file)
                data = list(rf)
            except Exception as e:
                log.error('Failed loading: %s, %s' % (file, e))
                continue

            try:
                if args.ioptype is not None:
                    if args.iopfile == '':
                        iopfile = Path(f_base + '.' + args.ioptype)
                    else:
                        iopfile = Path(args.iopfile)

                    if (args.ioptype != None) and (iopfile != ''):
                        log.info("Process IOP file: " + str(iopfile))
                        with open(Path(iopfile), 'r') as f:
                            # Find oposite role to the NXP board
                            if data[0].get('mciq.reflector.i', None) is None and \
                                data[0].get('tof.reflector.d', None) is None:
                                ioprole = 'refl'
                            else:
                                ioprole = 'init'
                            converter = IOPConverter(name=args.ioptype)
                            converter.convert(iopfile=f, ioprole=ioprole, data=data[0])
            except FileNotFoundError as e:
                log.error('Failed loading: %s, %s' % (args.iopfile, e))
                continue
        else:
            log.error('Failed loading: %s, unsupported extension: %s' % (file, f_ext))
            continue

        # Process input file / generate output file
        targets = {}
        targets['json'] = False
        log.info('Saving %s.xlsx' % (f_out,))
        for t in config['targets']:
            targets[t] = True

        if not config['combined']:
            # Create a new excel workbook per measurement
            wb = Workbook()
            wb.remove(wb.active)

        # Main routine to process npz file
        save_hadm(wb, data, f_token)

        if targets['json']:
            log.info('Saving %s.json' % (f_out,))
            with open(f_out + '.json', "w") as outfile:
                json.dump(data, outfile, cls=NestedDictEncoder)

        if len(wb.worksheets) != 0:
            wb.save(f_out + '.xlsx')

    log.info("Finished")

if __name__ == '__main__':
    main(sys.argv[1:])