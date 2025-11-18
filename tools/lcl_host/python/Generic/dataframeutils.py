# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D       # needed for 3d-plotting, removing is not possible, even though the IDE indicates it's not used.

if __name__ == '__main__':
    import sys
    import os
    import logging.config
    import glob
    BaseDir = os.path.dirname(os.path.realpath(__file__))
    sys.path.append(os.path.join(BaseDir, "..", "Instruments"))
    sys.path.append(os.path.join(BaseDir, "..", "Generic"))

    from RangingPlatform_FileUtils import load_ranging_data, create_table

    # load the logging configuration
    logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))

ColorMap = ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 'tab:purple', 'tab:brown', 'tab:pink', 'tab:gray',
            'tab:olive', 'tab:cyan']
MarkerMap = ['.', 'o', 'v', '1', 's', 'p', '*', '+', 'x', 'd', '|', '_']


def dataframe_get_label(info):
    lbl = '%s' % info.get('label')
    if info.get('unit'):
        lbl += ' [%s]' % info.get('unit')
    return lbl


# convert DataFrame to numpy arrays with mean and std-dev
# df = DataFrame or table with all content
# x_field : identifier for X axis (strings) in df. e.g. ['CFO']
# y_field : identifier for Y axis (strings) in df. e.g. ['Attenuation']
# z_field : identifier for Z axis (strings) in df. e.g. ['Distance']
def dataframe_to_ndarray_mean_std(df, x_field, y_field, z_field):
    x_values = np.unique(df.loc[:, x_field])
    y_values = np.unique(df.loc[:, y_field])

    mean_values = np.zeros((len(x_values), len(y_values)))
    std_values = np.zeros((len(x_values), len(y_values)))
    for row, x_val in enumerate(x_values):
        for col, y_val in enumerate(y_values):
            indx = (df.loc[:, x_field] == x_val) & (df.loc[:, y_field] == y_val)        # the brackets are important!!
            if np.all(np.isnan(df.loc[indx, z_field])):
                mean_values[row, col] = np.nan
                std_values[row, col] = np.nan
            else:
                mean_values[row, col] = np.nanmean(df.loc[indx, z_field])
                std_values[row, col] = np.nanstd(df.loc[indx, z_field], ddof=1)
    return x_values, y_values, mean_values, std_values,


# plot one or more data lines (no grouping)
# df = DataFrame or table with all content
# y_info_lst = list of dicts, where each dict should have the following keys:
# - fields : list of identifiers (strings) in df. e.g. ['CFO']
# - unit : string to show unit of line. e.g. 'm'
# - label : string to label graph
# - convert_func : function to convert the corresponding values; optional
# each dict is plotted in a separate subplot
def plot_any_vs_y(df, y_info_lst, **kwargs):
    filepath = kwargs.get('filepath', None)
    fig = plt.figure(figsize=kwargs.get('figsize', (32, 18)))
    no_subplots = len(y_info_lst)
    for n, y_info in enumerate(y_info_lst, 1):
        plt.subplot(no_subplots, 1, n)
        for k in range(len(y_info.get('fields'))):
            CmIndx = np.mod(k, len(ColorMap))
            # MrkIndx = np.mod(k, len(MarkerMap))
            MrkIndx = 0
            y_values = df.loc[:, y_info.get('fields')[k]]
            if y_info.get('convert_func', None):
                y_values = y_info.get('convert_func')(y_values)
            plt.plot(y_values, ColorMap[CmIndx], marker=MarkerMap[MrkIndx],
                     label=y_info.get('fields')[k])
        plt.grid(True)
        plt.legend()
        plt.xlabel('Measurement Number')
        plt.ylabel(dataframe_get_label(y_info))
        plt.title('%s vs Measurement' % (y_info.get('label')))

    if filepath:
        plt.savefig(filepath)
    plt.close()


# create a graph with 4 subplots (showing the mean, zoom-in of the mean, standard deviation and zoom-in of the
# standard deviation.
# df = DataFrame or table with all content
# x_info, y_info, z_info = dicts corresponding to its respective axis where each dict should have the following keys:
# - field : identifier (strings) in df. e.g. ['CFO']
# - unit : string to show unit of line. e.g. 'm'
# - label : string to label graph
# - convert_func : function to convert the corresponding values; optional
# figsize = figure size
# filepath = filepath of figure
def plot_x_vs_y_groupby_z_mean_std(df, x_info, y_info, z_info, **kwargs):
    def subplot(var, label):
        for k in range(var.shape[1]):
            CmIndx = np.mod(k, len(ColorMap))
            # MrkIndx = np.mod(k, len(MarkerMap))
            MrkIndx = 0
            plt.plot(x_values, var[:, k], ColorMap[CmIndx], marker=MarkerMap[MrkIndx],
                     label='%s = %.1f%s' % (z_info.get('label'), y_values[k], z_info.get('unit')))
        plt.grid(True)
        plt.legend()
        plt.xlabel(dataframe_get_label(x_info))
        plt.ylabel(dataframe_get_label(y_info))
        plt.title('%s %s vs %s' % (label, y_info.get('label'), x_info.get('label')))

    filepath = kwargs.get('filepath', None)
    fig = plt.figure(figsize=kwargs.get('figsize', (32, 18)))

    x_values, y_values, mean_values, std_values = dataframe_to_ndarray_mean_std(df, x_info.get('field'),
                                                                                z_info.get('field'),
                                                                                y_info.get('field'))
    if x_info.get('convert_func', None):
        x_values = x_info.get('convert_func')(x_values)
    if y_info.get('convert_func', None):
        y_values = y_info.get('convert_func')(y_values)

    MeanVar_median = np.nanmedian(mean_values)
    StdVar_median = max(0.001, np.nanmedian(std_values))

    plt.subplot(2, 2, 1)
    subplot(mean_values, 'Mean of')

    plt.subplot(2, 2, 2)
    subplot(mean_values, '(Zoom) Mean of')
    plt.ylim(MeanVar_median - 4.0 * StdVar_median, MeanVar_median + 4.0 * StdVar_median)

    plt.subplot(2, 2, 3)
    subplot(std_values, 'Standard deviation of')

    plt.subplot(2, 2, 4)
    subplot(std_values, '(Zoom) Standard deviation of')
    plt.ylim(0.5 * StdVar_median, 1.5 * StdVar_median)

    if filepath:
        plt.savefig(filepath)
    plt.close()


