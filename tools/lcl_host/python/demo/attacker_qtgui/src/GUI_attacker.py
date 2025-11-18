"""
GUI for a RF animation test.
Extract Python file from *.ui using:
   python -m PyQt5.uic.pyuic -x [FILENAME].ui -o [FILENAME].py

Extract Python file from *.qrc
    pyrcc5 [FILENAME].qrc -o [FILENAME]_rc.py
"""

# dictionary where each key represents the GUI version and the value a list of supported
# board sw/fw versions (can be extended using ini file (section: general, key: sw_version_list)
SwVersion = {
    'kw36' : {
        'v1.1.0' : ['v0.9.56', 'v0.9.62', 'v0.1.7', 'v0.9.67']
    }
}

gui_version = 'v2.0.0'

import sys
import os

# Include directories for ranging algorithm, images, etc
curdir = os.path.dirname(os.path.realpath(__file__))        # Get directory of current python file
sys.path.append(os.path.join(curdir, "..", "..", "common"))
sys.path.append(os.path.join(curdir, "..", "..", "..", "Instruments"))
sys.path.append(os.path.join(curdir, "..", "..", "..", "Generic"))
del curdir

import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *
from QLed import QLed

import pyqtgraph as pg
import logging
import logging.config
from datetime import datetime

import misc.app_config as ac
from valueArray import valueArray
from threads.rangingThread import rangingThread
from dirutils import ensure_dir

from Ui_localization_fullHD import Ui_MainWindow

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

app_config = ac.read()  # Read ini-file

# Create logfile directory
filepaths = {'logfile': None, 'csvfile': None, 'rawfile': None}
timestring = datetime.now().replace(microsecond=0).strftime('%Y%m%d_%H%M%S')

curdir = os.path.dirname(os.path.realpath(__file__))  # Get directory of current python file
basepath = os.path.join(curdir, '..', 'logfiles', timestring)

filepaths['logfile'] = os.path.join(basepath, "applog" + app_config['logfile']['postfix'])
if app_config['csvfile']['enable']:
    filepaths['csvfile'] = os.path.join(basepath, "distances" + app_config['csvfile']['postfix'])
if app_config['rawfile']['enable']:
    filepaths['rawfile'] = os.path.join(basepath, "output" + app_config['rawfile']['postfix'])
ensure_dir(filepaths['logfile'])

