#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import logging.config
import os

BaseDir = os.path.dirname(os.path.realpath(__file__))
logging.config.fileConfig(os.path.join(BaseDir, "..", 'logging.ini'))  # load the logging configuration

import matlab.engine
MLengineFound = True

#try:
#    import matlab.engine
#    MLengineFound = True
#except:
#    MLengineFound = False

class MatlabEngine(object):
        MLengine = None

        def __init__(self, asynchronous=False, MatlabSession=None):
            # create logger
            log = logging.getLogger()

            # To connect to current running Matlab session. This enables the use of breakpoints in Matlab as well.
            # run 'matlab.engine.shareEngine' in matlab command-prompt first
            # to find specific matlab-session, run from python: matlab.engine.find_matlab()

            eng = None
            if MLengineFound:
                if MatlabSession is not None:
                    assert isinstance(MatlabSession, str)
                    eng = matlab.engine.connect_matlab(MatlabSession)
                else:
                    temp = matlab.engine.find_matlab()
                    if len(temp) < 1:
                        log.debug('No shared Matlab-session found, starting Matlab instead.')
                        eng = matlab.engine.start_matlab(background=asynchronous)
                    else:
                        log.debug('Shared Matlab-session found with id %s.' % temp[0])
                        eng = matlab.engine.connect_matlab(temp[0])

            self.MLengine = eng
            if not self.self_test():
                log.warning('Matlab engine is not connected; self-test failed.')
                self.MLengine = None
            else:
                log.info('Successfully connected to Matlab-engine')

        def self_test(self):
            if self.MLengine is None:
                return False
            else:
                return self.MLengine.isprime(37)

        def reset(self):
            if self.MLengine:
                self.MLengine.eval('clc; clear all; close all;', nargout=0)

        def __repr__(self):
            if self.MLengine is None:
                S = 'None\n'
            else:
                ver = self.MLengine.version(nargout=1)
                S = 'Version: %s\n' % ver
            return S

        __str__ = __repr__

        def __del__(self):
            if self.MLengine is not None:
                self.MLengine.exit()


if __name__ == '__main__':
    MLE = MatlabEngine()
    print(MLE)

    MLE.MLengine.add(1.0, 1.0)
