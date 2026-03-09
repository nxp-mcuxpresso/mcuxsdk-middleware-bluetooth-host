# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2025 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
import os
import sys
import logging.config
import time

if __name__ == '__main__':
    BaseDir = os.path.dirname(os.path.realpath(__file__))
    sys.path.append(os.path.join(BaseDir, '..', '..', 'Instruments'))
    sys.path.append(os.path.join(BaseDir, '..', '..', 'Generic'))
    logging.config.fileConfig(os.path.join(BaseDir, '..', '..', 'logging.ini'))


import pandas as pd
import numpy as np
import numpy
import yaml
import copy
import json
from scipy.io import savemat, loadmat
from nesteddict import NestedDict
from dirutils import ensure_dir
from .validator import MyValidator
from .rng_platform_types import ReturnCodes
from json import JSONEncoder

import re
from pathlib import Path
import glob
import traceback
from importlib import import_module
from openpyxl import Workbook
from openpyxl.chart import Reference, Series, ScatterChart, LineChart
from openpyxl.styles import Font
import math
from .rng_platform_types import MeasurementModes


class NestedDictJsonEncoder(JSONEncoder):
    def default(self, o):
        if isinstance(o, np.ndarray):
            return np.ndarray.tolist(o)
        elif isinstance(o, complex):
            return str(o)
        else:
            return o.__dict__

class IOPConverter(object):
    def __new__(cls, **kwargs):
        identifier = kwargs.get('name', 'unspecified')
        modname = f'IOP_convert_{identifier}'
        try:
            # imported_module = import_module('.' + modname, package=__package__)
            imported_module = import_module(modname)
            return getattr(imported_module, modname)(**kwargs)
        except ModuleNotFoundError:
            raise Exception("Unknown IOP_convert_ identifier: %s" % (identifier,))



class ResultFile:
    mat_separator = '__'
    py_separator = '.'

    def __init__(self, *args, **kwargs):
        self._version = kwargs.get('version', 2)
        self._max_entries = kwargs.get('max_entries', 0)  # < 1 is unlimited
        if [1, 2].count(self._version) == 0:
            raise Exception(f'{self.__class__.__name__}: unknown version {self._version}')
        self._normalized = kwargs.get('normalized', True)  # purge unknown fields if True
        self._data = args[0] if len(args) else []

        filepath = os.path.dirname(os.path.realpath(__file__))
        with open(os.path.join(filepath, f'resultschema.{self._version}.yaml'), 'r') as stream:
            self._schema = NestedDict(yaml.safe_load(stream))

        self._pre_validator = None
        self._validator = None
        self._modes = []

        self.initialize(kwargs.get('modes', ['mciq']))
        self.__rotate()

    def __rotate(self):
        if len(self._data) > self._max_entries > 0:
            idx = len(self._data) - self._max_entries
            self._data = self._data[idx:]

    def initialize(self, modes):
        modes.sort()
        if self._modes == modes:
            return

        group_keys = list(filter(lambda x: x[-6:] == '.group' in x, self._schema.keys()))

        schema = copy.deepcopy(self._schema)
        for key in group_keys:
            parent_key = key[:-6]
            if key not in schema:
                continue
            groups = schema[key]
            for group in groups:
                if group in ['post', 'sizeable', 'info', 'hadm_cfg', 'md0']:
                    continue
                elif group not in modes:
                    del schema[parent_key]
                    break

        self._validator = MyValidator(schema.__dict__, purge_unknown=True)

        pre_schema = copy.deepcopy(schema)
        for key in group_keys:
            parent_key = key[:-6]
            if key not in pre_schema:
                continue
            groups = pre_schema[key]
            if 'post' in groups:
                del pre_schema[parent_key]

        self._pre_validator = MyValidator(pre_schema.__dict__, purge_unknown=True)
        self._modes = modes

    def is_item_valid(self, item):
        if self._version == 1:
            return item['ReturnCode'].is_valid()
        elif self._version == 2:
            return item['meta.error_msg'] == ''

    def set_item_error(self, item, msg):
        if self._version == 1:
            item['ReturnCode'] = ReturnCodes('validation')
        elif self._version == 2:
            item['meta.error_msg'] = msg

    def clear(self):
        self._data.clear()

    def extend(self, results):
        self._data.extend(results)
        self.__rotate()

    def validate(self, result, append=False, pre=False):
        if pre:
            append = False
            v = self._pre_validator.validate(result.__dict__)
            errors = self._pre_validator.errors
        else:
            v = self._validator.validate(result.__dict__)
            errors = self._validator.errors
        if self.is_item_valid(result) and not v:
            self.set_item_error(result, str(errors))
        if append:
            self._data.append(result)
            self.__rotate()
        return v

    def save(self, filename, normalized=None, sizeable=None):
        pys = self.py_separator
        mas = self.mat_separator
        (filepath, ext) = os.path.splitext(filename)
        ensure_dir(filename)
        log = logging.getLogger(__name__)
        try:
            if ext in ['.csv', '.xlsx']:
                df = self.to_dataframe(sizeable=False if sizeable is None else sizeable, normalized=normalized)
            elif ext in ['.mat']:
                df = self.to_dataframe(sizeable=True if sizeable is None else sizeable, normalized=normalized)
        except:
            raise Exception(f'{self.__class__.__name__}: failed creating DataFrame {filename}')
        try:
            if ext == '.csv':
                # Attempt to clean the CSV to remove the unwanted fields.
                # Some of the fields would require to be present in the python dictionary for the FWK functionality.
                # If one or more fields are removed from the resultschema, the entire filtering will fail, and the output will be unfiltered.
                try:
                    df = df.drop(columns=['meta.initiator.sw_version', 'meta.initiator.unique_id', 'meta.initiator.platform', 'meta.initiator.board_nr'])
                    df = df.drop(columns=['meta.reflector.sw_version', 'meta.reflector.unique_id', 'meta.reflector.platform', 'meta.reflector.board_nr'])
                    df = df.drop(columns=['meta.has_mciq', 'meta.has_tof'])
                except:
                    log.error("CSV file could not be filtered, this could be caused by modified resultschema file structure because columns are not found")
                df.to_csv(filename, sep=',', index=False, line_terminator='\n', encoding='ascii')
            elif ext == '.xlsx':
                df.to_excel(filename, sheet_name='resultfile', index=False)
                # Convert results
                cmd = 'convert_to_excel.py --silent --pattern ' + filepath + '.npz'
                # New function take same arguments of call system of convert_to_excel.py
                convert_to_excel(silent=0, pattern=filepath + '.npz')
                #os.system('py -3.6 ' + cmd)
            elif ext == '.npz':
                try:
                    np.savez_compressed(filename, resultfile=self._data, version=self._version)
                except Exception as e:
                    log.error(e)
            elif ext == '.json':
                with open(filename, "w") as outfile:
                    json.dump(self._data, outfile, cls=NestedDictJsonEncoder)
            elif ext == '.mat':
                df.columns = [mas.join(v.split(pys)) for v in df.columns.values]
                savemat(filename, {'resultfile': df.to_records(index=False),
                                   'columnnames': list(df.columns),
                                   'version': self._version},
                        long_field_names=True, do_compression=True, oned_as='column')
            elif ext == '.txt':
                if self._version == 1:
                    txt_config = {'copy': 'Copy', 'parameters': 'Info', 'response': 'Msg'}
                elif self._version == 2:
                    txt_config = {'copy': 'hidden.copy', 'parameters': 'meta.testcase', 'response': 'hidden.response'}
                fields = []
                msg_lst = []
                for item in self._data:
                    if item.get(txt_config['copy']):
                        continue
                    if len(fields) == 0 and txt_config['parameters'] in item:
                        fields = list(item.keys(txt_config['parameters']))
                    msg = '=== '
                    for field in fields:
                        msg += '%s = %s, ' % (field.split('.')[-1], item[field])
                    msg += ' ==='
                    msg += '\n' + item[txt_config['response']] + '\n'
                    msg_lst.append(msg)
                fh = open(filename, 'w')
                fh.write(''.join(msg_lst))
                fh.close()
            else:
                raise Exception(f'{self.__class__.__name__}: can not save {filename}, unsupported extension')
        except Exception as e:
            raise Exception(f'{self.__class__.__name__}: failed saving {filename}')


    def load(self, filename):
        pys = self.py_separator
        mas = self.mat_separator
        if not os.path.isfile(filename):
            raise Exception(f'Unable to load {filename}, does not exist')
        (_, ext) = os.path.splitext(filename)
        try:
            if ext == '.mat':
                # FIXME: loading empty string is not working
                myrec = loadmat(filename, squeeze_me=True)['resultfile']
                df = pd.DataFrame.from_records(myrec)
                df.columns = [pys.join(v.split(mas)) for v in df.columns.values]
                results = pd.DataFrame.to_dict(df, orient='records')
                results = list(map(lambda x: NestedDict(x), results))
                self._data = results
            elif ext == '.csv':
                # FIXME: loading empty string, ndarray is not working
                df = pd.read_csv(filename)
                df.columns = [pys.join(v.split(mas)) for v in df.columns.values]
                results = pd.DataFrame.to_dict(df, orient='records')
                results = list(map(lambda x: NestedDict(x), results))
                self._data = results
            elif ext == '.npz':
                x = np.load(filename, allow_pickle=True)
                self._data = x[x.files[0]].tolist()
            else:
                raise Exception(f'{self.__class__.__name__}: can not load {filename}, unsupported extension')
        except Exception as e:
            raise Exception(f'{self.__class__.__name__}: failed loading {filename, e}')

    def to_dataframe(self, normalized=None, sizeable=True):
        if normalized is None:
            normalized = self._normalized

        for item in self._data:
            if not isinstance(item, NestedDict) or not self.is_item_valid(item):
                continue
            if normalized:
                result = self._validator.normalized(item.__dict__)
                result = NestedDict(result)
                key_lst = list(result.keys())
            else:
                key_lst = list(item.keys())
            break
        else:
            raise Exception(f'No valid result exists')

        group_keys = list(filter(lambda x: x[-6:] == '.group' in x, self._schema.keys()))

        if not sizeable:
            for key in group_keys:
                field = key[:-6].replace('schema.', '')
                if 'sizeable' in self._schema[key] and field in key_lst:
                    key_lst.remove(field)

        table_data = {}
        for k in key_lst:
            table_data[k] = []

        for item in self._data:
            if not isinstance(item, NestedDict):
                continue
            for k in key_lst:
                table_data[k].append(item.get(k, np.nan))

        if len(table_data) == 0:
            raise Exception(f'{self.__class__.__name__}: can not create empty DataFrame')
        df = pd.DataFrame(table_data)
        return df

    def to_list(self):
        return self._data

    def __len__(self):
        return len(self._data)

    def __iter__(self):
        return self._data.__iter__()

    def __next__(self):
        return self._data.__next__()



