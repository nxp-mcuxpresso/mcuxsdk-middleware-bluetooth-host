from PyQt5.QtCore import QThread, pyqtSignal
from time import strftime
from pytimer import Timer
import logging
from RangingPlatform import RangingPlatform
import range_estimator_pkg as range_estimator

class rangingThread(QThread):
    datachange = pyqtSignal(object, object, object) # (result, ReturnCode, remote unique id)
    textchange = pyqtSignal(object, object, bool)  # text, color, append

    '''
         setup class
    '''
    def __init__(self, **kargs):
        QThread.__init__(self)
        self.requestStop = False
        self.sleeptime = 20
        self.maxRawMeasurements = 4096
        self.measNo = 0
        self.rawfh = None
        self.params = {}
        self.dut = None

        for key in ['board', 'remote_boardnr', 'portname', 'sw_version', 'rawpath', 'measurement_mode', 'circle_of_trust', 'circle_of_trust_tolerance', 'ake_enabled', 'percentage_carriers']:
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
        self.textchange.emit('Ranging setup is initiating.', 'orange', False)

        if not self.params['rawpath'] is None:
            self.rawfh = open(self.params['rawpath'], 'ab')
            if self.rawfh is None:
                self.textchange.emit('Failed opening %s for writing.' % self.params['rawpath'], 'red', True)
                return

        self.dut = RangingPlatform(board=self.params['board'],
                                   RangeEstimator={'channel_reconstruct': range_estimator.ChannelReconstruct_2D(),
                                                   'ranging_engine': range_estimator.RangingEngine_Music_AoA()},
                                   ResultFile={'max_entries': 1},
                                   usemaxbaudrate=True)
        self.dut.estimator_config('re.La', 4)
        self.dut.estimator_config('re.type', range_estimator.AlgoType.LEFTRIGHT)
        self.dut._estimator['config'].pop('re.TOL2', None)     # FIXME: this is a workaround, but it should not happen in the first place.

        freq_step = 1e6
        f_stop = 2479e6     # avoid last advertisement channel
        if freq_step == 0.5e6:
            f_stop = 2440e6
            self.dut.estimator_config('re.maxiter', 200)       # increase for larger distances
            self.dut.estimator_config('re.MethodSubspaceSep', 2)  # decrease when outliers with cabled measurements
        elif freq_step == 1e6:
            self.dut.estimator_config('re.maxiter', 100)       # increase for larger distances
            self.dut.estimator_config('re.MethodSubspaceSep', 8)  # decrease when outliers with cabled measurements
        elif freq_step == 2e6:
            self.dut.estimator_config('re.maxiter', 140)       # increase for larger distances
            self.dut.estimator_config('re.MethodSubspaceSep', 8)  # decrease when outliers with cabled measurements
        else:
            self.dut.estimator_config('re.maxiter', 200)       # increase for larger distances
            self.dut.estimator_config('re.MethodSubspaceSep', 8)  # decrease when outliers with cabled measurements
        self.dut.set_freq_range_step_L(fstart=2401e6, fstop=f_stop, deltaf=freq_step, L=None)
        self.dut.estimator_config('re.MaxDist', 15.0)

        # self.dut = RangingPlatform(board=self.params['board'],
        #                            cr=range_estimator.ChannelReconstruct_2D(),
        #                            re=range_estimator.RangingEngine_Music_AoA())
        # self.dut.estimator.re.La = 4
        # self.dut.estimator.re.type = range_estimator.AlgoType.LEFTRIGHT

        try:
            self.dut.initialize(port=self.params['portname'])
        except:
            log.exception('DUT was not initialized')
            raise

        if self.dut is None or not self.dut.connected:
            self.textchange.emit('Connection to %s ranging platform has failed.' % self.params['board'], 'red', True)
            self.close_thread()
            return

        self.dut.set_remote_address(self.dut.boardnumber2address(self.params['remote_boardnr']))
        self.dut.set_percentage_carriers(self.params['percentage_carriers'])
        self.dut.set_circle_of_trust(self.params['circle_of_trust'] + self.params['circle_of_trust_tolerance'])
        self.dut.set_measurement_mode(self.params['measurement_mode'], refl_boardnumber=self.params['remote_boardnr'],
                                      init_boardnumber=self.dut.BoardNumber)
        self.dut.store_parameters()

        if not self.dut.wait_on_connection():
            self.textchange.emit('No remote device is detected. Please check Remote Board Number and its configuration.', 'red', True)
            self.close_thread()
            return
        elif not self.dut.SwVersion in self.params['sw_version']:
            log.info(f'SW version of Local board {self.dut.SwVersion} is in supported-list {self.params["sw_version"]}.')
            self.textchange.emit('SW version of Local board is not supported.', 'red', True)
            self.close_thread()
            return

        self.dut.enable_raw_data(0)

        self.textchange.emit('Ranging is started.', 'green', False)

        timer = Timer()
        while not self.requestStop:
            self.msleep(self.sleeptime)

            timer.restart()
            try:
                Result = self.dut.run_range_measurement()[0]
                timer.checkpoint('run_range_measurement')
                log.info('time to execute measurement: %.2fs (%.2fs average)' % (
                            timer.cur_time['run_range_measurement'], timer.cum_time['run_range_measurement'] / timer.n))
                log.info(f"Estimated CFO is {Result['Result']['CFO'] / 1000:.1f}kHz")

                Result['Result']['RSSI'] = Result['Initiator']['RSSI'] # Add RSSI to 'Result' dict
            except:
                log.exception('An exception occured in either the run_range_measurement or in addressing the data.', exc_info=True)
                from RangingPlatformTypes import ReturnCodes
                Result['ReturnCode'] = ReturnCodes('badresponse')

            self.measNo += 1
            if self.maxRawMeasurements:
                self.maxRawMeasurements -= 1

            #FIXME: UniqueID might not exist when connection is lost
            try:
                self.datachange.emit(Result['Result'], Result['ReturnCode'], Result['Remote']['UNIQUEID'])
                self.textchange.emit('Ranging set-up is connected.', 'green', False)
            except:
                self.textchange.emit('Ranging set-up lost connection.', 'orange', False)
                log.info('Ranging set-up lost connection.')

        self.textchange.emit('Ranging is stopped.', 'green', False)
        self.close_thread()

    def close_thread(self):
        if not self.rawfh is None:
            self.rawfh.close()
            self.rawfh = None
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
