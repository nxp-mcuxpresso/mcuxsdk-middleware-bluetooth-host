#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

from PyQt5.QtCore import QThread, pyqtSignal
from pytimer import Timer
import logging
import numpy as np
from ranging_platform_pkg import RangingPlatform
import range_estimator_pkg as range_estimator

class rangingThread(QThread):
    datachange = pyqtSignal(object)  # results
    change_led = pyqtSignal(dict)

    '''
         setup class
    '''
    def __init__(self, **kargs):
        QThread.__init__(self)
        self.requestStop = False
        self.sleeptime = 20
        self.params = {}
        self.dut = None

        for key in ['board', 'remote_brd', 'portname', 'measurement_mode', 'circle_of_trust', 'percentage_carriers']:
            self.params[key] = kargs.get(key, None)

    def setParameters(self, **kargs):
        for k, v in list(kargs.items()):
            self.params[k] = v

    '''
        run the thread
    '''
    def run(self):
        log = logging.getLogger(__name__)

        self.setPriority(QThread.LowPriority)
        self.requestStop = False
        self.change_led.emit({'conn_led': 'yellow'})
        log.info('Ranging setup is initiating.')

        self.dut = RangingPlatform(board=self.params['board'],
                                   RangeEstimator={'channel_reconstruct': range_estimator.ChannelReconstruct_2D(),
                                                   'ranging_engine': range_estimator.RangingEngine_Music_AoA()},
                                   ResultFile={'max_entries': 1},
                                   usemaxbaudrate=True)

        self.dut.estimator_config('re.La', 4)
        self.dut.estimator_config('re.type', range_estimator.AlgoType.LEFTRIGHT)

        try:
            self.dut.initialize(port=self.params['portname'])
        except:
            pass

        if self.dut is None or not self.dut.connected:
            self.change_led.emit({'conn_led': 'red'})
            log.info('Connection to %s ranging platform has failed.' % self.params['board'])
            self.close_thread()
            return

        freq_step = 1e6
        f_stop = 2479e6     # avoid last advertisement channel
        if freq_step == 0.5e6:
            f_stop = 2440e6
            self.dut.estimator_config('re.maxiter', 200)
            self.dut.estimator_config('re.MethodSubspaceSep', 2)
        elif freq_step == 1e6:
            self.dut.estimator_config('re.maxiter', 100)
            self.dut.estimator_config('re.MethodSubspaceSep', 8)
        elif freq_step == 2e6:
            self.dut.estimator_config('re.maxiter', 140)
            self.dut.estimator_config('re.MethodSubspaceSep', 8)
        else:
            self.dut.estimator_config('re.maxiter', 200)
            self.dut.estimator_config('re.MethodSubspaceSep', 8)
        self.dut.set_freq_range_step_L(fstart=2401e6, fstop=f_stop, deltaf=freq_step, L=None)
        self.dut.estimator_config('re.MaxDist', 15.0)

        self.dut.set_remote_address(self.dut.boardnumber2address(self.params['remote_brd']))
        self.dut.set_percentage_carriers(self.params['percentage_carriers'])
        self.dut.set_circle_of_trust(self.params['circle_of_trust'])
        self.dut.set_measurement_mode(self.params['measurement_mode'], refl_boardnumber=self.params['remote_brd'],
                                      init_boardnumber=self.dut.BoardNumber)
        self.dut.store_parameters()

        if not self.dut.wait_on_connection():
            self.change_led.emit({'conn_led': 'red'})
            log.info('No remote device is detected. Please check Remote Board Number and its configuration.')
            self.close_thread()
            return

        self.dut.enable_raw_data(0)

        self.change_led.emit({'conn_led': 'green'})
        log.info('Ranging starting..')

        timer = Timer()
        while not self.requestStop:
            self.msleep(self.sleeptime)

            timer.restart()
            try:
                results = self.dut.run_range_measurement()
                timer.checkpoint('run_range_measurement')
                log.debug('time to execute measurement: %.2fs (%.2fs average)' % (
                            timer.cur_time['run_range_measurement'], timer.cum_time['run_range_measurement'] / timer.n))
                for result in results:
                    log.debug(f"Estimated CFO is {result.get('mciq.result.cfo', np.nan) / 1000:.1f}kHz")
            except:
                results = []

            try:
                self.datachange.emit(results)
                self.change_led.emit({'conn_led': 'green'})
            except:
                self.change_led.emit({'all': 'orange'})
                log.warning('Ranging set-up lost connection.')

        log.info('Ranging is stopping..')
        self.change_led.emit({'all': 'grey'})
        self.close_thread()

    def close_thread(self):
        if not self.dut is None:
            self.dut.ifc.close()
            self.dut = None

    '''
        stop in a controlled manner
    '''
    def stop(self):
        self.requestStop = True
        while self.isRunning():
            pass
