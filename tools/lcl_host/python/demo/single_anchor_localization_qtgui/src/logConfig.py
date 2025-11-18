#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

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