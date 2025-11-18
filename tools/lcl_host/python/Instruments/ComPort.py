# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

import serial
import time
import serial.tools.list_ports as st

# IMPORTANT: changing the timeout when characters are received results in weird behavior
class ComPort(object):
    def __init__(self, debug=0):
        self.s = None
        self.timeout = 0.2
        self.vid = None
        self.pid = None
        self.port = None
        self.debug = debug

    def connected(self):
        if self.s is not None and self.s.isOpen():
            return True
        return False

    def flush(self):
        if self.s is None:
            return
        if self.s.out_waiting:
            time.sleep(0.010)
        self.s.flush()
        self.s.reset_input_buffer()
        # self.s.reset_output_buffer()

    def close(self):
        if self.connected():
            self.s.close()

    def open(self, port, **kargs):
        self.s = None
        self.vid = None
        self.pid = None
        self.port = None
        try:
            self.s = serial.Serial(port=port,
                                   baudrate=kargs.get('baudrate', 115200),
                                   bytesize=serial.EIGHTBITS,
                                   parity=serial.PARITY_NONE,
                                   stopbits=serial.STOPBITS_ONE,
                                   xonxoff=False,
                                   rtscts=False,
                                   dsrdtr=False,
                                   timeout=kargs.get('timeout', self.timeout))
            for cp in st.comports():
                if port == cp.device:
                    self.vid = cp.vid
                    self.pid = cp.pid
                    self.port = port
                    break
        except:
            pass
        return self.s

    def write(self, txt):
        if not self.connected():
            return
        if self.debug == 2:
            print("> ", self.s.port, txt)
        self.s.write(txt.encode())

    def read(self, expected=None, timeout=None):
        if not self.connected():
            return None
        if timeout is None:
            timeout = self.timeout
        self.s.apply_settings({'timeout': timeout})
        if expected is None:
            txt = self.s.read(2048)
        else:
            txt = self.s.read_until(expected.encode())
        try:
            txt2 = txt.decode()
            txt = txt2
        except UnicodeDecodeError:
            print('Decoding went wrong because some character(s) cannot be decoded. The byte-string is', txt)
            txt = ''
        if txt:
            txt = txt.replace('\r', '')
        if self.debug == 2:
            print("< ", self.s.port, txt)
        return txt

    def write_with_response(self, msg, expected=None, timeout=None, flush=True):
        if not self.connected():
            return None
        if flush:
            self.flush()
        if timeout is None:
            timeout = self.timeout
        self.s.apply_settings({'timeout': timeout})
        time.sleep(0.02)
        self.write(msg)
        time.sleep(0.02)
        return self.read(expected=expected, timeout=timeout)

    def read_until(self, expected='\n', timeout=0.2):
        return self.read(expected=expected, timeout=timeout)

    def readline(self):
        return self.read(expected='\n')

    def apply_settings(self, **kargs):
        if self.s is None:
            return
        self.s.apply_settings(kargs)

    def get_settings(self):
        if self.s is None:
            return {}
        return self.s.get_settings()

    # id_lst is a list of vid/pid pairs (tuple)
    # serial_lst is a list of serial numbers (strings), this is an optional filter
    @staticmethod
    def get_serial_ports(id_lst=[], serial_lst=[]):
        ports = []
        for cp in st.comports():
            if cp.pid is None or cp.vid is None:
                continue
            found = False
            for vid, pid in id_lst:
                if cp.vid == vid and cp.pid == pid:
                    found = True
                    break
            if (len(serial_lst) == 0 or cp.serial_number in serial_lst) and found:
                ports.append(str(cp.device))
        if len(ports) == 0:
            return ports
        ports.sort()
        return ports
