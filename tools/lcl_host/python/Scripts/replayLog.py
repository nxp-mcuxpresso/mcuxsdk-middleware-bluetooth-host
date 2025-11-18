#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

# Call ranging engine from captured data
# Usage:  --pattern <folder>\*.npz --plot

import sys
import os
from pathlib import Path
import glob
import logging.config
import argparse
BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))
logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))

from nesteddict import NestedDict
from ranging_platform_pkg.resultfile import ResultFile
from ranging_platform_pkg.RangingPlatform import RangingPlatform
from ranging_platform_pkg.rng_platform_types import MeasurementModes
from convert_range_measurement_data import load_txt

import re
import matplotlib.pyplot as plt
import csv
import math

plot_dist = []
plot_true_dist = []
plot_err_dist_CDE = []
plot_err_dist_MUSIC = []


def create_csv_file(f_out):
    row_list = []
    outfile = open(f_out, 'w', newline='')
    writer = csv.writer(outfile)
    row_list.append(["True_dist", "iteration", "RSSI", "Music_dist", "Likeliness", "CDE_Dist", "CDE_DQI"])
    writer.writerows(row_list)
    return outfile, writer


def get_true_distance(file):
    # 20201117T170629_music_xyz_test_x_8p0_y_0p0_z_0p46.npz
    #match = re.findall("music_xyz_test_x_(\d+)\.?p?(\d+)_y_*", file)
    #true_dist = float(match[0][0] + "." + match[0][1])

    # 20201116T151256_music_xy_test_x_0p5_y_0p5
    match = re.findall("test_x_(\d+)\.?p?(\d+)_y_(\d+)\.?p?(\d+)_*", file)
    if (len(match) == 4):
        x = float(match[0][0] + "." + match[0][1])
        y = float(match[0][2] + "." + match[0][3])
        true_dist = math.sqrt(x*x + y*y)
    else:
        true_dist = 0

    return true_dist


def plot_results():
    fig, ((f1, f3), (f2, f4)) = plt.subplots(2, 2, figsize=(15, 30), sharex='col')

    f1.plot(plot_true_dist, plot_dist, 'bo')
    f1.set_ylabel('MUSIC dist (m)')
    #f1.set_xlabel('True dist (m)')
    f1.set_title('MUSIC distance vs True distance')
    f1.grid()

    f2.plot(plot_true_dist, plot_err_dist_MUSIC, 'ro')
    f2.set_ylabel('MUSIC dist error (m)')
    f2.set_xlabel('True dist (m)')
    #f2.set_title('MUSIC distance error vs True distance')
    f2.grid()

    hx, hy, _ = f3.hist(plot_err_dist_MUSIC, bins=50, color="lightblue")
    f3.set_ylim(0.0, max(hx) + 0.05)
    #f3.set_xlabel('MUSIC dist error (m)')
    f3.set_ylabel('Distribution')
    f3.set_title('MUSIC dist error distribution')
    f3.grid()
    plt.savefig("music_err_dist.png", bbox_inches='tight')

    #dx = hy[1] - hy[0]
    #cdf = numpy.cumsum(hx)
    #plt.plot(hy[1:], cdf)
    # plot the cumulative histogram
    f4.hist(plot_err_dist_MUSIC, bins=50, density=True, histtype='step', cumulative=True, color="green")
    f4.set_xlabel('MUSIC dist error (m)')
    f4.set_ylabel('CDF')
    f4.set_ylim(0.0, 1.0)
    #f4.set_title('MUSIC dist error CDF')
    f4.grid()

    plt.show()

def populate_plot_results(res, true_dist):
    dist = round(res['distance'], 3)
    plot_dist.append(dist)
    plot_true_dist.append(true_dist)
    plot_err_dist_MUSIC.append(abs(true_dist - dist))

def print_results(writer, res, i, rssi, true_dist):

    row = []

    row.append([true_dist, i, rssi, round(res['distance'], 3), res['CDE_distance'], res['CDE_dqi']])
    # print(row)
    writer.writerows(row)


