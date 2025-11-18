'''
-------------------------------------------------------------------------
-- Project   : Narrow-band ranging
-- Content   : This script will try to load an ini file and store it in a dictionary
--             which can be used in the program
-- Commiter  : $Author: govers43 $
-------------------------------------------------------------------------
--  CONFIDENTIAL and PROPRIETARY
--  COPYRIGHT (c) Stichting IMEC Nederland, 2014
--
--  All rights are reserved. Reproduction in whole or in part is
--  prohibited without the written consent of the copyright owner
'''

import configparser
import os
import sys

def read(filename = None):
    if filename is None:
        filename = os.path.splitext(os.path.realpath(sys.argv[0]))[0] + '.ini'
    Config = configparser.ConfigParser()
    Config.read(filename)

    # each value may be one of the following types:
    # - boolean
    # - string
    # - list (comma seperate strings)
    # - integers
    # - floats
    app_config = {
        'general': {
            'board' : 'kw36',
            'max_values': 40,
            'max_errors': 0,
            'activity_depth': 2,
            'sw_version_list' : []
        },
        'logfile': {
            'verbose': True,
            'postfix': '.txt',
        },
        'csvfile': {
            'enable': False,
            'postfix': '.csv'
        },
        'rawfile': {
            'enable': False,
            'postfix': '.txt'
        },
        'defaults': {
            'serial_port': 'COM1',
            'remote_boardnr': 1,
            'measurement_mode': 1,
            'percentage_carriers': 75,
            'ake_enabled': True
        },
        'post_processing': {
            'distance_offset': 2.2,
            'jump_threshold': 2.0,
            'distance_max': 20.0,
            'circle_of_trust': 2.0,
            'circle_of_trust_tolerance': 10,
            'angle_min': -60,
            'angle_max': 60,
            'angle_jump_threshold': 24,
            'aoa_offset': 0
        },
        'plot': {
            'raw': False,
        }
    }

    for sect in Config.sections():
        if not sect in app_config:
            continue
        for name, value in Config.items(sect):
            if name in app_config[sect]:
                vl = app_config[sect][name]
                try:
                    if isinstance(app_config[sect][name], str):
                        vl = Config.get(sect, name)
                    elif isinstance(app_config[sect][name], bool):
                        vl = Config.getboolean(sect, name)
                    elif isinstance(app_config[sect][name], int):
                        vl = Config.getint(sect, name)
                    elif isinstance(app_config[sect][name], float):
                        vl = Config.getfloat(sect, name)
                    elif isinstance(app_config[sect][name], list):
                        s = Config.get(sect, name)
                        vl = s.translate('\t').split(', ')
                except:
                    pass
                app_config[sect][name] = vl
                #print 'Loaded configuration item - section: %s, name: %s, value: %s, type: %s' % (sect, name, value, str(type(vl)))
    return app_config
