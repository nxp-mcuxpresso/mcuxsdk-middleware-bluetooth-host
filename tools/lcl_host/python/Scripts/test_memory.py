# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import matplotlib
# The backend must be changed to 'Agg', to be able to run this script on a headless Linux-machine. Without this an
# error-message is shown that "display :0" cannot be opened. This must be done before importing some other packages,
# therefore it is done first.
matplotlib.use("Agg")

import numpy as np
import matplotlib.pyplot as plt
import os
import re
import argparse
import sys


def acquire_data(infile):
    with open(infile) as f:
        f = f.readlines()

    data = dict()
    paragraph = []
    for line in f:
        line = line.strip()
        if 'MEMTRACE ' not in line:
            continue
        columns = line.replace('MEMTRACE ', '').split(',')
        if len(columns) != 3:
            continue
        if columns[0] == 'peak':
            # syntax: peak,<test-name>,peak memory usage [bytes]
            data[columns[1] + f' (pk={int(int(columns[2])/1024)}kB)'] = paragraph
            paragraph = []
        elif columns[0] == 'malloc':
            # syntax: free,<#bytes-freed>,<#bytes-still-allocated>
            # syntax: malloc,<#bytes-allocated>,<#bytes-still-allocated>
            paragraph.append(int(columns[2]))

    return data


def plot_data(data, keyList, filename):
    fig = plt.figure(figsize=(8, 4.5))
    plt.grid()
    for key in keyList:
        y = np.asarray(data[key])/1024.0
        lines = plt.plot(y, linestyle='-', label=key)
        plt.plot(len(y) - 1, y[-1], 'o', color=lines[0].get_color())
    plt.xlabel('Memory allocation entry')
    plt.ylabel('Memory usage [kB]')
    plt.legend(loc='best')
    plt.title('Top %d memory usage during unit-tests' % len(keyList))
    plt.savefig(filename)


def get_top_mem_usage(data, n=2, excludes=[]):
    values = []
    keys = []

    values_tot = map(max, data.values())
    srt = sorted(zip(values_tot, data.keys()), reverse=True)

    for value, key in srt:
        if len(keys) == n:
            break
        for excl in excludes:
            if re.match(excl, key):
                break
        else:
            keys.append(key)
            values.append(value)

    return keys, values


def main(argv):
    parser = argparse.ArgumentParser(description='Input arguments for analyzing memory usage during unit-tests')
    parser.add_argument('-N', action='store', type=int, default=3, help='Shows top N memory-usage-test')
    parser.add_argument('-i', action='store', type=str, help='Input filename')
    parser.add_argument('--od', action='store', type=str, help='output folder; default follows input directory')
    parser.add_argument('--exclude', action='append', type=str, help='exclude testcase (regular expression')

    args = parser.parse_args(argv[1:])

    if args.exclude is None:
        args.exclude = []

    if args.od is None:
        basedir = os.path.dirname(os.path.realpath(args.i))
    else:
        basedir = os.path.dirname(os.path.realpath(args.od))
    filename = os.path.join(basedir, 'memory_usage.png')
    infile = args.i
    data = acquire_data(infile)
    keys, _ = get_top_mem_usage(data, n=args.N, excludes=args.exclude)

    plot_data(data, keyList=keys, filename=filename)


if __name__ == "__main__":
    main(sys.argv)
