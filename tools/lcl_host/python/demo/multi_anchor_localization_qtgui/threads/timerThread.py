#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

from PyQt5 import QtCore
from PyQt5.QtCore import *

class timerThread(QtCore.QThread):

    timeout = QtCore.pyqtSignal(object)

    '''
        setup thread
        
    '''
    def __init__(self, timeout_ms = 200):
        QtCore.QThread.__init__(self)
      
        self.requestStop = False
        self.sleeptime = timeout_ms

    '''
        start thread
    '''

    def run(self):
        self.setPriority(QThread.LowPriority)
        self.requestStop = False
        while not self.requestStop:
            self.msleep(self.sleeptime)
            self.timeout.emit(True)
        self.timeout.emit(False)
        # thread stopped

    '''
        stop thread in a controlled manner
    '''

    def stop(self):
        self.requestStop = True