# Utils to sort strings containing numbers in natural ordering (1,2,10,11 vs 1,10,11,2)
def atoi(text):
    return int(text) if text.isdigit() else text


def natural_keys(text):
    list = [atoi(c) for c in re.split(r'(\d+)', text)]
    return list


# Print string in bold (header) on column A
def print_header(sheet, line, name):
    sheet["A" + str(line)] = name
    sheet["A" + str(line)].font = Font(bold=True)


# Print name/value pair on columns A&B, at line 'line' in a sheet
def print_attribute(sheet, line, name, val):
    sheet["A" + str(line)] = name
    sheet["B" + str(line)] = val


def set_sheettab_color(sheet):
    sheet.sheet_properties.tabColor = get_sheet_tab_group_color()


sheet_tag_group_color = 0
COLOR_RED = 'E02020'
COLOR_ORANGE = 'E0E020'


def toggle_sheet_tab_group_color():
    global sheet_tag_group_color
    sheet_tag_group_color += 1


def get_sheet_tab_group_color():
    global sheet_tag_group_color
    return 'E0E0E0' if (sheet_tag_group_color % 2 == 0) else 'C0C0C0'


# Utils to convert record_range_measurement output to some human readable format
def utils_rttstat_to_cfo(rttstat, channel, bitrate):
    # extract RTT_STAT.RTT_CFO from register value
    cfo = (rttstat & 0xFFFF)
    cfo = numpy.short(cfo)
    # Approximate convertion from sfix16En15: CFO_hz=RTT_CFO*bitrate/2
    cfo = cfo * 15 if (bitrate == 1) else 30
    cfo_ppm = round(cfo / (2402.0 + channel), 2)
    return cfo_ppm


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