# create a 3d plot of the mean or standard deviation of Z
# df = DataFrame or table with all content
# x_info, y_info, z_info = dicts corresponding to its respective axis where each dict should have the following keys:
# - field : identifier (strings) in df. e.g. ['CFO']
# - unit : string to show unit of line. e.g. 'm'
# - label : string to label graph
# - convert_func : function to convert the corresponding values; optional
# figsize = figure size
# filepath = filepath of figure
# plotmean = plot mean or standard deviation otherwise (boolean)
def plot_x_vs_y_vs_z(df, x_info, y_info, z_info, **kwargs):
    filepath = kwargs.get('filepath', None)
    plotmean = kwargs.get('plotmean', True)
    fig = plt.figure(figsize=kwargs.get('figsize', (16, 9)))

    m_label = 'Mean' if plotmean else 'Standard deviation'
    x_values, y_values, mean_values, std_values = dataframe_to_ndarray_mean_std(df, x_info.get('field'), y_info.get('field'), z_info.get('field'))

    if x_info.get('convert_func', None):
        x_values = x_info.get('convert_func')(x_values)
    if y_info.get('convert_func', None):
        y_values = y_info.get('convert_func')(y_values)

    (xv, yv) = np.meshgrid(x_values, y_values, sparse=False, indexing='ij')
    ax = plt.axes(projection='3d')
    ax.plot_wireframe(xv, yv, mean_values if plotmean else std_values)
    ax.set_xlabel(dataframe_get_label(x_info))
    ax.set_ylabel(dataframe_get_label(y_info))
    ax.set_zlabel('%s of %s [%s]' % (m_label, z_info.get('label'), z_info.get('unit')))
    ax.view_init(29, -110)

    if filepath:
        plt.savefig(filepath)
    plt.close()


if __name__ == '__main__':
    def convert_att(values):
        return -(4+14) - values

    log = logging.getLogger(__name__)
    load_data=True
    if load_data:
        filepath= '../Scripts/output.csv'
        log.info("loading data..")
        data = pd.read_csv(filepath)

        z_info = {'field': 'CFO', 'unit': 'Hz', 'label': 'CFO'}
        x_info = {'field': 'AttenuationCommon', 'unit': 'dBm', 'label': 'RX-power', 'convert_func': convert_att}
        y1_info = {'field': 'Distance', 'unit': 'm', 'label': 'Distance'}
        y2_info = {'field': 'ToF_Distance', 'unit': 'm', 'label': 'ToF_Distance'}
        y3_info = {'field': 'ToF_SuccessRate', 'unit': '%', 'label': 'ToF_SuccessRate'}

        log.info("Plotting mesh..")
        plot_x_vs_y_vs_z(data, x_info, z_info, y1_info, filepath='./mesh_Distance_Mean.png', plotmean=True)
        plot_x_vs_y_vs_z(data, x_info, z_info, y1_info, filepath='./mesh_Distance_Std.png', plotmean=False)
        log.info("Plotting x vs y..")
        plot_x_vs_y_groupby_z_mean_std(data, x_info, y1_info, z_info, filepath='./vs_Distance_MeanStd.png')
        plot_x_vs_y_groupby_z_mean_std(data, x_info, y2_info, z_info, filepath='./vs_ToF_Distance_MeanStd.png')
        plot_x_vs_y_groupby_z_mean_std(data, x_info, y3_info, z_info, filepath='./vs_ToF_SuccessRate_MeanStd.png')

        log.info("Plotting all measurements..")
        y_info_lst = []
        y_info = {'fields':['InitiatorRSSI', 'ReflectorRSSI'], 'unit':'dB', 'label': 'RSSI'}
        y_info_lst.append(y_info)
        y_info = {'fields':['Distance','ToF_Distance'], 'unit':'m', 'label': 'Distance'}
        y_info_lst.append(y_info)
        y_info = {'fields':['ToF_SuccessRate'], 'unit':'%', 'label': 'ToF_SuccessRate'}
        y_info_lst.append(y_info)
        plot_any_vs_y(data, y_info_lst, filepath='./rssi.png')
    if not load_data:
        all_data = []
        # filepattern='./20190417T115814/*.npz'
        filepattern='./20190419T082149/*.npz'
        log.info("loading data..")
        for f in glob.glob(filepattern):
            data = load_ranging_data(f)
            all_data.extend(data)
        log.info("Creating table..")
        key_lst = [
            ['Initiator', 'RSSI'],
            ['Reflector', 'RSSI'],
            ['*Result', 'Distance'],
            ['*Result', 'ToF_Distance'],
            ['*Result', 'ToF_SuccessRate'],
        ]
        create_table('output.csv', all_data, key_lst=key_lst)
    log.info("All done")


