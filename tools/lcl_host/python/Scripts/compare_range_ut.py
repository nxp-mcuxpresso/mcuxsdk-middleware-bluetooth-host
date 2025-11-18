# Copyright 2022-2023 NXP
# 
# SPDX-License-Identifier: BSD-3-Clause
import csv
import os
import sys
import shutil
import argparse
import pandas as pd
import numpy as np
import glob
import xlsxwriter

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))
line_colors = ('blue', 'red', 'green', 'orange', 'purple', 'brown', 'pink', 'cyan', 'yellow', 'magenta')


def ut_results_parse(test_folder):
    """
    Reads in and parses the csv files for all tests in a single record_range_ut.py regression test run.

    This function reads in all of the the .csv files in a test as dataframes for

    Args:
        test_folder: The folder of csv files to read in.

    Returns:
        testdict A dict with entries for each test (csv file) that was read in.

    Raises:
        None.
    """
    # Read in each test file in test_folder (as pandas df), assuming name format of *_#.csv (where # is the test #)
    # Store results as a dict by test
    testdict = {}
    testfiles1 = glob.glob(test_folder + '/*_[0-9].csv')  # Single digit test numbers
    testfiles2 = glob.glob(test_folder + '/*_[1-9][0-9].csv')  # 2 digit test numbers
    testfiles3 = glob.glob(test_folder + '/*_[1-9][0-9][0-9].csv')  # 3 digit test numbers
    testfiles = testfiles1 + testfiles2 + testfiles3
    testdict["max_num_ap"] = 1  # Keep track of maximum number of antenna paths across all tests in this
    for test in testfiles:
        [testpath, testfilename] = os.path.split(test)  # separate the path and filename
        [tn, ext] = testfilename.split(".")  # Get rid of .csv extension
        dummy = tn.split("_")  # Test number is after last underscore, first part of name may have underscores
        testnum = dummy.pop(-1)  # Removes last element from the list representing the name & number
        # Open the csv file as dataframe
        testdf = pd.read_csv(test)
        #            testdf['name'] = tn
        testdict[testnum] = testdf
        if testdf["meta.has_mciq"][0]:  # Only update num AP count if flag for MCIQ is true
            testdict["max_num_ap"] = max(testdict["max_num_ap"], testdf['mciq.cfg.n_ap'][0])
        testdict['name'] = "_".join(dummy)  # Rejoins the name portion of the list (if it was split before)
    return testdict


def extract_df_one_run(test):
    """
    Extracts the IQ and TOF data from the test results for a single record_range_ut.py regression test run.

    This function processes all of the the .csv files in a test. It extracts IQ and time-of-flight data from each test
    and creates two dicts containing data frames for those results. It expands any results that have multiple antenna
    paths and names the columns by the data type, the folder name, and the antenna path number. The keys to the dicts
    are the extracted test numbers.

    Args:
        test: The dict representing the test results that should be parsed.

    Returns:
        [rmse_dict,tof_dict, test_meta] the results of processing all csv files and extracting RMSE and TOF data. The
        test_meta return is a dict of metadata indexed by test numbers as the key.

    Raises:
        None.
    """
    # Process each test that exists in the results and extract the MCIQ and TOF data
    rmse_dict = {}
    tof_dict = {}
    test_meta = {}
    keys = list(test.keys())
    keys.sort()
    namestr = test['name']
    for key in keys:
        if (key == 'name') or (key == 'max_num_ap'):
            continue  # Skip any attempt to process the name or max_num_ap keys
        df = test[key]
        # Extract MCIQ results if they exist
        num_ap = 0  # Flag that there is no AP count available
        if (df['meta.has_mciq'][0]) and ('mciq.result.slope_rmse' in df.columns):
            # IQ statistics are stored in the original CSV as strings representing lists so they import as strings
            # The length of the list corresponds to the number of antenna paths
            s = df['mciq.result.slope_rmse']
            # Convert from string to list, replacing any NaN values (handles minor failures in large number of reps)
            for idx, val in s.items():
                if val is np.nan:
                    s.loc[idx] = s.loc[idx - 1]  # Handle a single NaN value by replacing with neighbor value
            s = s.apply(eval)
            # Make a dataframe from the series and store in dict
            num_ap = df['mciq.cfg.n_ap'][0]
            ap_nums = list((range(num_ap)))
            ap_nums = [str(x) for x in ap_nums]  # make the list of integers into a list of strings
            colnames = [namestr + '_AP' + ap for ap in ap_nums]
            # colnames.insert(0, 'Index')
            rmse_dict[key] = pd.DataFrame(s.tolist(), columns=colnames)
        # Process TOF_ results if they exist
        if (df['meta.has_tof'][0]) and ('tof.result.std' in df.columns):
            s = df['tof.result.std']
            tof_dict[key] = pd.DataFrame(s.tolist(), columns=[namestr])
        # Assemble a meta description string for this test
        if num_ap == 0:
            ap_str = ""
        else:
            ap_str = "N_AP=" + str(num_ap)
        if df["hadm.cfg.rphy"][0] == 0:
            rate_str = "1M"
        else:
            rate_str = "2M"
        test_meta[key] = '/'.join(["T_FC=" + str(df["hadm.cfg.fcs"][0]), "T_IP1=" + str(df["hadm.cfg.ip1"][0]),
                                   "T_IP2=" + str(df["hadm.cfg.ip2"][0]), "T_PM=" + str(df["hadm.cfg.tpm"][0]),
                                   rate_str, ap_str])
    return [rmse_dict, tof_dict, test_meta]


