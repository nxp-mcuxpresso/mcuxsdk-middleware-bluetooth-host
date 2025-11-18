'''
-------------------------------------------------------------------------
-- Project      : Narrow-band ranging
-- Content      : 
-- Filename     : logConfig.py
-- Creation date: 09-May-2019 
-- Version      : GIT
-- Commiter     : boerp
-------------------------------------------------------------------------
--  CONFIDENTIAL and PROPRIETARY
--  COPYRIGHT (c) Stichting IMEC Nederland, 2019
--
--  All rights are reserved. Reproduction in whole or in part is
--  prohibited without the written consent of the copyright owner
'''

import logging

logConfig = {
    'version': 1,
    'disable_existing_loggers': True,
    'formatters': {
        'standard': {'format': '%(asctime)s [%(levelname)s] %(name)s: %(message)s', 'datefmt': '%Y-%m-%d %H:%M:%S'},
    },
    'handlers': {
        'default': {
            'level': 'INFO',
            'formatter': 'standard',
            'class': 'logging.StreamHandler',
            'stream': 'ext://sys.stdout',  # Default is stderr
        },
        'file': {
            'level': 'DEBUG',
            'formatter': 'standard',
            'filename': 'logfile.txt',
            'class': 'logging.FileHandler',
        },
        'memory': {'class': 'logging.handlers.MemoryHandler',
                   'capacity': 4096,
                   'flushLevel': logging.WARNING,
                   'target': 'file',
                   },
    },
    'loggers': {
        '': {  # root logger
            'handlers': ['default', 'memory'],
            'level': 'INFO',
            'propagate': True
        },
    }
}