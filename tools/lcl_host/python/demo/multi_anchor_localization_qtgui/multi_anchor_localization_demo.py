#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

gui_version = 'v2.0.0'

import sys
import os

# Include directories for ranging algorithm, images, etc
curdir = os.path.dirname(os.path.realpath(__file__))        # Get directory of current python file
sys.path.append(os.path.join(curdir, "..", "common"))
sys.path.append(os.path.join(curdir, "..", "..", "Instruments"))
sys.path.append(os.path.join(curdir, "..", "..", "Generic"))
del curdir

import numpy as np
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import *
from QLed import QLed
import matplotlib.path as mpath
import matplotlib.patches as mpatches

import pyqtgraph as pg
import logging, logging.config
from time import strftime
from scipy import interpolate
from matplotlib import animation

import misc.app_config as ac
from threads.rangingThread import rangingThread
from locationArray import locationArray
from dirutils import ensure_dir
from gui_multi_anchor_localization import Ui_MainWindow

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
timestring = strftime('%Y%m%d_%H%M%S')

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

import GUI_colormap
hex_list = ['#3DDE1E', '#9BEC1A', '#F9EB18', '#F98020', '#F51329'] # green to red gradient

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        QtWidgets.QWidget.__init__(self, parent)
        self.app_config = ac.read()

        self.ranging = {
            'activity': None,
            'threads': None,
            'csv': None
        }
        self.t_end_prev = None
        # TODO: check arrayconfig; are there any fields missing or superfluous
        self.locationdata = locationArray(loc_est_cfg=self.app_config['location_estimator'], array_cfg=self.app_config['1D_filter'])

        # GUI stuff goes here
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # Create a LED for connection
        self.conn_led = QLed(self, onColour=QLed.Red, shape=QLed.Circle)
        self.ui.connection_led.addWidget(self.conn_led)
        
        # Create a LED for authorization
        self.circle_led = QLed(self, onColour=QLed.Green, shape=QLed.Circle)
        self.ui.circleoftrust_led.addWidget(self.circle_led)
        self.circle_led.hide()
        
        # Hide Circle of Trust and Authentication labels at the beginning
        self.ui.circleoftrust_label.setHidden(True)

        # Show LED and enable label if there is both MCIQ and ToF data
        # i.e. distance bounding, measurement mode 3 and 7
        if self.app_config['ranging']['measurement_mode'] in [3,7]:
            self.circle_led.show()
            self.ui.circleoftrust_label.setHidden(False)

        self.led_dict = {'conn_led': self.conn_led, 'circ_led': self.circle_led}

        # Defining Exit buttons actions
        self.exit_action = QtGui.QAction('Exit', self)
        self.ui.btnExit.clicked.connect(self.close_app)

        # Defining Start/Test buttons
        self.ui.btnStart.clicked.connect(self.start_button_click)

        anchor_pos = np.array(self.locationdata.get_anchor_positions())
        anchor_IDs = np.array(self.locationdata.get_anchor_ids())

        ax = self.ui.MplWidget.canvas.axes

        # Draw a car (rectangle figure)
        if self.app_config['plot']['draw_car_enabled']:
            corners = [
                mpath.Path.MOVETO,
                mpath.Path.LINETO,
                mpath.Path.LINETO,
                mpath.Path.LINETO,
                mpath.Path.CLOSEPOLY]
            path = mpath.Path(np.append(anchor_pos[0:4, 0:2], [anchor_pos[0, 0:2]], axis=0), corners)
            car_contour = mpatches.PathPatch(path, facecolor=imec_light_gray, lw=0)
            ax.add_patch(car_contour)

        # Set grid properties (limits, ticks, labels, titles)
        x_lim = self.locationdata.get_xlimits()
        y_lim = self.locationdata.get_ylimits()
        x_major_ticks = np.arange(min(x_lim), max(x_lim)+1, 1)
        x_minor_ticks = np.arange(min(x_lim), max(x_lim), 0.5)
        y_major_ticks = np.arange(min(y_lim), max(y_lim) + 1, 1)
        y_minor_ticks = np.arange(min(y_lim), max(y_lim), 0.5)

        ax.clear()
        ax.axhline(0, color='black', lw=1.5)
        ax.axvline(0, color='black', lw=1.5)
        ax.set_xlim(x_lim)
        ax.set_ylim(y_lim)
        ax.set_ylabel('y (m)', fontsize=15, fontweight='bold')
        ax.set_xlabel('x (m)', fontsize=15, fontweight='bold')
        ax.tick_params(labelsize=13)
        ax.set_xticks(x_major_ticks)
        ax.set_xticks(x_minor_ticks, minor=True)
        ax.set_yticks(y_major_ticks)
        ax.set_yticks(y_minor_ticks, minor=True)
        if self.app_config['plot']['minor_ticks']:
            ax.grid(True, linestyle='-.', which='both')
        else:
            ax.grid(True, linestyle='-.', which='major')
        if self.app_config['plot']['match_scaling']:
            ax.set_aspect('equal', 'box')

        self.plot_objects = {'position': [], 'history': [], 'likeliness': [], 'likeliness_sqr': [], 'ani': None}
        coord = self.locationdata.le.initial_state_pos.tolist()
        self.coordinates = np.array([coord, [0, 0]]).reshape((2, 2))

        # Defining likeliness colormap
        self.GUI_cmap = GUI_colormap.get_continuous_cmap(hex_list)

        # Plot likeliness attached to each anchor
        for x, y, z in anchor_pos:
            pl = ax.plot([x, coord[0]], [y, coord[1]], c='gray', ls='--', lw=1, alpha=0.5)
            self.plot_objects['likeliness'].extend(pl)

        # Add anchors name/board_number to the plot
        for i in range(len(anchor_pos)):
            ax.text(anchor_pos[i, 0] + 0.05, anchor_pos[i, 1] + 0.05, anchor_IDs[i], size=15, ha='left', va='bottom')

        # Draw anchors as squares on the plot
        ax.plot(anchor_pos[:, 0], anchor_pos[:, 1], marker='s', lw=0, mec=imec_bondi_blue, mew=2, mfc='w', markersize=15)

        # plot initial state position
        self.plot_objects['position'] = ax.plot(coord[0], coord[1], marker='o', mec=imec_purple, mfc=imec_purple, markersize=15, zorder=9)

        # reorder values to anchor_IDs
        sidx = np.argsort(anchor_IDs)
        anchor_IDs = anchor_IDs[sidx]

        # Add anchor names in the left-side of the plot
        self.ui.likeliness_textEdit_brd1.setHtml('<div style="text-align: left">Board %.2d</div>' % anchor_IDs[0])
        self.ui.likeliness_textEdit_brd2.setHtml('<div style="text-align: left">Board %.2d</div>' % anchor_IDs[1])
        self.ui.likeliness_textEdit_brd3.setHtml('<div style="text-align: left">Board %.2d</div>' % anchor_IDs[2])
        self.ui.likeliness_textEdit_brd4.setHtml('<div style="text-align: left">Board %.2d</div>' % anchor_IDs[3])
        if len(anchor_IDs) > 4:
            self.ui.likeliness_textEdit_brd5.setHtml('<div style="text-align: left">Board %.2d</div>' % anchor_IDs[4])
        else:
            self.ui.progressBar_brd5.hide()
            
        if self.app_config['ranging']['autostart']:
            self.sb_click(btn=self.ui.btnStart)

    def enable_action_buttons(self, enabled):
        self.ui.btnStart.setEnabled(enabled)

    def retrievedata_finished(self):
        self.enable_action_buttons(True)

    # ------------------------------
    #   control start and stop 
    # ------------------------------
    def sb_click(self, btn):
        icon = QtGui.QIcon()

        if self.ranging['threads'] is None:
            icon.addPixmap(QtGui.QPixmap(":/Icons/figures/Stop.png"))
            self.enable_action_buttons(False)
            btn.setEnabled(True) # to be able to toggle
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

        retrievedata = rangingThread(anchor_list=self.locationdata.get_anchor_ids(),
                                     **self.app_config['ranging'])

        retrievedata.change_led.connect(self.turn_on_led)
        retrievedata.datachange.connect(self.updateplot)
        retrievedata.finished.connect(self.retrievedata_finished)
        retrievedata.start()
        self.ranging['threads'] = retrievedata

        self.plot_objects['history'].clear()
        self.coordinates = np.array([0., 0.]).reshape((1, 2))

        curdir = os.path.split(logConfig['handlers']['file']['filename'])[0]
        for n in range(32):
            filepath = os.path.join(curdir, 'info.%d.csv' % n)
            if not os.path.isfile(filepath):
                self.ranging['csv'] = open(filepath, 'wb')
                break
        if self.ranging['csv'] is not None:
            self.ranging['csv'].write('timestamp,returncode,anchorid,distance,posx,posy\n'.encode())
        else:
            log.warning('Error while opening csv for writing..')

        log.info('Started ranging measurement')

    # --------------------------------------------------
    #       execute a stop sequences stop all threads
    # --------------------------------------------------
    def stop_ranging_measurement(self):
        log = logging.getLogger(__name__)

        if self.ranging['threads'] is None:
            return

        self.ranging['threads'].stop()
        self.ranging['threads'].quit()

        # log information
        log.info('-------------------------------------')
        log.info('Stopped after %d ranging measurements' % (self.locationdata.get_count()/4))
        log.info('-------------------------------------')

        for h in log.root.handlers:
            if isinstance(h, logging.handlers.MemoryHandler):
                h.flush()

        if self.ranging['csv'] is not None:
            self.ranging['csv'].close()
        # clear data
        self.ranging = {
            'activity': None,
            'threads': None,
            'csv': None
        }

    # -----------------------------
    #    turn on a defined led
    # -----------------------------
    def turn_on_led(self, led_color, **kwargs):
        Colors = {'red': QLed.Red, 'green': QLed.Green, 'yellow': QLed.Yellow, 'grey': QLed.Grey, 'orange': QLed.Orange,
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

        anchor_pos = np.array(self.locationdata.get_anchor_positions())
        anchor_IDs = np.array(self.locationdata.get_anchor_ids())
        sidx = np.argsort(anchor_IDs)
        # reorder values to anchor_IDs
        anchor_pos = anchor_pos[sidx, :]
        anchor_IDs = anchor_IDs[sidx]
        n_anchors = len(anchor_IDs)

        if self.ranging['threads'] is None:
            return

        isvalid_lst = []
        distnan_lst = []
        t_ref = np.empty(len(results), dtype=np.int64)
        for k, result in enumerate(results):
            isvalid_lst.append(result['meta.error_msg'] == '')  # result['meta']['error_msg']
            distnan_lst.append(np.isnan(result['mciq.result.distance']))
            t_ref[k] = result['meta.profiling.t_reference']

        if not(np.all(isvalid_lst)):
            for k, result in enumerate(results):
                if not isvalid_lst[k]:
                    log.warning(f'Invalid data because of error-message from remote-board {result["meta.reflector.board_nr"]} with message {result["meta.error_msg"]}')
            return
        elif len(results) != n_anchors:
            log.warning(f'The length of the results-dict ({len(results)}) does not equal the number of anchors ({n_anchors})')
            return
        elif np.any(distnan_lst):
            log.warning(f'One or more distance values are NaN')
            return

        remote_board = np.zeros((n_anchors,), dtype=int)
        likeliness = np.zeros((n_anchors,), dtype=float)
        for k, result in enumerate(results):
            remote_board[k] = result['meta.reflector.board_nr']
            likeliness[k] = result.get('mciq.result.likeliness', 0.)
            if self.t_end_prev is not None:
                t_delta = t_ref[k] - self.t_end_prev[k]
            else:
                t_delta = 0.1
            self.locationdata.insert(result['mciq.result.distance'], isvalid_lst[k], remote_board[k], delta_t=t_delta)

        self.t_end_prev = t_ref.copy()

        sidx = np.argsort(remote_board)
        # reorder values to remote_board --> anchor_pos and anchor_IDs also have the same ordering
        remote_board = remote_board[sidx]
        likeliness = likeliness[sidx]

        # --------------------------------------------------------------------
        # Plotting
        # --------------------------------------------------------------------
        ax = self.ui.MplWidget.canvas.axes
        coord = self.locationdata.update()[:2]

        # Saving coordinate position in csv file
        if self.ranging['csv'] is not None:
            for k, result in enumerate(results):
                ret_code = 'OK' if result['meta.error_msg'] == '' else 'FAIL'
                msg = '%s,%s,%d,%.2f,%.2f,%.2f ' % (strftime('%Y%m%dT%H%M%S'), ret_code, result['meta.reflector.board_nr'],
                                                    result['mciq.result.distance'], coord[0], coord[1])
                msg += '\n'
                self.ranging['csv'].write(msg.encode())

        # plot prev coordinate and store it in history
        prev_coord = self.coordinates[-1, :]
        pl = ax.plot(prev_coord[0], prev_coord[1], marker='o', linewidth=0, markersize=15, mec=imec_pink, mfc=imec_pink, alpha=0.1)
        self.plot_objects['history'].extend(pl)
        self.coordinates = np.vstack((self.coordinates, np.array(coord).reshape(1, 2)))

        # Keep only the 'max_values' amount of previous dots
        if len(self.plot_objects['history']) > self.app_config['general']['max_values']:
            ax.lines.remove(self.plot_objects['history'].pop(0))

        if len(self.coordinates) > 2:  # It is only necessary the last two coordinates to do interpolation
            self.coordinates = self.coordinates[1:, :]

        # remove all attached likeliness lines
        for item in self.plot_objects['likeliness']:
            ax.lines.remove(item)
        self.plot_objects['likeliness'].clear()

        # remove all square likeliness anchors
        for item in self.plot_objects['likeliness_sqr']:
            ax.lines.remove(item)
        self.plot_objects['likeliness_sqr'].clear()

        # plot all attached likeliness lines
        for k in range(n_anchors):
            p1 = ax.plot([anchor_pos[k, 0], prev_coord[0]], [anchor_pos[k, 1], prev_coord[1]],
                         c=self.GUI_cmap(1 - likeliness[k]), lw=2.5)
            p2 = ax.plot(anchor_pos[k, 0], anchor_pos[k, 1], marker='s', lw=0, mec=self.GUI_cmap(1 - likeliness[k]),
                    mew=2, mfc=self.GUI_cmap(1 - likeliness[k]), markersize=15)
            self.plot_objects['likeliness'].extend(p1)
            self.plot_objects['likeliness_sqr'].extend(p2)

        self.update_position()
        
        # --------------------------------------------------------------------
        # Determine circle of trust LED status
        # --------------------------------------------------------------------
        if self.app_config['ranging']['measurement_mode'] in [3, 7]:
            cot_lst = []
            for k, result in enumerate(results):
                cot_lst.append(np.isnan(result['tof.result.in_cot']))

            if np.all(cot_lst):
                self.turn_on_led({'circ_led': 'green'})
            else:
                self.turn_on_led({'circ_led': 'red'})
                
        # --------------------------------------------------------------------
        # Show the distance in the LCD
        # --------------------------------------------------------------------
        self.ui.lcdNumber_position_x.display('%2.1f' % coord[0])
        self.ui.lcdNumber_position_y.display('%2.1f' % coord[1])

        # --------------------------------------------------------------------
        # Show Data
        # --------------------------------------------------------------------
        try:
            self.ui.progressBar_brd1.setValue(100.0 * likeliness[0])
            self.ui.progressBar_brd2.setValue(100.0 * likeliness[1])
            self.ui.progressBar_brd3.setValue(100.0 * likeliness[2])
            self.ui.progressBar_brd4.setValue(100.0 * likeliness[3])
            if len(remote_board) > 4:
                self.ui.progressBar_brd5.setValue(100.0 * likeliness[4])
        except:
            pass

        activity_lvl = self.locationdata.get_activity(depth=self.app_config['general']['activity_depth'])
        activity = self.ranging['activity']
        if activity_lvl >= self.app_config['general']['activity_minimum']:
            self.ranging['activity'] = True
        else:
            self.ranging['activity'] = False
        if activity != self.ranging['activity']:
            if self.ranging['activity']:
                self.turn_on_led({'conn_led':'green'})
                log.info('Platform is stable again')
            else:
                self.turn_on_led({'conn_led':'orange', 'circ_led': 'orange'})
                log.info('Platform is unstable')

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
        objects = self.plot_objects['position'] + self.plot_objects['likeliness']
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

        n_interpolation = self.app_config['plot']['pt_interpol']
        xcoord = np.array([coord0[0], coord1[0]])
        ycoord = np.array([coord0[1], coord1[1]])
        if n_interpolation > 1: # interpolate
            step_size = 1. / (n_interpolation - 1)
            samples = np.arange(0, 2)
            samplesnew = np.arange(0, 1. + step_size - 0.01, step_size)

            fx = interpolate.interp1d(samples, xcoord)
            fy = interpolate.interp1d(samples, ycoord)

            xnew = fx(samplesnew)
            ynew = fy(samplesnew)
            self.plot_objects['ani'] = animation.FuncAnimation(
                fig, self.update_objects, len(xnew), fargs=(list(zip(xnew, ynew)), objects), interval=5, repeat=False)
        else:       # no interpolation
            self.plot_objects['ani'] = animation.FuncAnimation(
                fig, self.update_objects, 1, fargs=(list(zip(xcoord, ycoord)), objects), interval=1, repeat=False)

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
