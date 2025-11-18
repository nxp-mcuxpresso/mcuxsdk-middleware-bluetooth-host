# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import csv
from scipy.io import savemat, loadmat
from dirutils import ensure_dir

def save_raw(filename, raw):
    ensure_dir(filename)
    f = open(filename, "wb")
    if f is None:
        raise Exception("save_raw: failed saving file to %s" % filename)
    else:
        f.write(raw.encode())


def save_data(filename, fields, *args):
    if filename[-4:] == ".csv":
        data = []
        for v in args:
            data.append(v)
        try:
            csvfile = open(filename, 'wb')
            csvwriter = csv.writer(csvfile)
            csvwriter.writerow(fields)
            csvwriter.writerows(list(zip(*data)))
            csvfile.close()
        except:
            raise Exception("save_data: failed saving CSV file to %s" % filename)
    elif filename[-4:] == ".mat":
        data = {}
        for k, v in zip(fields, args):
            data[k] = v
        try:
            savemat(filename, data, appendmat=False, format='5', long_field_names=False,
                    do_compression=True, oned_as='row')
        except:
            raise Exception("save_data: failed saving MAT file to %s" % filename)
    else:
        raise Exception("save_data: unknown file extension")


def load_data(filename, type="int"):
    data = {}
    if filename[-4:] == ".csv":
        try:
            csvfile = open(filename, 'rb')
            csvreader = csv.reader(csvfile, delimiter=',')
            cfields = next(csvreader)
            for key in cfields:
                data[key] = []
            for row in csvreader:
                for key, idx in zip(cfields, list(range(len(cfields)))):
                    data[key].append(row[idx])
            try:
                for key in cfields:
                    if type == "int":
                        data[key] = list(map(int, data[key]))
                    elif type == "float":
                        data[key] = list(map(float, data[key]))
            except:
                raise Exception("load_data: failed reading CSV file %s. Could not convert to type." % filename)
            csvfile.close()
            return data
        except:
            raise Exception("load_data: failed reading CSV file to %s" % filename)
    elif filename[-4:] == ".mat":
        try:
            rdata = loadmat(filename, appendmat=False)
            for key, val in list(rdata.items()):
                if key[:2] != "__":  # ignore internal elements
                    data[key] = list(val[0])
            return data
        except:
            raise Exception("load_data: failed reading MAT file to %s" % filename)
    else:
        raise Exception("load_data: unknown file extension")