from logConfig import logConfig
logConfig['handlers']['file']['filename'] = filepaths['logfile']
logging.config.dictConfig(logConfig)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        QtWidgets.QWidget.__init__(self, parent)
        self.app_config = ac.read()     # Read ini-file

        self.ranging = {
            'activity': 'inactive',
            'threads': [],
            'csv': None
        }
        self.ranging_data = None

        try:
            from PedestalLocalization import PedestalLocalization
            self.pedestal = PedestalLocalization(reset=True)
        except:
            log.exception('Unable to connect to the pedestal - it will not be used')
            from PedestalLocalizationGeneric import PedestalLocalizationGeneric
            self.pedestal = None

        self.measuNo = 0
        
        # GUI stuff goes here
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # GUI align to the right numeric values and start with 0.0
        self.ui.likeliness_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % np.NaN)
        self.ui.RSSI_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % np.NaN)
        self.ui.ToF_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % np.NaN)
        self.ui.vel_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % np.NaN)

        # Create a LED for connection
        self.conn_led=QLed(self, onColour=QLed.Red, shape=QLed.Circle)
        self.conn_led.value=False
        self.ui.connection_led.addWidget(self.conn_led)

        # Create a LED for security
        self.authen_led = QLed(self, onColour=QLed.Green, shape=QLed.Circle)
        self.authen_led.value = False
        self.ui.authentication_led.addWidget(self.authen_led)

        # Create a LED for authorization
        self.circle_led = QLed(self, onColour=QLed.Green, shape=QLed.Circle)
        self.circle_led.value = False
        self.ui.circleoftrust_led.addWidget(self.circle_led)

        try:
            sshFile="style/style.css"
            with open(sshFile,"r") as fh:
                x = fh.read()
            self.ui.centralwidget.setStyleSheet(x)
        except:
            pass

        self.x_val = np.array([])
        self.x_val_TOF = np.array([])

        # Defining Exit buttons actions
        self.exit_action = QtGui.QAction('Exit', self)
        self.ui.btnExit_jade.clicked.connect(self.close_app)

        # Defining Start/Test buttons
        self.ui.btnStart_jade.clicked.connect(self.start_button_jade_click)


    def enable_action_buttons(self, enabled):
        self.ui.btnStart_jade.setEnabled(enabled)

    def get_sw_version(self):
        lst = []
        brd = self.app_config['general']['board']
        if brd not in list(SwVersion.keys()):
            return lst
        if len(self.app_config['general']['sw_version_list']) > 0:
            lst.extend(self.app_config['general']['sw_version_list'])
        if gui_version in list(SwVersion[brd].keys()):
            lst.extend(SwVersion[brd][gui_version])
        return lst

    def retrievedata_finished(self):
        self.enable_action_buttons(True)

    # ------------------------------
    #   control start and stop 
    # ------------------------------
    def sb_click(self, btnStart):
        icon = QtGui.QIcon()

        if len(self.ranging['threads']) == 0:
            icon.addPixmap(QtGui.QPixmap(":/Icons/figures/Stop.png"))
            self.enable_action_buttons(False)
            btnStart.setEnabled(True) # to be able to toggle
            self.start_ranging_measurement()
            btnStart.setText(_translate("MainWindow", "Stop", None))
        else:
            self.stop_ranging_measurement()
            icon.addPixmap(QtGui.QPixmap(":/Icons/figures/Play.png"))
            btnStart.setText(_translate("MainWindow", "Start", None))

        btnStart.setIcon(icon)

    def start_button_jade_click(self):
        # self.change_text_target = self.ui.txtLog_jade
        self.sb_click(btnStart=self.ui.btnStart_jade)

    # ---------------------------------------------
    #   execute a start sequence
    #   basically start a number of threads
    # ---------------------------------------------
    def start_ranging_measurement(self):
        log = logging.getLogger(__name__)

        portname = self.app_config['defaults']['serial_port']
        if portname is None:

            # self.change_text('Please connect to %s ranging platform; no serial-port specified.' % self.app_config['general']['board'], 'red', False)
            return

        remote_boardnr = self.app_config['defaults']['remote_boardnr']

        # Create retrievedataThread
        retrievedata = rangingThread(board = self.app_config['general']['board'],
                                     rawpath = filepaths['rawfile'],
                                     portname = portname,
                                     remote_boardnr = remote_boardnr,
                                     sw_version = self.get_sw_version(),
                                     measurement_mode = self.app_config['defaults']['measurement_mode'],
                                     circle_of_trust = self.app_config['post_processing']['circle_of_trust'],
                                     circle_of_trust_tolerance=self.app_config['post_processing']['circle_of_trust_tolerance'],
                                     ake_enabled = self.app_config['defaults']['ake_enabled'],
                                     percentage_carriers = self.app_config['defaults']['percentage_carriers'])
        retrievedata.textchange.connect(self.change_led)

        self.ranging_data = valueArray(verbose=self.app_config['logfile']['verbose'],
                                       depth=self.app_config['general']['max_values'],
                                       offset=self.app_config['post_processing']['distance_offset'],
                                       bounds_lower=0.0,
                                       bounds_upper=self.app_config['post_processing']['distance_max'],
                                       flt_hysteresis=self.app_config['post_processing']['jump_threshold'],
                                       flt_window=5,
                                       flt_warnings=True,
                                       label='distance',
                                       fmt='%.2fm')
        self.aoa_data = valueArray(verbose=self.app_config['logfile']['verbose'],
                                   depth=self.app_config['general']['max_values'],
                                   offset=self.app_config['post_processing']['aoa_offset'],
                                   bounds_lower=self.app_config['post_processing']['angle_min'],
                                   bounds_upper=self.app_config['post_processing']['angle_max'],
                                   flt_hysteresis=self.app_config['post_processing']['angle_jump_threshold'],
                                   flt_window=5,
                                   flt_warnings=True,
                                   label='angle',
                                   fmt='%.1fdegrees')
        self.likeliness_data = valueArray(depth=self.app_config['general']['max_values'],
                                          verbose=self.app_config['logfile']['verbose'],
                                          label='Likeliness',
                                          fmt='%2.2f',
                                          bounds_lower=0.0,
                                          bounds_upper=101)

        if self.app_config['defaults']['measurement_mode']>1:
            self.tof_data = valueArray(verbose=self.app_config['logfile']['verbose'],
                                       depth=self.app_config['general']['max_values'],
                                       bounds_lower=0.0,
                                       bounds_upper=self.app_config['post_processing']['distance_max'],
                                       flt_hysteresis=30,
                                       flt_window=5,
                                       flt_warnings=True,
                                       label='tof-distance',
                                       fmt='%.2fm')

        retrievedata.datachange.connect(self.updatejadetab)

        retrievedata.finished.connect(self.retrievedata_finished)
        retrievedata.start()
        self.ranging['threads'].append(retrievedata)

        if filepaths['csvfile'] is not None:
            self.ranging['csv'] = open(filepaths['csvfile'], 'w+b')
            if self.ranging['csv'] is None:
                log.error('Failed opening raw CSV file for writing (%s)' % filepaths['csvfile'])
            else:
                self.ranging['csv'].write('# id, error-key, distance-raw [m], distance [m]\n'.encode())

        log.info('Started ranging measurement')

    #-------------------------------------
    #     execute a stop sequence
    #     stop all threads
    #-------------------------------------
    def stop_ranging_measurement(self):
        log = logging.getLogger(__name__)

        if len(self.ranging['threads']) == 0:
            return

        for thread in self.ranging['threads']:
            thread.stop()
            thread.quit()

        # log information
        log.info('-------------------------------------')
        error_info = self.ranging_data.get_error_info()
        for count, key, text in error_info:
            log.info('Encountered %d %s(s)' % (count, text))
        log.info('Stopped after %d ranging measurements' % self.ranging_data.get_count())
        log.info('-------------------------------------')

        for h in log.root.handlers:
            if isinstance(h, logging.handlers.MemoryHandler):
                h.flush()

        if not self.ranging['csv'] is None:
            self.ranging['csv'].close()
        # clear data
        self.ranging = {
            'activity': 'inactive',
            'threads' : [],
            'csv' : None
        }
        self.ranging_data = None
        self.authen_led.value = False
        self.circle_led.value = False

    def change_led(self, str, color, **kwargs):
        log = logging.getLogger(__name__)
        log.info(str)

        Colors = {'red': QLed.Red, 'green': QLed.Green, 'yellow': QLed.Yellow, 'grey': QLed.Grey, 'orange': QLed.Orange,
                  'purple': QLed.Purple, 'blue': QLed.Blue}
        assert color.lower() in list(Colors.keys())

        self.conn_led.value = False
        self.conn_led.onColour = Colors[color.lower()]
        self.conn_led.value = True

    def updatejadetab(self, result, returncode, reflect_id):
        log = logging.getLogger(__name__)

        if len(self.ranging['threads']) == 0:
            return

        self.measuNo += 1
        rc = self.ranging_data.insert(result['Distance'], returncode)
        self.aoa_data.insert(np.degrees(result['AoA']), returncode)
        if result['Likeliness'] != None:
            self.likeliness_data.insert(result['Likeliness']*100, returncode)

        if self.app_config['defaults']['measurement_mode'] > 1:
            self.tof_data.insert(result['ToF_Distance'], returncode)

        if not self.ranging['csv'] is None:
            key = returncode.key
            if not rc.is_valid():
                key = rc.key
            msg = '%d, %s, %.2f, %.2f' % (
            self.ranging_data.get_count(), key, self.ranging_data.raw_lst[-1], self.ranging_data.data_lst[-1])
            if 'Kest' in list(result.keys()):
                msg += ', %.1f' % np.mean(result['Kest'])
            if 'Uncertainty' in list(result.keys()):
                msg += ', %.0f' % np.mean(result['Uncertainty'])  # FIXME: only uses Uncertainty from first antenna pair
            msg += '\n'
            self.ranging['csv'].write(msg.encode())

        # check if total number of errors does not exceed limit
        if self.app_config['general']['max_errors'] > 0 and self.ranging_data.get_error_count() >= self.app_config['general'][
            'max_errors']:
            log.error('Maximum number of errors reached (%d). Stopping.' % (self.app_config['general']['max_errors']))
            # same behavior as pressing the "Start/Stop" button
            self.start_button_2_click()
            return
        # --------------------------------------------------------------------
        # Determine activity
        # --------------------------------------------------------------------
        if np.all(self.ranging_data.error_lst[-self.app_config['general']['activity_depth']:]):
            self.ranging['activity'] = 'lost'
        else:
            self.ranging['activity'] = 'active'

        # --------------------------------------------------------------------
        # Polar plot
        # --------------------------------------------------------------------
        dist_mciq = self.ranging_data.data_lst
        aoa = self.aoa_data.data_lst
        assert len(dist_mciq) == len(aoa)  # Just checking
        likeliness_val = self.likeliness_data.data_lst
        if self.app_config['defaults']['measurement_mode'] > 1:
            tof = self.tof_data.data_lst
        else:
            tof = np.array([np.NaN])

        # Clear the plot each iteration
        self.ui.MplWidget.canvas.axes.clear()
        # Data
        self.ui.MplWidget.canvas.axes.bar(0, self.app_config['post_processing']['distance_max'], width=2 * (np.pi/3),
                                          bottom=self.app_config['post_processing']['circle_of_trust'],
                                          color=(1, 0.1764, 0.1764, 0.1255))  # red
        self.ui.MplWidget.canvas.axes.bar(0, self.app_config['post_processing']['circle_of_trust'], width=2 * (np.pi/3),
                                          bottom=0.0, color=(0.1333, 0.6941, 0.298, 0.1255))  # green
        self.ui.MplWidget.canvas.axes.plot(np.radians(aoa[-1:]), dist_mciq[-1:], linewidth=2, markersize=15, marker='o')

        # Axes manipulation for a nice plotting view
        self.ui.MplWidget.canvas.axes.set_rmax(self.app_config['post_processing']['distance_max'])
        self.ui.MplWidget.canvas.axes.set_rticks(list(range(0, int(self.app_config['post_processing']['distance_max'])+1)))
        self.ui.MplWidget.canvas.axes.set_rlabel_position(self.app_config['post_processing']['angle_min'])
        self.ui.MplWidget.canvas.axes.set_thetamin(self.app_config['post_processing']['angle_min'])
        self.ui.MplWidget.canvas.axes.set_thetamax(self.app_config['post_processing']['angle_max'])
        self.ui.MplWidget.canvas.axes.set_theta_zero_location("S")
        self.ui.MplWidget.canvas.axes.annotate('Distance [m]', xy=(np.radians(self.app_config['post_processing']['angle_max']-5),
                                                                   self.app_config['post_processing']['distance_max']/2),
                                               rotation=-self.app_config['post_processing']['angle_max']/2, fontsize=13)
        #self.ui.MplWidget.canvas.fig.subplots_adjust(left=0.0, bottom=0.0, right=0.001, top=0.001)
        #self.ui.MplWidget.canvas.fig.tight_layout(pad=0.001, h_pad=0.001, w_pad=0.001)
        #self.ui.MplWidget.canvas.axes.set_position([-0.2, 0, 1, 1])
        #self.ui.MplWidget.canvas.fig.patch.set_facecolor('r')

        # Draw the plot
        self.ui.MplWidget.canvas.draw()

        # --------------------------------------------------------------------
        # Determine security and authorization
        # --------------------------------------------------------------------
        is_old_firmware = np.isnan(result['InsideCircleOfTrust'])
        if  is_old_firmware or not self.app_config['defaults']['ake_enabled']:
            self.circle_led.value = False
        else:
            self.circle_led.value = True
            if dist_mciq[-1:] < self.app_config['post_processing']['circle_of_trust'] and result['InsideCircleOfTrust']:
                self.circle_led.onColour = QLed.Green
            else:
                self.circle_led.onColour = QLed.Red

        if is_old_firmware or not self.app_config['defaults']['ake_enabled']:
            self.authen_led.value = False
        else:
            self.authen_led.value = True
            if tof[-1:] > 0:
                self.authen_led.onColour = QLed.Green
            else:
                self.authen_led.onColour = QLed.Red

        # --------------------------------------------------------------------
        # Show the distance and AoA in the LCD
        # --------------------------------------------------------------------
        self.ui.lcdNumber_jade_MCIQdist.display('%2.1f' % dist_mciq[-1:])
        self.ui.lcdNumber_jade_aoa.display('%2.1f' % aoa[-1:])

        # --------------------------------------------------------------------
        # Show Data
        # --------------------------------------------------------------------
        self.ui.likeliness_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % likeliness_val[-1:])
        self.ui.RSSI_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % result['RSSI'])
        self.ui.ToF_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % tof[-1:])
        if len(self.ranging_data.data)>1:
            self.ui.vel_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % self.ranging_data.data[1])

        # --------------------------------------------------------------------
        # Show the distance and AoA on the pedestal
        # --------------------------------------------------------------------
        if self.pedestal is not None:
            self.pedestal.set_distance_angle_color(angle = aoa[-1], distance = dist_mciq[-1])

    '''
        execute a close application 
    '''        
    def close_app(self):
        log = logging.getLogger(__name__)
        self.stop_ranging_measurement()
        log.info('Exit application')
        sys.exit()

'''
    start the application
'''
if __name__ == "__main__":
    log = logging.getLogger(__name__)
    sys._excepthook = sys.excepthook


    def exception_hook(exctype, value, traceback):
        print(exctype, value, traceback)
        sys._excepthook(exctype, value, traceback)
        sys.exit(1)


    sys.excepthook = exception_hook
    log.info('Working on %s' % sys.platform)

    app = QtWidgets.QApplication(sys.argv)
    myapp = MainWindow()
    #myapp.setWindowState(Qt.WindowFullScreen)
    myapp.show()
    sys.exit(app.exec_())