def generate_phase_sheet(wb, sheetName, stepNb, stepName, chanList, slopeRmse, antennaPathNum, initTabI, initTabQ,
                         reflTabI, reflTabQ, initTabTQI, reflTabTQI, combinedPhase, averagedIQ=True):
    # Create a sheet to store measurement
    sheet = wb.create_sheet(sheetName)
    sheet["C1"] = "ChanNum"
    sheet["D1"] = "MD I"
    sheet["E1"] = "MD Q"
    sheet["F1"] = "RD I"
    sheet["G1"] = "RD Q"
    sheet["H1"] = "MD phase norm."
    sheet["I1"] = "RD phase norm."
    sheet["J1"] = "Combined phase"
    sheet["K1"] = "Freq Mask"
    sheet["K2"] = "TBD"
    sheet["L1"] = "MD Ampl"
    sheet["M1"] = "RD Ampl"
    sheet["N1"] = "Comb Ampl"
    sheet["O1"] = "Unwrapped phase"
    sheet["P1"] = "Trend"
    sheet["Q1"] = "Phase Error"
    if initTabTQI is not None:
        sheet["R1"] = "MD TQI"
        sheet["S1"] = "RD TQI"

    numItems = 8
    col_start = 15
    for antennaPathIdx in range(1, antennaPathNum):
        sheet.cell(1, col_start + 0 + antennaPathIdx * numItems).value = "MD I " + str(antennaPathIdx)
        sheet.cell(1, col_start + 1 + antennaPathIdx * numItems).value = "MD Q " + str(antennaPathIdx)
        sheet.cell(1, col_start + 2 + antennaPathIdx * numItems).value = "RD I " + str(antennaPathIdx)
        sheet.cell(1, col_start + 3 + antennaPathIdx * numItems).value = "RD Q " + str(antennaPathIdx)
        sheet.cell(1, col_start + 4 + antennaPathIdx * numItems).value = "Comb phase " + str(antennaPathIdx)
        sheet.cell(1, col_start + 5 + antennaPathIdx * numItems).value = "Comb Ampl " + str(antennaPathIdx)
        if initTabTQI is not None:
            sheet.cell(1, col_start + 6 + antennaPathIdx * numItems).value = "MD TQI " + str(antennaPathIdx)
            sheet.cell(1, col_start + 7 + antennaPathIdx * numItems).value = "RD TQI " + str(antennaPathIdx)

    if combinedPhase is not None:
        sheet["K1"] = "Combined Embed"

    antennaPathIdx = 0
    if (antennaPathIdx == 0):
        comb_phase = []
        for stepId in range(0, stepNb):
            if initTabI is not None:
                md_i = int(initTabI[antennaPathIdx][stepId])
                md_q = int(initTabQ[antennaPathIdx][stepId])
            else:
                md_i = 0
                md_q = 0
            if reflTabI is not None:
                rd_i = int(reflTabI[antennaPathIdx][stepId])
                rd_q = int(reflTabQ[antennaPathIdx][stepId])
            else:
                rd_i = 0
                rd_q = 0
            md_phase = math.atan2(md_q, md_i)
            rd_phase = math.atan2(rd_q, rd_i)
            combined = md_phase + rd_phase
            if combined >= math.pi:
                combined -= 2 * math.pi
            elif combined < -math.pi:
                combined += 2 * math.pi
            comb_phase.append(combined)

            # print("Sample: " + str(sampleIdx), flush=True)
            row = str(2 + stepId)
            sheet["C" + row] = chanList[stepId]
            sheet["D" + row] = md_i
            sheet["E" + row] = md_q
            sheet["F" + row] = rd_i
            sheet["G" + row] = rd_q
            sheet["H" + row] = '=ATAN2(D{0},E{0})/PI()'.format(row)
            sheet["I" + row] = '=ATAN2(F{0},G{0})/PI()'.format(row)
            sheet[
                "J" + row] = '=IF((H{0} + I{0}) >= 1, (H{0} + I{0}) - 2, IF((H{0} + I{0}) < (-1), (H{0} + I{0}) + 2, H{0} + I{0}))'.format(
                row)

            if combinedPhase is not None:
                sheet["K" + row] = combinedPhase[stepId]
            sheet["L" + row] = '=SQRT(D{0}*D{0}+E{0}*E{0})'.format(row)
            sheet["M" + row] = '=SQRT(F{0}*F{0}+G{0}*G{0})'.format(row)
            sheet["N" + row] = '=(L{0}+M{0})/2'.format(row)

        if initTabI is not None and reflTabI is not None:
            # Generate unwrapped combined phase in radian
            comb_phase_np = numpy.array(comb_phase)
            chan_list_np = numpy.fromiter(chanList, int)
            comb_phase_sorted_np = comb_phase_np[chan_list_np.argsort()]  # sort combined phase
            # numpy.argsort() returns the indices that would sort an array
            chan_list_np.sort()  # sort channel list

            try:
                comb_phase_sorted_np = numpy.unwrap(comb_phase_sorted_np)
                fit = numpy.polyfit(chan_list_np, comb_phase_sorted_np, 1)
                # trend= a*x+b
                trend_line = [f * fit[0] + fit[1] for f in chan_list_np]
            except Exception as e:
                print(e)

            # Compute Root Mean Square Error in degrees
            difference_array = numpy.subtract(comb_phase_sorted_np, trend_line)
            squared_array = numpy.square(difference_array)
            slopeRmse = numpy.sqrt(squared_array.mean()) * 180 / math.pi

            # Compute phase slope
            difference_array = numpy.subtract(comb_phase_sorted_np, trend_line)
            slope_dist = ((-1) * fit[0] * 3.0e8) / (4 * math.pi * 1.0e6)
            print_header(sheet, 11, "First APath (excel):")
            print_attribute(sheet, 12, "Phase slope", fit[0])
            print_attribute(sheet, 13, "Slope dist (m)", slope_dist)
            print_attribute(sheet, 14, "RMSE (deg)", slopeRmse);

            for stepId in range(0, stepNb):
                row = str(2 + stepId)
                sheet["O" + row] = comb_phase_sorted_np[stepId]
                sheet["P" + row] = trend_line[stepId]
                sheet["Q" + row] = (difference_array[stepId] * 180) / math.pi
                if initTabTQI is not None:
                    sheet["R" + row] = initTabTQI[0][stepId]
                    sheet["S" + row] = reflTabTQI[0][stepId]

        for antennaPathIdx in range(1, antennaPathNum):
            for stepId in range(0, stepNb):
                # print("Sample: " + str(sampleIdx), flush=True)
                row = (2 + stepId)
                MdI = int(initTabI[antennaPathIdx][stepId])
                MdQ = int(initTabQ[antennaPathIdx][stepId])
                RdI = int(reflTabI[antennaPathIdx][stepId])
                RdQ = int(reflTabQ[antennaPathIdx][stepId])
                PhiComb = math.atan2(MdQ, MdI) / math.pi + math.atan2(RdQ, RdI) / math.pi
                if (PhiComb >= 1):
                    PhiComb = PhiComb - 2
                elif (PhiComb < (-1)):
                    PhiComb = PhiComb + 2

                sheet.cell(row, col_start + 0 + antennaPathIdx * numItems).value = MdI
                sheet.cell(row, col_start + 1 + antennaPathIdx * numItems).value = MdQ
                sheet.cell(row, col_start + 2 + antennaPathIdx * numItems).value = RdI
                sheet.cell(row, col_start + 3 + antennaPathIdx * numItems).value = RdQ
                sheet.cell(row, col_start + 4 + antennaPathIdx * numItems).value = PhiComb
                sheet.cell(row, col_start + 5 + antennaPathIdx * numItems).value = (math.sqrt(
                    MdI * MdI + MdQ * MdQ) + math.sqrt(RdI * RdI + RdQ * RdQ)) / 2
                if initTabTQI is not None:
                    sheet.cell(row, col_start + 6 + antennaPathIdx * numItems).value = int(
                        initTabTQI[antennaPathIdx][stepId])
                    sheet.cell(row, col_start + 7 + antennaPathIdx * numItems).value = int(
                        reflTabTQI[antennaPathIdx][stepId])

        if (averagedIQ):
            datax_channels = Reference(sheet, min_col=3, min_row=2, max_col=3, max_row=stepNb + 1)

            # Draw combined phase vs channel for averaged IQs
            c = ScatterChart()
            c.height = 7.0
            c.width = 20.0
            column_plot = 10
            data = Reference(sheet, min_col=column_plot, min_row=2, max_col=column_plot, max_row=stepNb + 1)
            series = Series(values=data, xvalues=datax_channels)
            series.graphicalProperties.line.noFill = True
            c.series.append(series)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.legend = None
            c.y_axis.title = 'Normalized combined phase'
            c.x_axis.title = 'Channel number'
            c.x_axis.majorUnit = 2
            c.title = "Normalized Combined Phase"
            sheet.add_chart(c, "H5")

            # Draw combined phase vs step
            c = LineChart()
            c.height = 7.0
            c.width = 20.0
            column_plot = 10
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.title = "Normalized combined phase vs step"
            c.x_axis.title = 'Step id'
            c.x_axis.majorUnit = 2
            sheet.add_chart(c, "T49")

            # Draw Combined Phase unwrapped & trendline
            c = LineChart()
            c.height = 7.0
            c.width = 20.0
            column_plot = 15
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            column_plot = 16
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.series[1].marker.symbol = None
            c.series[1].marker.size = 2
            c.title = "Combined Phase unwrapped"
            c.x_axis.title = 'Channel number'
            c.y_axis.title = 'unwrapped phase'
            c.x_axis.majorUnit = 2
            sheet.add_chart(c, "T5")

            # Draw Phase Error
            c = ScatterChart()
            c.height = 7.0
            c.width = 20.0
            column_plot = 17
            data = Reference(sheet, min_col=column_plot, min_row=2, max_col=column_plot, max_row=stepNb + 1)
            series = Series(values=data, xvalues=datax_channels)
            series.graphicalProperties.line.noFill = True
            c.series.append(series)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.legend = None
            c.y_axis.title = 'Phase Error (deg)'
            c.x_axis.title = 'Channel number'
            c.x_axis.majorUnit = 2
            c.title = "Phase Error vs channel"
            sheet.add_chart(c, "T19")

            c = LineChart()
            c.height = 7.0
            c.width = 20.0
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.title = "Phase Error vs step"
            c.x_axis.title = 'Step id'
            c.y_axis.title = 'Phase Error (deg)'
            c.x_axis.majorUnit = 2
            sheet.add_chart(c, "T34")
        else:
            # Draw combined phase vs channel for non-averaged IQs (may have several IQs per step)
            c = LineChart()
            c.height = 7.0
            c.width = 20.0
            column_plot = 10
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.title = "Combined Phase"
            sheet.add_chart(c, "H5")

            # Draw Combined Phase unwrapped & trendline
            c = LineChart()
            c.height = 7.0
            c.width = 20.0
            column_plot = 15
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            column_plot = 16
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            c.add_data(data, titles_from_data=True)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.series[1].marker.symbol = None
            c.series[1].marker.size = 2
            c.title = "Combined Phase unwrapped"
            sheet.add_chart(c, "T5")

    # Draw Amplitude over channels
    datax_channels = Reference(sheet, min_col=3, min_row=2, max_col=3, max_row=stepNb + 1)
    c = ScatterChart()
    c.height = 7.0
    c.width = 20.0
    column_plot = 12
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
    series = Series(values=data, xvalues=datax_channels, title_from_data=True)
    series.graphicalProperties.line.noFill = True
    c.series.append(series)
    column_plot = column_plot + 1
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
    series = Series(values=data, xvalues=datax_channels, title_from_data=True)
    series.graphicalProperties.line.noFill = True
    c.series.append(series)
    c.y_axis.title = 'IQ Amplitude'
    c.x_axis.title = 'Channel number'
    c.series[0].marker.symbol = "auto"
    c.series[0].marker.size = 4
    c.series[1].marker.symbol = "auto"
    c.series[1].marker.size = 4
    c.title = "Amplitude"
    sheet.add_chart(c, "H19")

    # Draw I&Q MD over steps
    c = LineChart()
    c.height = 7.0
    c.width = 20.0
    column_plot = 4
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
    c.add_data(data, titles_from_data=True)
    column_plot = column_plot + 1
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
    c.add_data(data, titles_from_data=True)
    c.title = "MD I&Q"
    sheet.add_chart(c, "H33")

    # Draw I&Q RD over steps
    c = LineChart()
    c.height = 7.0
    c.width = 20.0
    column_plot = 6
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
    c.add_data(data, titles_from_data=True)
    column_plot = column_plot + 1
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
    c.add_data(data, titles_from_data=True)
    c.title = "RD I&Q"
    sheet.add_chart(c, "H47")

    # Draw I vs Q MD
    c = ScatterChart()
    c.height = 10
    c.width = 10
    datax = Reference(sheet, min_col=4, min_row=2, max_col=4, max_row=stepNb + 1)
    datay = Reference(sheet, min_col=5, min_row=2, max_col=5, max_row=stepNb + 1)
    series = Series(values=datay, xvalues=datax)
    series.graphicalProperties.line.noFill = True
    c.series.append(series)
    c.series[0].marker.symbol = "circle"
    c.series[0].marker.size = 5
    c.legend = None
    c.title = "MD I vs Q"
    sheet.add_chart(c, "H61")

    # Draw I vs Q RD
    c = ScatterChart()
    c.height = 10
    c.width = 10
    datax = Reference(sheet, min_col=6, min_row=2, max_col=6, max_row=stepNb + 1)
    datay = Reference(sheet, min_col=7, min_row=2, max_col=7, max_row=stepNb + 1)
    series = Series(values=datay, xvalues=datax)
    series.graphicalProperties.line.noFill = True
    c.series.append(series)
    c.series[0].marker.symbol = "circle"
    c.series[0].marker.size = 5
    c.legend = None
    c.title = "RD I vs Q"
    sheet.add_chart(c, "N61")

    # Plots in case of multi-antenna pathes
    if (antennaPathNum > 1):

        # Draw combined phase vs channel
        c = ScatterChart()
        c.height = 7.0
        c.width = 30.0
        column_plot = 10
        datax_channels = Reference(sheet, min_col=3, min_row=2, max_col=3, max_row=stepNb + 1)
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax_channels, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        c.series[0].marker.symbol = "auto"
        c.series[0].marker.size = 4
        c.y_axis.title = 'Normalized phase'
        c.x_axis.title = 'Channel number'
        c.x_axis.majorUnit = 2

        for antennaPathIdx in range(1, antennaPathNum):
            column_plot = col_start + 4 + antennaPathIdx * numItems
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            series = Series(values=data, xvalues=datax_channels, title_from_data=True)
            series.graphicalProperties.line.noFill = True
            c.series.append(series)
            c.series[antennaPathIdx].marker.symbol = "auto"
            c.series[antennaPathIdx].marker.size = 4

        c.title = "Normalized combined phase per AP"
        sheet.add_chart(c, "AF5")

        # Draw Amplitude vs channels frequencies
        c = ScatterChart()
        c.height = 20.0
        c.width = 30.0
        c.y_axis.title = 'Amplitude'
        c.x_axis.title = 'Channel number'
        column_plot = 14
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax_channels, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        c.series[0].marker.symbol = "auto"
        c.series[0].marker.size = 4

        for antennaPathIdx in range(1, antennaPathNum):
            column_plot = col_start + 5 + antennaPathIdx * numItems
            data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
            series = Series(values=data, xvalues=datax_channels, title_from_data=True)
            series.graphicalProperties.line.noFill = True
            c.series.append(series)
            c.series[antennaPathIdx].marker.symbol = "auto"
            c.series[antennaPathIdx].marker.size = 4

        c.title = "Combined Amplitude per AP"
        sheet.add_chart(c, "AF19")

        for antennaPathIdx in range(1, antennaPathNum):
            # I vs Q charts
            c = ScatterChart()
            c.height = 10
            c.width = 10
            column_plot = col_start + 0 + antennaPathIdx * numItems
            datax = Reference(sheet, min_col=column_plot, min_row=2, max_col=column_plot, max_row=stepNb + 1)
            datay = Reference(sheet, min_col=column_plot + 1, min_row=2, max_col=column_plot + 1, max_row=stepNb + 1)
            series = Series(values=datay, xvalues=datax)
            series.graphicalProperties.line.noFill = True
            c.series.append(series)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.legend = None
            c.title = "MD I vs Q (ant path = " + str(antennaPathIdx) + ")"
            sheet.add_chart(c, 'AF{0}'.format(5 + (antennaPathIdx - 1) * c.height * 2))

            c = ScatterChart()
            c.height = 10
            c.width = 10
            column_plot = col_start + 2 + antennaPathIdx * numItems
            datax = Reference(sheet, min_col=column_plot, min_row=2, max_col=column_plot, max_row=stepNb + 1)
            datay = Reference(sheet, min_col=column_plot + 1, min_row=2, max_col=column_plot + 1, max_row=stepNb + 1)
            series = Series(values=datay, xvalues=datax)
            series.graphicalProperties.line.noFill = True
            c.series.append(series)
            c.series[0].marker.symbol = "circle"
            c.series[0].marker.size = 5
            c.legend = None
            c.title = "RD I vs Q (ant path = " + str(antennaPathIdx) + ")"
            sheet.add_chart(c, 'AL{0}'.format(5 + (antennaPathIdx - 1) * c.height * 2))

    return sheet


