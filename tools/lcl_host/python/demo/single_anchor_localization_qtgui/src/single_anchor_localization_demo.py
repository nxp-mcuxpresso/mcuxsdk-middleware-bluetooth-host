#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

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
import logging, logging.config
from datetime import datetime
from scipy import interpolate
from matplotlib import animation

import misc.app_config as ac
from valueArray import valueArray
from threads.rangingThread import rangingThread
from dirutils import ensure_dir

from gui_single_anchor_localization import Ui_MainWindow

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
filepaths = {'logfile': None, 'csvfile': None}
timestring = datetime.now().replace(microsecond=0).strftime('%Y%m%d_%H%M%S')

curdir = os.path.dirname(os.path.realpath(__file__))  # Get directory of current python file
basepath = os.path.join(curdir, '..', 'logfiles', timestring)

filepaths['logfile'] = os.path.join(basepath, "applog" + app_config['logfile']['postfix'])
if app_config['csvfile']['enable']:
    filepaths['csvfile'] = os.path.join(basepath, "distances" + app_config['csvfile']['postfix'])
ensure_dir(filepaths['logfile'])

from logConfig import logConfig
logConfig['handlers']['file']['filename'] = filepaths['logfile']
logging.config.dictConfig(logConfig)

imec_bondi_blue = '#3F98BD'
imec_dark_gray = '#3C3C3B'
imec_light_gray = '#929497'
imec_midnight_blue = '#36337D'
imec_medium_blue = '#1582BE'
imec_turquoise = '#52BDC2'
imec_pink = '#C778AD'
imec_light_blue = '#99BDE4'
imec_purple = '#90298D'


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
        self.likeliness_data = None
        self.aoa_data = None
        self.tof_data = None

        try:
            from PedestalLocalization import PedestalLocalization
            self.pedestal = PedestalLocalization(reset=True)
        except:
            log.warning('Unable to connect to the pedestal')
            self.pedestal = None

        # GUI stuff goes here
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # GUI align to the right numeric values and start with 0.0
        self.ui.RSSI_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % np.NaN)
        self.ui.ToF_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % np.NaN)

        # Create a LED for connection
        self.conn_led=QLed(self, onColour=QLed.Red, shape=QLed.Circle)
        self.ui.connection_led.addWidget(self.conn_led)

        # Create a LED for authorization
        self.circle_led = QLed(self, onColour=QLed.Green, shape=QLed.Circle)
        self.ui.circleoftrust_led.addWidget(self.circle_led)

        self.led_dict = {'conn_led': self.conn_led, 'circ_led': self.circle_led}

        # Defining Exit buttons actions
        self.exit_action = QtGui.QAction('Exit', self)
        self.ui.btnExit.clicked.connect(self.close_app)

        # Defining Start/Test buttons
        self.ui.btnStart.clicked.connect(self.start_button_click)

        ax = self.ui.MplWidget.canvas.axes
        # Set grid properties
        ax.tick_params(labelsize=13)
        green_bar_hight = self.app_config['post_processing']['distance_max']-self.app_config['post_processing']['circle_of_trust']
        ax.bar(0, green_bar_hight, width=2 * (np.pi / 3),
               bottom=self.app_config['post_processing']['circle_of_trust'],
               color=(1, 0.1764, 0.1764, 0.1255))  # red
        ax.bar(0, self.app_config['post_processing']['circle_of_trust'], width=2 * (np.pi / 3),
               bottom=0.0, color=(0.1333, 0.6941, 0.298, 0.1255))  # green
        self.plot_objects = {'position': [], 'history': [], 'ani': None}

        # plot initial state position
        self.plot_objects['position'] = ax.plot(np.radians(0), 0, marker='o', mec=imec_purple, mfc=imec_purple,
                                                markersize=15, zorder=9)

        ax.annotate('Distance [m]', xy=(np.radians(self.app_config['post_processing']['angle_max'] - 5),
                    self.app_config['post_processing']['distance_max'] / 2),
                    rotation=-self.app_config['post_processing']['angle_max'] / 2, fontsize=13)
        self.plot_axes_update(ax)

        if self.app_config['ranging']['autostart']:
            self.sb_click(btn=self.ui.btnStart)

    # -------------------------------------------
    # Axes manipulation for a nice plotting view
    # -------------------------------------------
    def plot_axes_update(self, ax):
        ax.set_rmax(self.app_config['post_processing']['distance_max'])
        ax.set_thetamin(self.app_config['post_processing']['angle_min'])
        ax.set_thetamax(self.app_config['post_processing']['angle_max'])
        ax.set_rticks(list(range(0, int(self.app_config['post_processing']['distance_max'] + 1))))
        ax.set_rlabel_position(self.app_config['post_processing']['angle_min'])


    def enable_action_buttons(self, enabled):
        self.ui.btnStart.setEnabled(enabled)

    def retrievedata_finished(self):
        self.enable_action_buttons(True)

    # ------------------------------
    #   control start and stop 
    # ------------------------------
    def sb_click(self, btn):
        icon = QtGui.QIcon()

        if len(self.ranging['threads']) == 0:
            icon.addPixmap(QtGui.QPixmap(":/Icons/figures/Stop.png"))
            self.enable_action_buttons(False)
            btn.setEnabled(True)  # to be able to toggle
            self.start_ranging_measurement()
            btn.setText(_translate("MainWindow", "Stop", None))
        else:
            self.stop_ranging_measurement()
            icon.addPixmap(QtGui.QPixmap(":/Icons/figures/Play.png"))
            btn.setText(_translate("MainWindow", "Start", None))

        btn.setIcon(icon)

    def start_button_click(self):
        self.sb_click(btn=self.ui.btnStart)

    # ---------------------------------------------
    #   execute a start sequence
    #   basically start a number of threads
    # ---------------------------------------------
    def start_ranging_measurement(self):
        log = logging.getLogger(__name__)

        retrievedata = rangingThread(board=self.app_config['ranging']['board'],
                                     portname=self.app_config['ranging']['serial_port'],
                                     remote_brd=self.app_config['ranging']['remote_brd'],
                                     measurement_mode=self.app_config['ranging']['measurement_mode'],
                                     circle_of_trust=self.app_config['post_processing']['circle_of_trust'] + self.app_config['post_processing']['circle_of_trust_tolerance'],
                                     percentage_carriers=self.app_config['ranging']['percentage_carriers'])

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
                                          flt_hysteresis=1000,
                                          flt_window=5,
                                          flt_warnings=True,
                                          offset=0,
                                          fmt='%2.2f',
                                          bounds_lower=0.0,
                                          bounds_upper=101)

        if self.app_config['ranging']['measurement_mode']>1:
            self.tof_data = valueArray(verbose=self.app_config['logfile']['verbose'],
                                       depth=self.app_config['general']['max_values'],
                                       bounds_lower=0.0,
                                       bounds_upper=20,
                                       flt_hysteresis=30,
                                       flt_window=5,
                                       flt_warnings=True,
                                       offset=self.app_config['post_processing']['tof_dist_offset'],
                                       label='tof-distance',
                                       fmt='%.2fm')

        retrievedata.change_led.connect(self.turn_on_led)
        retrievedata.datachange.connect(self.updateplot)
        retrievedata.finished.connect(self.retrievedata_finished)
        retrievedata.start()
        self.ranging['threads'].append(retrievedata)

        self.plot_objects['history'].clear()
        self.coordinates = np.array([0., 0.]).reshape((1, 2))

        if filepaths['csvfile'] is not None:
            self.ranging['csv'] = open(filepaths['csvfile'], 'w+b')
            if self.ranging['csv'] is None:
                log.error('Failed opening raw CSV file for writing (%s)' % filepaths['csvfile'])
            else:
                self.ranging['csv'].write('# id, error-key, distance-raw [m], distance [m]\n'.encode())

        log.info('Started ranging measurement')

    #-------------------------------------------------
    #     execute a stop sequence stop all threads
    #-------------------------------------------------
    def stop_ranging_measurement(self):
        log = logging.getLogger(__name__)

        if len(self.ranging['threads']) == 0:
            return

        for thread in self.ranging['threads']:
            thread.stop()
            thread.quit()

        # log information
        log.info('-------------------------------------')
        log.info('Encountered %d errors' % self.ranging_data.get_error_count())
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
            'threads': [],
            'csv': None
        }

    # -----------------------------
    #    turn on a defined led
    # -----------------------------
    def turn_on_led(self, led_color, **kwargs):
        Colors = {'red': QLed.Red, 'green': QLed.Green, 'yellow': QLed.Yellow, 'grey': QLed.Grey,
                  'orange': QLed.Orange,
                  'purple': QLed.Purple, 'blue': QLed.Blue}

        for k, led in enumerate(led_color.keys()):
            if led == 'all':
                for n, all_led in enumerate(self.led_dict):
                    temp_led = self.led_dict.get(all_led)
                    assert led_color['all'].lower() in list(Colors.keys())
                    temp_led.setOnColour(Colors[led_color['all'].lower()])
                    temp_led.value = True
                return
            else:
                temp_led = self.led_dict.get(led)
                assert led_color[led].lower() in list(Colors.keys())
                temp_led.setOnColour(Colors[led_color[led].lower()])
                temp_led.value = True

    def updateplot(self, results):
        log = logging.getLogger(__name__)

        if len(self.ranging['threads']) == 0 or len(results) == 0:
            return

        result = results[0]
        isvalid = result['meta.error_msg'] == ''

        self.ranging_data.insert(result.get('mciq.result.distance', np.nan), isvalid)
        self.aoa_data.insert(np.degrees(result.get('mciq.result.aoa', np.nan)), isvalid)

        if result.get('mciq.result.likeliness') is not None:
            self.likeliness_data.insert(result.get('mciq.result.likeliness') * 100, isvalid)

        if self.app_config['ranging']['measurement_mode'] > 1:
            self.tof_data.insert(result.get('tof.result.distance', np.nan), isvalid)

        # Saving coordinate position in csv file
        if not self.ranging['csv'] is None:
            key = 'OK' if isvalid else 'FAILED'
            msg = '%d, %s, %.2f, %.2f' % (
            self.ranging_data.get_count(), key, self.ranging_data.raw_lst[-1], self.ranging_data.data_lst[-1])
            msg += '\n'
            self.ranging['csv'].write(msg.encode())

        # check if total number of errors does not exceed limit
        if self.app_config['general']['max_errors'] > 0 and self.ranging_data.get_error_count() >= self.app_config['general']['max_errors']:
            log.error('Maximum number of errors reached (%d). Stopping.' % (self.app_config['general']['max_errors']))
            self.sb_click(btn=self.ui.btnStart)
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
        if self.app_config['ranging']['measurement_mode'] > 1:
            tof = self.tof_data.data_lst
        else:
            tof = np.array([np.NaN])

        ax = self.ui.MplWidget.canvas.axes

        # plot prev coordinate and store it in history
        prev_aoa = self.coordinates[-1][0]
        prev_dist = self.coordinates[-1][1]
        pl = ax.plot(np.radians(prev_aoa), prev_dist, marker='o', linewidth=0, markersize=15, mec=imec_pink, mfc=imec_pink, alpha=0.1)
        self.plot_objects['history'].extend(pl)
        self.coordinates = np.vstack((self.coordinates, np.array([aoa[-1], dist_mciq[-1]]).reshape(1,2)))

        # Keep only the 'max_values' amount of previous dots
        if len(self.plot_objects['history']) > self.app_config['general']['max_values']:
            ax.lines.remove(self.plot_objects['history'].pop(0))

        if len(self.coordinates) > 2:  # It is only necessary the last two coordinates to do interpolation
            self.coordinates = self.coordinates[1:, :]

        self.update_position()

        # --------------------------------------------------------------------
        # Determine security and authorization
        # --------------------------------------------------------------------
        if dist_mciq[-1:] < self.app_config['post_processing']['circle_of_trust'] and result.get('tof.result.in_cot', False) and not np.isnan(result.get('tof.result.distance', np.nan)):
            self.turn_on_led({'circ_led': 'green'})
        else:
            self.turn_on_led({'circ_led': 'red'})

        # --------------------------------------------------------------------
        # Show the distance and AoA in the LCD
        # --------------------------------------------------------------------
        self.ui.lcd_dist.display('%2.1f' % prev_dist)
        self.ui.lcd_aoa.display('%2.1f' % prev_aoa)

        # --------------------------------------------------------------------
        # Show Data
        # --------------------------------------------------------------------
        try:
            self.ui.progressBar_brd1.setValue(likeliness_val[-1:])
            self.ui.RSSI_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % result.get('mciq.initiator.rssi'))
            self.ui.ToF_textEdit.setHtml('<div style="text-align: right">%.1f</div>' % tof[-1:])
        except:
            pass
        # --------------------------------------------------------------------
        # Show the distance and AoA on the pedestal
        # --------------------------------------------------------------------
        if self.pedestal is not None:
            self.pedestal.set_distance_angle_color(angle = aoa[-1], distance = dist_mciq[-1])

    @staticmethod
    def update_objects(num, dataLines, lines):
        data = dataLines[num]
        for n, line in enumerate(lines):
            if n == 0:  # current position (point)
                line.set_data(data[0], data[1])
            else:  # likeliness lines
                x, y = line.get_data()
                x[1] = data[0]
                y[1] = data[1]
                line.set_data(x, y)
        return lines

    def update_position(self):
        ax = self.ui.MplWidget.canvas.axes
        objects = self.plot_objects['position']
        if len(self.coordinates) == 0:
            return
        elif len(self.coordinates) == 1:
            self.update_objects(0, self.coordinates, objects)
            return
        coord0 = self.coordinates[-2, :]
        coord1 = self.coordinates[-1, :]
        if np.array_equal(coord0, coord1):
            return

        fig = self.ui.MplWidget.canvas.fig

        n_interpolation = self.app_config['post_processing']['pt_interpol']
        aoa_coord = np.array([coord0[0], coord1[0]])
        dist_coord = np.array([coord0[1], coord1[1]])
        if n_interpolation > 1:  # interpolate
            step_size = 1. / (n_interpolation - 1)
            samples = np.arange(0, 2)
            samplesnew = np.arange(0, 1. + step_size - 0.01, step_size)

            faoa = interpolate.interp1d(samples, aoa_coord)
            fdist = interpolate.interp1d(samples, dist_coord)

            aoa_new = faoa(samplesnew)
            dist_new = fdist(samplesnew)
            self.plot_objects['ani'] = animation.FuncAnimation(
                fig, self.update_objects, len(aoa_new), fargs=(list(zip(np.radians(aoa_new), dist_new)), objects), interval=5, repeat=False)
        else:  # no interpolation
            self.plot_objects['ani'] = animation.FuncAnimation(
                fig, self.update_objects, 1, fargs=(list(zip(np.radians(aoa_coord), dist_coord)), objects), interval=1, repeat=False)

        self.plot_axes_update(ax)
        self.ui.MplWidget.canvas.draw()

    def close_app(self):
        log = logging.getLogger(__name__)
        self.stop_ranging_measurement()
        log.info('Exit application')
        sys.exit()


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