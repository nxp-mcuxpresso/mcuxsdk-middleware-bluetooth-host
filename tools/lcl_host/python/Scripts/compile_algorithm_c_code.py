#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

import platform
import struct
from subprocess import call, check_output
import subprocess
import os
from shutil import copyfile

import logging.config
logging.config.fileConfig(os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", 'logging.ini'))


def call_shell(cmdline, showconsole=False):
    if showconsole:
        status = call(cmdline, shell=True)
        assert status == 0, 'FATAL: Running command %s failed.' % cmdline
    else:
        try:
            check_output(cmdline, shell=True)
        except subprocess.CalledProcessError:
            assert False, 'FATAL: Running command %s failed.' % cmdline


log = logging.getLogger(__name__)
(OsNrBits, _) = platform.architecture()         # determine number of bits of the OS
OsType = platform.system()                      # determine os-type (Linux or Windows)
PythonNrBits = struct.calcsize("P") * 8         # determine number of bits of current python executable
log.info('This system is a %s machine running a %dbit Python' % (OsNrBits, PythonNrBits))

abspath = os.path.abspath(__file__)
scriptpath = os.path.dirname(abspath)  # Directory of THIS file
buildpath = os.path.join(scriptpath, '..', '..', 'software', 'algo', 'ranging', 'apps', 'libpython','gcc')
ranging_py_path = os.path.join(scriptpath, '..', '..', 'software', 'algo', 'ranging', 'apps', 'libpython', 'python', 'ranging.py')

if OsType == 'Linux':
    ranging_so_path = os.path.join(buildpath, 'bin')
    
    cmdline1 = f'cd {buildpath}; make clean; make -j6;'
    cmdline2 = f'cd {buildpath}; make clean'
    
    log.info('Copy ranging.py to /python/Instruments-folder')
    copyfile(ranging_py_path, os.path.join(scriptpath, "..", "Instruments", "ranging.py"))
    log.info('Compiling C-code ...')
    call_shell(cmdline1, showconsole=True)    
    log.info('... done')
    log.info('Copy _ranging.so to /python/Instruments-folder')
    copyfile(os.path.join(ranging_so_path, '_ranging.so'), os.path.join(scriptpath, "..", "Instruments", "_ranging.so"))
    log.info('Copy _ranging.version to /python/Instruments-folder')
    copyfile(os.path.join(ranging_so_path, 'appversion.txt'), os.path.join(scriptpath, "..", "Instruments", "_ranging.version"))
    log.info('Copy _ranging.buildinfo to /python/Instruments-folder')
    copyfile(os.path.join(ranging_so_path, 'build.info'), os.path.join(scriptpath, "..", "Instruments", "_ranging.buildinfo"))
    log.info('Clean build directory')
    call_shell(cmdline2, showconsole=False)
elif OsType == 'Windows':
    if PythonNrBits == 32:
        log.info('It is possible to compile the algorithm for a 32bit-python, but it is currently not foreseen')
    elif PythonNrBits == 64:
        # The files are build with Jenkins on the ICT0122346.
        # After each build, the files are copied (by Jenkins) to \\unixnl\nrbrpoc\projectdata\build_artifacts\default.
        ranging_pyd_path = '\\\\unix\\nrbrpoc\\projectdata\\build_artifacts\\default\*.pyd'
        assert os.path.exists(os.path.dirname(ranging_pyd_path)), 'The directory %s does not exist' % os.path.dirname(ranging_pyd_path)

        cmdline1 = 'copy /y "%s" "%s"' % (ranging_py_path, os.path.join(scriptpath, '..', 'Instruments'))   # '/y' means overwrite
        cmdline2 = 'copy /y "%s" "%s"' % (ranging_pyd_path, os.path.join(scriptpath, '..', 'Instruments'))  # '/y' means overwrite

        log.info('Copy ranging.py to /python/Instruments-folder')
        call_shell(cmdline1, showconsole=False)
        log.info('Copy _ranging.pyd to /python/Instruments-folder')
        call_shell(cmdline2, showconsole=False)
    else:
        raise Exception('Currently only 32 and 64bit Python is supported under Windows')