def generate_measure_phase(wb, measId, result):
    stepNb = int(result.get('mciq.cfg.n_stp'))
    antennaPathNum = int(result.get('mciq.cfg.n_ap'))
    initTabI = None
    initTabQ = None
    initTabTQI = None
    reflTabI = None
    reflTabQ = None
    reflTabTQI = None

    try:
        stepName, chanList = step_seq(result.get('hadm.stp.modes'), [2, 3], result.get('hadm.stp.channels'))
    except:
        # If hadm is not available
        stepName = ["IQ #" + str(i) for i in range(stepNb)]
        chanList = [i for i in range(stepNb)]

    # Inverse antennaId and stepId indexing (transpose matrix)
    try:
        initTabI = result.get('mciq.initiator.i').transpose()
        initTabQ = result.get('mciq.initiator.q').transpose()
        initTabTQI = result.get('mciq.initiator.tqi').transpose()
    except Exception as e:
        print(e)
        pass

    try:
        reflTabI = result.get('mciq.reflector.i').transpose()
        reflTabQ = result.get('mciq.reflector.q').transpose()
        reflTabTQI = result.get('mciq.reflector.tqi').transpose()
    except Exception as e:
        print(e)
        pass

    combinedPhase = None
    slope_rmse = result.get('mciq.result.slope_rmse', 0)

    sheetName = 'IQ' + measId
    sheet = generate_phase_sheet(wb, sheetName, stepNb, stepName, chanList, slope_rmse, antennaPathNum, initTabI,
                                 initTabQ, reflTabI, reflTabQ, initTabTQI, reflTabTQI, combinedPhase)
    sheet.sheet_properties.tabColor = get_sheet_tab_group_color()

    # Print measurement results
    sheet["A1"] = "Slope dist"
    sheet["A2"] = "Slope RMSE"
    sheet["A3"] = "CDE dist"
    sheet["A4"] = "CDE DQI"
    sheet["A5"] = "RADE dist"
    sheet["A6"] = "RADE DQI"

    if 'result' in result.get('mciq'):
        if 'distance' in result.get('mciq.result'):
            sheet["B1"] = round(float(result.get('mciq.result.distance')), 2)
            sheet["B2"] = str(slope_rmse)
        else:
            sheet["B1"] = sheet["B2"] = "Not run"

        if 'CDE_distance' in result.get('mciq.result'):
            sheet["B3"] = float(result.get('mciq.result.CDE_distance'))
            sheet["B4"] = float(result.get('mciq.result.CDE_dqi'))
        else:
            sheet["B3"] = sheet["B4"] = "Not run"

        if 'RADE' in result.get('mciq.result'):
            sheet["B5"] = float(result.get('mciq.result.RADE'))
            sheet["B6"] = float(result.get('mciq.result.RADE_dqi'))
        else:
            sheet["B5"] = sheet["B6"] = "Not run"