def replay(writer, config, ranging_data, true_distance, run_range):

    # Loop on measurement entries
    i = 0
    for result in ranging_data:
        i += 1
        if not isinstance(result, NestedDict):
            print("Error: not a NestedDict instance")
            continue

        if result['meta.error_msg'] != '':
            print("Warning: meta.error_msg=%s", result['meta.error_msg'])

        if (run_range == 1):
            # Initialize RangingPlatform object
            ranging_pltf = RangingPlatform(board='kw45', usemaxbaudrate='False')
            ranging_pltf.distance_calculation_direct(result=result, antenna_pair_list=list(range(8)))

        res = result.get('mciq.result')
        print_results(writer, res, i, result.get('mciq.initiator.rssi'), true_distance)

        if config['plot_en']:
            populate_plot_results(res, true_distance)


def main(argv):

    log = logging.getLogger(__name__)
    log.setLevel(logging.DEBUG)
    log.info('Working on %s' % sys.platform)

    parser = argparse.ArgumentParser(description='convert measurement results of RangingPlatform')
    parser.add_argument('--pattern', action='store', type=str, default='*.npz', help='files to convert, use * as wildcard; default: ./input/*.txt')
    parser.add_argument('--output', action='store', type=str, help='output folder; default follows input directory')
    parser.add_argument('--true', action="store", type=str, default=None, help='Provide true distance')
    parser.add_argument('--plot', action="store_true", help='Enable plot')
    parser.add_argument('--norange', action="store_true", help='Valid only with .npz input: do not re-execute ranging')

    args = parser.parse_args()

    config = {
        'input_pattern': args.pattern,
        'output_directory': args.output,
        'plot_en': args.plot,
        'true_dist': args.true,
        'norange': args.norange
    }

    path = config['input_pattern']

    # Find most recent directory if input path not provided
    if os.path.dirname(path) == '':
        paths = [x for x in Path(".").iterdir() if x.is_dir()]
        most_recent_path = sorted(paths, key=os.path.getmtime, reverse=True)[0]
        path = os.path.join(most_recent_path, path)
    files = glob.glob(path)
    no_files = len(files)
    if no_files == 0:
        logging.log(logging.FATAL, 'No files match pattern (%s)' % (config['input_pattern']))
        return
    print("Found %d files using input pattern %s.." % (no_files, config['input_pattern']))

    # Create csv output
    f_base = 'concatenated_results.csv'
    f_dir, f_bnm = os.path.split(path)
    if config['output_directory'] is None:
        f_out = os.path.join(f_dir, f_base)
    else:
        f_out = os.path.join(config['output_directory'], f_base)
    outfile, writer = create_csv_file(f_out)

    rf = ResultFile()
    rf.initialize([MeasurementModes.MCIQ])

    # Process all files
    for file in files:

        if (config['true_dist']):
            true_distance = float(config['true_dist'])
        else:
            true_distance = get_true_distance(file)

        print("Process: " + file + "  (True distance:" + str(true_distance) + ")")

        f_dir, f_bnm = os.path.split(file)
        f_base, f_ext = os.path.splitext(f_bnm)

        if f_ext == '.txt':
            datatxt = load_txt(file, board='kw38', format='4')
            rf.extend(datatxt)
            data = list(rf)
            run_range = 0  # already done as part of load_txt()
        elif ['.npz', 'npy'].count(f_ext) != 0:
            try:
                rf.load(file)
                data = list(rf)
                if config['norange']:
                    run_range = 0
                else:
                    run_range = 1
            except:
                print('Failed loading npz')
                continue
        else:
            print('Failed loading: unsupported extension: %s', f_ext)
            continue

        print(str(len(data)) + " measurements present")

        replay(writer, config, data, true_distance, run_range)

    outfile.close()
    log.info("Finished")

    if config['plot_en']:
        plot_results()

if __name__ == '__main__':
    main(sys.argv)