def compare_tests(testlist, outputpath, avg_len):
    """
    Groups multiple (one or more) post-processed sets of results for record_range_ut.py regression test run(s).

    This function processes the .csv files in a test.

    Args:
        testlist: A list of UT_results_parse objects that should be parsed.
        outputpath: The path for storing output of the processing.
        avg_len: The length of averaging window to be used. A negative, 0 or 1 will result in no averaging.

    Returns:
        None.

    Raises:
        None.
    """
    # Store extracted data in dicts of df, one df for each test number. Test number is the dict key.
    rmse_results = {}
    tof_results = {}
    meta_data_results = {}
    rmse_overview = {}
    tof_overview = {}
    #  Iterate through all tests extracting IQ/TOF data
    for test in testlist:
        [temp_rmse_results, temp_tof_results, temp_meta_data_results] = extract_df_one_run(test)
        # Merge results for RMSE
        for key in temp_rmse_results.keys():
            if key in rmse_results.keys():
                rmse_results[key] = pd.concat([rmse_results[key], temp_rmse_results[key]], axis=1)  # Concat the two dfs
            else:
                rmse_results[key] = temp_rmse_results[key]  # key didn't exist in results dict, add the df
        # Merge results for TOF
        for key in temp_tof_results.keys():
            if key in tof_results.keys():
                tof_results[key] = pd.concat([tof_results[key], temp_tof_results[key]], axis=1)  # Concat the two dfs
            else:
                tof_results[key] = temp_tof_results[key]  # key didn't exist in results dict, add the df
        # Merge meta data results
        for key in temp_meta_data_results.keys():
            if key in meta_data_results.keys():
                if meta_data_results[key] != temp_meta_data_results[key]:  # Check that both meta data match!
                    print("Error in meta data for test number " + str(key))
            else:
                meta_data_results[key] = temp_meta_data_results[key]  # key didn't exist in results dict, add the df
    # After all data has been extracted, output data and plots to XLS sheets
    # Filter RMSE data if needed (moving average to smooth it out)
    if avg_len < 1:
        avg_len = 1
    outfile = outputpath + '/Compare_RMSE.xlsx'
    writer1 = pd.ExcelWriter(outfile, engine='xlsxwriter')
    workbook = writer1.book
    # TODO: Add filtering/windowing
    for key in rmse_results.keys():
        # Calculate overview statistics and store by test number
        rmse_overview[key] = rmse_results[key].describe().loc['mean']
        # Output each test to a worksheet with a chart
        shtnm = 'Test_' + str(key)
        rmse_results[key].to_excel(writer1, sheet_name=shtnm, startrow=1, header=False, index=False)
        column_settings = [{'header': column} for column in rmse_results[key].columns]
        (max_row, max_col) = rmse_results[key].shape
        worksheet = writer1.sheets[shtnm]
        worksheet.add_table(0, 0, max_row, max_col - 1, {'columns': column_settings})
        chart = workbook.add_chart({'type': 'line'})
        for i in range(max_col):
            if avg_len > 1:  # Use trendline to perform moving average, replacing normal line
                series_dict = {'values': [shtnm, 1, i, max_row, i],
                               'name': [shtnm, 0, i],
                               'line': {'none': True},  # Turn off normal line in favor of trendline
                               'marker': {'type': 'none'},  # Turn off markers
                               'trendline': {  # Enable trendline
                                   'type': 'moving_average',
                                   'line': {
                                       'color': line_colors[i % 10],
                                       'width': 2,
                                   },
                                   'period': avg_len}  # If avg_len == 1 then trendline is back to original line
                               }
            else:
                series_dict = {'values': [shtnm, 1, i, max_row, i],
                               'name': [shtnm, 0, i],
                               }
            chart.add_series(series_dict)
        chart.set_legend({'position': 'bottom'})
        chart.set_size({'x_scale': 2, 'y_scale': 2})
        chart.set_title({'name': 'Phase RMSE (deg) for Test ' + key + ' (' + meta_data_results[key] + ')'})
        chart.set_y_axis({'name': 'Phase RMSE (deg)'})
        worksheet.insert_chart(1, max_col + 1, chart)
    # Process overall statistics and create overview sheet
    rmse_means = pd.DataFrame(rmse_overview)
    rmse_keys = rmse_means.keys()
    rmse_meta_data = {}
    for key in meta_data_results:
        if key in rmse_keys:
            rmse_meta_data[key] = meta_data_results[key]  # Only keep meta data for tests that include RMSE results
    meta_frame = pd.DataFrame.from_dict([rmse_meta_data], orient='columns')
    # Rename columns in dataframe. Only useful when metadata is more meaningful than test name.
    # rmse_means = rmse_means.rename(meta_data_results, axis="columns")
    rmse_means.to_excel(writer1, sheet_name='Overview', startrow=0)
    (max_row, max_col) = rmse_means.shape
    meta_frame.to_excel(writer1, sheet_name='Overview', startrow=max_row+1, startcol=1, index=False)
    worksheet = writer1.sheets['Overview']
    chart = workbook.add_chart({'type': 'line'})
    for i in range(max_row):
        chart.add_series({'values': ['Overview', i+1, 1, i+1, max_col],
                          'categories': ['Overview', 0, 1, 0, max_col],
                          'name': ['Overview', i+1, 0],
                          'line': {'none': True},  # Turn off normal line to just leave markers
                          'marker': {'type': 'automatic'}
                          })
    chart.set_legend({'position': 'bottom'})
    chart.set_size({'x_scale': 2, 'y_scale': 2})
    chart.set_title({'name': 'Mean Phase RMSE (deg) by Test'})
    chart.set_y_axis({'name': 'Mean Phase RMSE (deg)'})
    worksheet.insert_chart(max_row+3, 1, chart)
    writer1.sheets['Overview'].activate()
    writer1.save()
    # TOF Excel output
    outfile = outputpath + '/Compare_TOF.xlsx'
    writer2 = pd.ExcelWriter(outfile, engine='xlsxwriter')
    workbook = writer2.book
    for key in tof_results.keys():
        # Calculate overview statistics and store by test number
        tof_overview[key] = tof_results[key].describe()
        # Output each test to a worksheet with a chart
        shtnm = 'Test_' + str(key)
        tof_results[key].to_excel(writer2, sheet_name=shtnm, startrow=1, header=False, index=False)
        column_settings = [{'header': column} for column in tof_results[key].columns]
        (max_row, max_col) = tof_results[key].shape
        worksheet = writer2.sheets[shtnm]
        worksheet.add_table(0, 0, max_row, max_col - 1, {'columns': column_settings})
        chart = workbook.add_chart({'type': 'line'})
        for i in range(max_col):
            if avg_len > 1:  # Use trendline to perform moving average, replacing normal line when averaging
                series_dict = {'values': [shtnm, 1, i, max_row, i],
                               'name': [shtnm, 0, i],
                               'line': {'none': True},  # Turn off normal line in favor of trendline
                               'marker': {'type': 'none'},  # Turn off markers
                               'trendline': {  # Enable trendline
                                   'type': 'moving_average',
                                   'line': {
                                       'color': line_colors[i % 10],
                                       'width': 2,
                                   },
                                   'period': avg_len}  # If avg_len == 1 then trendline is back to original line
                               }
            else:
                series_dict = {'values': [shtnm, 1, i, max_row, i],
                               'name': [shtnm, 0, i],
                               }
            chart.add_series(series_dict)
        chart.set_legend({'position': 'bottom'})
        chart.set_size({'x_scale': 2, 'y_scale': 2})
        chart.set_y_axis({'name': 'Std. Dev. (ns)'})
        chart.set_title({'name': 'TOF Std Dev. (ns) for Test ' + key + ' (' + meta_data_results[key] + ')'})
        worksheet.insert_chart(1, max_col + 1, chart)
    writer2.save()


def main(argv):
    parser = argparse.ArgumentParser(description='Compares ranging measurements between different unit test runs.')
    parser.add_argument('-o', '--output', action='store', type=str, help='output folder name within "records" dir')
    parser.add_argument('-a', '--avg_len', action='store', nargs='?', type=int, const=10, default=10,
                        help='averaging window length')
    parser.add_argument('-i', '--input', action='append', type=str, nargs='+',
                        help='the folder name within "records" directory for comparison may be used multiple times \
                             to compare several runs')

    args = parser.parse_args()
    # Create output file
    if args.output is not None:
        if os.path.isabs(args.output):
            # Absolute path is provided
            filepath = args.output
        else:
            # Relative path is provided: relates to records folder
            filepath = 'records/' + args.output
    else:
        filepath = 'records/defaultCompare'
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
    filenamebase = filepath + "/"

    # Read in all files to objects with all the data
    if args.input is not None:
        testlist = []
        for filename in args.input:
            filepath = 'records/' + filename[0]  # args.input has a []list for each input file name
            results = ut_results_parse(filepath)
            testlist.append(results)
        compare_tests(testlist, filenamebase, args.avg_len)


if __name__ == '__main__':
    main(sys.argv)