def generate_measure_phase_debug(wb, measId, result, mode0=False):
    try:
        stepNb = len(result.get('dbg.init.iq'))
    except:
        # no debug IQ available
        return

    antennaPathNum = 1  # For debug IQs plots, consider IQ falt array, whatever N_AP is

    initTabI = []
    initTabQ = []
    reflTabI = []
    reflTabQ = []
    stepName = []
    chanList = []
    initIfMixIdx = []
    reflIfMixIdx = []
    samplesTotal = 0
    # Concatenate IQs from all steps
    for stepId in range(0, stepNb):
        samplesNb = int(len(result.get('dbg.init.iq')[stepId]) / 2)  # Half of IQ array size
        isMode0 = (result.get('hadm.stp.modes')[stepId] == 0)
        channelNum = result.get('hadm.stp.channels')[stepId]

        # Do not print mode0 if not requested
        if mode0 != isMode0:
            continue

        samplesTotal += samplesNb
        stepName.extend([stepId] * samplesNb)
        chanList.extend([channelNum] * samplesNb)
        # Initiator data
        initTabI.extend(result.get('dbg.init.iq')[stepId][0::2])
        initTabQ.extend(result.get('dbg.init.iq')[stepId][1::2])
        try:
            initIfMixIdx.extend(result.get('dbg.init.ifdx')[stepId])
        except:
            pass

        if isMode0:
            # Reflector data: for mode0, print zeroed values (mode0 not available on reflector)
            reflTabI.extend([0] * samplesNb)
            reflTabQ.extend([0] * samplesNb)
            reflIfMixIdx.extend([0] * samplesNb)
        else:
            reflTabI.extend(result.get('dbg.refl.iq')[stepId][0::2])
            reflTabQ.extend(result.get('dbg.refl.iq')[stepId][1::2])
            try:
                reflIfMixIdx.extend(result.get('dbg.refl.ifdx')[stepId])
            except:
                pass

    initTabI = (initTabI,)
    initTabQ = (initTabQ,)
    reflTabI = (reflTabI,)
    reflTabQ = (reflTabQ,)
    initIfMixIdx = (initIfMixIdx,)
    reflIfMixIdx = (reflIfMixIdx,)

    slope_rmse = result.get('mciq.result.slope_rmse', 0)

    sheetName = ("mod0-" if mode0 else "") + "dbg" + measId

    sheet = generate_phase_sheet(wb, sheetName, samplesTotal, stepName, chanList, slope_rmse, antennaPathNum, initTabI,
                                 initTabQ, reflTabI, reflTabQ, None, None, None, False)
    sheet.sheet_properties.tabColor = get_sheet_tab_group_color()

    try:
        for stepId in range(0, samplesTotal):
            row = str(2 + stepId)
            sheet["O" + row] = int(initIfMixIdx[0][stepId])
            sheet["P" + row] = int(reflIfMixIdx[0][stepId])
        sheet["O1"] = "MD ifmix"
        sheet["P1"] = "RD ifmix"
    except:
        pass


