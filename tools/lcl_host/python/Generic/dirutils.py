# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import contextlib
import os
import shutil
import tempfile
import errno


# copied from: https://stackoverflow.com/questions/3223604/how-to-create-a-temporary-directory-and-get-the-path-file-name-in-python
@contextlib.contextmanager
def cd(newdir, cleanup=lambda: True):
    prevdir = os.getcwd()
    os.chdir(os.path.expanduser(newdir))
    try:
        yield
    finally:
        os.chdir(prevdir)
        cleanup()


# copied from: https://stackoverflow.com/questions/3223604/how-to-create-a-temporary-directory-and-get-the-path-file-name-in-python
@contextlib.contextmanager
def tempdir():
    dirpath = tempfile.mkdtemp()

    def cleanup():
        shutil.rmtree(dirpath)

    with cd(dirpath, cleanup):
        yield dirpath


def ensure_dir(file_path):
    # TODO: This function assumes that, if the file_path is not terminated with a '/', the last part after the last '/' refers to a filename. This part is truncate in the dirname-function.
    # This gives sometimes undesired behavior. Changing the behavior might have impact on behavior of older scripts.
    dirname = os.path.dirname(file_path)
    if ['', '.'].count(dirname) != 0:
        return
    try:
        os.makedirs(dirname)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise Exception("ensure_dir could not create directory")
