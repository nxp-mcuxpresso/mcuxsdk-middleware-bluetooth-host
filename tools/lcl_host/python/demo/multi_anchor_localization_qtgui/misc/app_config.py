#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

import configparser
import os
import sys

def read(filename = None):
    if filename is None:
        filename = os.path.splitext(os.path.realpath(sys.argv[0]))[0] + '.ini'
    Config = configparser.ConfigParser()
    Config.read(filename)

    # each value may be one of the following types:
    # - list (comma seperated value)
    # - boolean
    # - string
    # - integers
    # - floats
    app_config = {
        'general': {
            'max_values': 40,
            'activity_minimum': 0.9,
            'activity_depth': 2,
        },
        'csvfile': {
            'enable': False,
            'postfix': '.csv'
        },
        'ranging': {
            'board': 'kw36',
            'serial_port': 'COM1',
            'init_brd': 1,
            'timeout': 2.0,
            'measurement_mode': 1,
            'antinit': 1,
            'antrefl': 1,
            'autostart': False,
        },
        'logfile': {
            'verbose': True,
            'postfix': '.txt',
        },
        'location_estimator': {
            'type': 'GMM',
            'filename': 'config_gmm.json',
        },
        '1D_filter': {
            'filter': 'jump',
            'depth': 40,
            'offset': -1.25,
            'bounds_lower': 0.0,
            'bounds_upper': 20.0,
            'verbose': True,
            'fmt': '%.2fm',
            'label': 'Distance',
            'flt_hysteresis': 2.0,
            'flt_window': 5,
            'flt_warnings': True,
            'flt_alpha': 1.2,
            'flt_d_std': 1.0,
            'flt_a_std': 1.0,
        },
        'plot': {
            'match_scaling': True,
            'draw_car_enabled': False,
            'minor_ticks' : False,
            'pt_interpol' : 5
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
    return app_config