def generate_measure_summary(log, measId, result, checkConsistency=False):
    # Create a sheet to store measurement details
    sheet = log.create_sheet(measId)
    sheet.sheet_properties.tabColor = get_sheet_tab_group_color()
    line = 1

    print_header(sheet, line, "Status");
    sheet.column_dimensions['A'].width = 15
    line += 1
    status = 'OK'
    if result['meta.error_msg'] != '':
        status = result['meta.error_msg']
    sheet["A2"] = status;
    if status == 'OK' and checkConsistency:
        try:
            # Sanity check for test mode (should not occur in connected mode)
            assert len(result.get('tof.initiator.d', [])) == len(result.get('tof.reflector.d', []))
            assert len(result.get('mciq.initiator.i', [])) == len(result.get('mciq.reflector.i', []))
            assert len(result.get('mciq.initiator.q', [])) == len(result.get('mciq.reflector.q', []))
        except Exception as e:
            status = "FAILED - no reflector data"


    line += 1
    line += 1

    # Active configuration
    print_header(sheet, line, "Configuration");
    line += 1
    print_attribute(sheet, line, "T_FCS", result.get('hadm.cfg.fcs'));
    line += 1
    print_attribute(sheet, line, "T_IP1", result.get('hadm.cfg.ip1'));
    line += 1
    print_attribute(sheet, line, "T_IP2", result.get('hadm.cfg.ip2'));
    line += 1
    print_attribute(sheet, line, "T_PM", result.get('hadm.cfg.tpm'));
    line += 1
    print_attribute(sheet, line, "RTT Type", result.get('hadm.cfg.rtyp'));
    line += 1
    print_attribute(sheet, line, "RTT PHY", result.get('hadm.cfg.rphy'));
    line += 1
    try:
        print_attribute(sheet, line, "N_AP", result.get('mciq.cfg.n_ap'));
        line += 1
    except:
        line += 1
    line += 1

    # Sync info
    print_header(sheet, line, "Sync Info");
    line += 1
    print_attribute(sheet, line, "Sync stepId", result.get('info.init.sync_step_id', ''));
    line += 1
    print_attribute(sheet, line, "Sync AGCIdx", result.get('info.init.sync_agc', ''));
    line += 1
    print_attribute(sheet, line, "Sync CFO(Hz)", result.get('info.init.sync_cfo', ''));
    line += 1
    print_attribute(sheet, line, "Sync RSSI(dBm)", result.get('info.init.sync_rssi', ''));
    line += 1
    line += 1

    # Init info
    print_header(sheet, line, "Init Info");
    line += 1
    print_attribute(sheet, line, "Flags", hex(result.get('info.init.flags', 0)));
    line += 1
    print_attribute(sheet, line, "Temperature", result.get('info.init.temperature'));
    line += 1
    print_attribute(sheet, line, "XTAL trim", result.get('info.init.xtal_trim'));
    line += 1
    print_attribute(sheet, line, "Time Adj", result.get('info.init.time_adj'));
    line += 1
    line += 1

    # Refl info
    print_header(sheet, line, "Refl Info");
    line += 1
    print_attribute(sheet, line, "Flags", hex(result.get('info.refl.flags', 0)));
    line += 1
    print_attribute(sheet, line, "Temperature", result.get('info.refl.temperature'));
    line += 1
    print_attribute(sheet, line, "XTAL trim", result.get('info.refl.xtal_trim'));
    line += 1
    print_attribute(sheet, line, "Time Adj", result.get('info.refl.time_adj'));
    line += 1
    line += 1

    if status != 'OK':
        sheet.sheet_properties.tabColor = COLOR_RED
        return status

    # Steps details
    sheet["D1"] = "StepId"
    sheet["E1"] = "EvtIdx"
    sheet["F1"] = "SubevtIdx"
    sheet["G1"] = "Mode"
    sheet["H1"] = "Channel"
    sheet["I1"] = "RSSI MD (dBm)"
    sheet["J1"] = "RSSI RD (dBm)"
    sheet["K1"] = "CFO MD (ppm)"
    sheet["L1"] = "CFO RD (ppm)"

    try:
        try:
            subeventStatus = np.repeat(0, len(result.get('hadm.stp.subevt')))
        except:
            subeventStatus = [1]
        stepNb = result.get('hadm.stp.nb', 0)
        mode0_cnt = 0
        tof_cnt = 0
        subevtIdx = 0
        sheet["I2"] = result.get('mciq.initiator.cfo')
        for stepId in range(0, stepNb):
            rssi_init = 127
            rssi_refl = 127
            cfo_init = 0xFFFFFFFF
            cfo_refl = 0xFFFFFFFF
            row = str(2 + stepId)
            try:
                # Find next non empty subevent
                while stepId > (result.get('hadm.stp.subevt')[subevtIdx] - 1):
                    subevtIdx += 1
                subeventStatus[subevtIdx] = 1
                eventIdx = int(result.get('hadm.stp.event')[subevtIdx])
            except:
                eventIdx = 0
                subevtIdx = 0
            sheet["D" + row] = stepId
            sheet["E" + row] = eventIdx
            sheet["F" + row] = subevtIdx
            sheet["G" + row] = result.get('hadm.stp.modes')[stepId]
            sheet["H" + row] = result.get('hadm.stp.channels')[stepId]

            if (result.get('hadm.stp.modes')[stepId] == 0):  # Mode0 step
                try:
                    rssi_init = result.get('md0.init.rssi')[mode0_cnt]
                    # Convert 0.01 ppm integer value to ppm
                    cfo_init = result.get('md0.init.cfo')[mode0_cnt] / 100.0
                except Exception:
                    # No RSSI data
                    pass
                try:
                    rssi_refl = result.get('md0.refl.rssi')[mode0_cnt]
                    cfo_refl = 0xFFFFFFFF
                except Exception:
                    # No RSSI data
                    pass
                mode0_cnt += 1

            if (result.get('hadm.stp.modes')[stepId] == 1) or (result.get('hadm.stp.modes')[stepId] == 3):  # RTT step
                try:
                    rssi_init = result.get('tof.initiator.rssi')[tof_cnt]
                    if tof_cnt < len(result.get('dbg.init.rttstat')):
                        rttstat_init = result.get('dbg.init.rttstat')[tof_cnt]
                        cfo_init = utils_rttstat_to_cfo(rttstat_init, result.get('hadm.stp.channels')[stepId], 1)
                except Exception:
                    # No RSSI data
                    pass
                try:
                    rssi_refl = result.get('tof.reflector.rssi')[tof_cnt]
                    if tof_cnt < len(result.get('dbg.refl.rttstat')):
                        rttstat_refl = result.get('dbg.refl.rttstat')[tof_cnt]
                        cfo_refl = utils_rttstat_to_cfo(rttstat_refl, result.get('hadm.stp.channels')[stepId], 1)
                except Exception:
                    # No RSSI data
                    pass
                tof_cnt += 1

            if (rssi_init != 127):
                sheet["I" + row] = rssi_init
            if (rssi_refl != 127):
                sheet["J" + row] = rssi_refl
            if (cfo_init != 0xFFFFFFFF):
                sheet["K" + row] = cfo_init
            if (cfo_refl != 0xFFFFFFFF):
                sheet["L" + row] = cfo_refl

        # Update measurement status on top
        if status == 'OK' and not all(x == 1 for x in subeventStatus):
            status = "OK - subevents missed: " + str([i for i in range(len(subeventStatus)) if subeventStatus[i] == 0])
        sheet.column_dimensions['A'].width = 15
        sheet["A2"] = status;
        if status != 'OK':
            sheet.sheet_properties.tabColor = COLOR_ORANGE

        # RSSI plot
        datax = Reference(sheet, min_col=4, min_row=1, max_col=4, max_row=stepNb + 1)
        c = ScatterChart()
        c.height = 10.0
        c.width = 25.0

        column_plot = 9
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        column_plot = 10
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        c.series[0].marker.symbol = "auto"
        c.series[0].marker.size = 5
        c.series[1].marker.symbol = "auto"
        c.series[1].marker.size = 5
        c.y_axis.title = 'RSSI (dBm)'
        c.x_axis.title = 'Step Id'
        c.x_axis.majorUnit = 5
        c.title = "RSSI vs step Id"
        sheet.add_chart(c, "M2")

        # CFO plot
        datax = Reference(sheet, min_col=6, min_row=1, max_col=6, max_row=stepNb + 1)
        c = ScatterChart()
        c.height = 10.0
        c.width = 25.0

        column_plot = 11
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        column_plot = 12
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        c.series[0].marker.symbol = "auto"
        c.series[0].marker.size = 5
        c.series[1].marker.symbol = "auto"
        c.series[1].marker.size = 5
        c.y_axis.title = 'CFO (ppm)'
        c.x_axis.title = 'Step Id'
        c.x_axis.majorUnit = 5
        c.title = "CFO (ppm) vs step Id"
        sheet.add_chart(c, "M23")

        # Channels plot
        datax = Reference(sheet, min_col=4, min_row=1, max_col=4, max_row=stepNb + 1)
        c = ScatterChart()
        c.height = 10.0
        c.width = 25.0

        column_plot = 8
        data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=stepNb + 1)
        series = Series(values=data, xvalues=datax, title_from_data=True)
        series.graphicalProperties.line.noFill = True
        c.series.append(series)
        c.series[0].marker.symbol = "auto"
        c.series[0].marker.size = 5
        c.series[0].marker.graphicalProperties.solidFill = "FF0000"  # Marker filling
        c.y_axis.title = 'Channel #'
        c.x_axis.title = 'Step Id'
        c.x_axis.majorUnit = 5
        c.title = "Channel number vs step Id"
        sheet.add_chart(c, "M43")

    except Exception as e:
        print("Exception " + str(e))
        traceback.print_exc()
        pass

    return status


# Build a dict of {stepIds, channelNum} based on given mode filter
def step_seq(modes, filter, channels):
    stepIds = []
    chanNums = []
    for i in range(0, len(modes)):
        if modes[i] in filter:
            stepIds.append(i)
            chanNums.append(channels[i])
    return stepIds, chanNums


# return {'Hest': H_est, 'Uncertainty': [np.NaN] * Ka}
def generate_measure_tof(wb, measId, result):
    elements_init = len(result.get('tof.initiator.d', ()))
    elements_refl = len(result.get('tof.reflector.d', ()))

    if elements_init == 0 and elements_refl == 0:
        # No ToF data
        return
    if elements_init != elements_refl and elements_init != 0 and elements_refl != 0:
        # Inconsistent result
        return
    elements = max(elements_init, elements_refl)

    # Create a sheet to store measurement
    sheet = wb.create_sheet("ToF" + measId)
    sheet.sheet_properties.tabColor = get_sheet_tab_group_color()
    line = 1

    print_attribute(sheet, line, "ns Avg", '=AVERAGEIF(H:H,"<>0")');
    line += 1
    print_attribute(sheet, line, "Dist Raw", '=B1/2*0.3');
    line += 1
    print_attribute(sheet, line, "Dist fw", result.get('tof.result.distance'));
    line += 1
    print_attribute(sheet, line, "ns stddev", '=STDEV(INDIRECT("H2:H"&ROWS(H:H)))');
    line += 1

    sheet["D1"] = "MD ns"
    sheet["E1"] = "MD Error"
    sheet["F1"] = "RD ns"
    sheet["G1"] = "RD Error"
    sheet["H1"] = "ToF *2 (ns)"
    sheet["I1"] = "NADM MD"
    sheet["J1"] = "NADM RD"

    nbError = 0
    for sampleIdx in range(0, elements):
        # print("Sample: " + str(sampleIdx), flush=True)
        row = str(2 + sampleIdx)
        # Only append RTT that got no error during packet exchange (see TSFLAGS_ERROR_MASK)
        # mask = 0x8000 # For kw38
        mask = 0xF00000  # For kw45

        if (elements_init != 0):
            tsInitiator = int(result.get('tof.initiator.d')[sampleIdx])
            try:
                nadmInitiator = int(result.get('tof.initiator.nadm')[sampleIdx])
                qualityInitiator = result.get('tof.initiator.quality')[sampleIdx]
            except:
                nadmInitiator = 0xF
                qualityInitiator = 0x2
        else:
            tsInitiator = 0
            nadmInitiator = 0xF
            qualityInitiator = 0x2

        if (qualityInitiator == 0):
            sheet["D" + row] = tsInitiator
            sheet["I" + row] = nadmInitiator
        else:
            sheet["E" + row] = '0x{:02X}'.format(qualityInitiator)

        if (elements_refl != 0):
            tsReflector = int(result.get('tof.reflector.d')[sampleIdx])
            try:
                nadmReflector = int(result.get('tof.reflector.nadm')[sampleIdx])
                qualityReflector = result.get('tof.reflector.quality')[sampleIdx]
            except:
                nadmReflector = 0xF
                qualityReflector = 0x2
        else:
            tsReflector = 0
            nadmReflector = 0xF
            qualityReflector = 0x2

        if (qualityReflector == 0):
            sheet["F" + row] = tsReflector
            sheet["J" + row] = nadmReflector
        else:
            sheet["G" + row] = '0x{:02X}'.format(qualityReflector)

        if (qualityInitiator == 0) and (qualityReflector == 0):
            sheet["H" + row] = '=D' + row + '-' + 'F' + row
        else:
            nbError += 1

    print_attribute(sheet, line, "nbErrors", nbError);
    line += 1

    # Draw ToF
    c = LineChart()
    c.height = 10.0
    c.width = 30.0
    column_plot = 8
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=elements)
    c.add_data(data, titles_from_data=True)
    c.series[0].marker.symbol = "circle"
    c.series[0].marker.size = 5
    c.series[0].graphicalProperties.line.noFill = True
    # c.y_axis.scaling.min = -6
    # c.y_axis.scaling.max = 16
    sheet.add_chart(c, "L1")

    # Draw TS charts
    c = LineChart()
    c.height = 10.0
    c.width = 30.0
    column_plot = 4
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=elements)
    c.add_data(data, titles_from_data=True)
    c.series[0].marker.symbol = "circle"
    c.series[0].marker.size = 5
    c.x_axis.title = 'RTT id'
    c.y_axis.title = 'MD ns'
    c.y_axis.majorGridlines = None

    c2 = LineChart()
    c2.height = 10.0
    c2.width = 30.0
    column_plot = 6
    data = Reference(sheet, min_col=column_plot, min_row=1, max_col=column_plot, max_row=elements)
    c2.add_data(data, titles_from_data=True)
    c2.series[0].marker.symbol = "circle"
    c2.series[0].marker.size = 5
    c2.y_axis.axId = 200
    c2.y_axis.title = 'RD ns'

    c.y_axis.crosses = "max"
    c += c2  # Combine charts
    c.title = "MD and RD timestamps"

    sheet.add_chart(c, "L21")


def save_hadm(wb, ranging_data, measure_id_base):
    # Loop on measurement entries
    measure_iter = 0
    for result in ranging_data:
        if not isinstance(result, NestedDict):
            continue

        # A measure (sheet) will be identified by measure_id_base.iteration
        measure_id = measure_id_base + "." + str(measure_iter)

        # Generate main measurement sheet (may return an error in case of failing measurement)
        status = generate_measure_summary(wb, measure_id, result)
        if "OK" not in status:
            measure_iter += 1
            toggle_sheet_tab_group_color()
            continue

        # Generate tof sheet
        generate_measure_tof(wb, measure_id, result)

        # Generate phase-related sheets
        elements_init = len(result.get('mciq.initiator.i', ()))
        elements_refl = len(result.get('mciq.reflector.i', ()))
        if elements_init != 0 or elements_refl != 0:
            try:
                generate_measure_phase(wb, measure_id, result)
                generate_measure_phase_debug(wb, measure_id, result, mode0=False)
                generate_measure_phase_debug(wb, measure_id, result, mode0=True)
            except Exception as e:
                print(e)

        measure_iter += 1
        toggle_sheet_tab_group_color()


def convert_to_excel(**params):

    config = {
        'input_pattern': '',
        'output_directory': None,
        'targets': [],
        'combined': False,
        'iopfile': "",
        'ioptype': None
    }
    for key, value in params.items():
        if key == "silent":
            config['silent'] = value
        elif key == "pattern":
            config['input_pattern'] = value
        elif key == "output":
            config['output_directory'] = value
        elif key == "target":
            config['targets'] = value
        elif key == "combined":
            config['combined'] = value
        else:
            break
    log = logging.getLogger(__name__)
    if config['silent'] == 0:
        log.setLevel(logging.ERROR)
    else:
        log.setLevel(logging.DEBUG)

    log.info('Working on %s' % sys.platform)

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
        f_out = os.path.join(os.path.dirname(path), folder + '_combined_hadm')

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
        time.sleep(1)
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
                if config['ioptype'] is not None:
                    if config['iopfile'] == '':
                        iopfile = Path(f_base + '.' + config['ioptype'])
                    else:
                        iopfile = Path(config['iopfile'])

                    if (config['ioptype'] != None) and (iopfile != ''):
                        log.info("Process IOP file: " + str(iopfile))
                        with open(Path(iopfile), 'r') as f:
                            # Find oposite role to the NXP board
                            if data[0].get('mciq.reflector.i', None) is None and \
                                    data[0].get('tof.reflector.d', None) is None:
                                ioprole = 'refl'
                            else:
                                ioprole = 'init'
                            converter = IOPConverter(name=config['ioptype'])
                            converter.convert(iopfile=f, ioprole=ioprole, data=data[0])
            except FileNotFoundError as e:
                log.error('Failed loading: %s, %s' % (config['iopfile'], e))
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


########################################################################################


if __name__ == '__main__':
    pd.set_option('display.width', 1000)
    pd.set_option('display.max_columns', 500)
    pd.option_context('display.multi_sparse', False)        # display sparse multi-index
    rf = ResultFile()

    rf.load('test.npz')
    rf.save('out.mat')
    rf.save('out.csv')
    rf.save('out.xlsx')

    rf.load('out.mat')
    rf.load('out.csv')

