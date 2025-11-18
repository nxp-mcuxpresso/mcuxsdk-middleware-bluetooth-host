#------------------------------------------------------------------------------
# Copyright : Stichting imec Nederland (http://www.imec.nl)
#             *** IMEC CONFIDENTIAL ***
#------------------------------------------------------------------------------

from PyQt5.QtCore import QThread, pyqtSignal
import logging
from ranging_platform_pkg import RangingPlatform

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

        for key in ['board', 'anchor_list', 'init_brd', 'serial_port', 'timeout',
                    'measurement_mode', 'antinit', 'antrefl']:
            self.params[key] = kargs.get(key, None)

    def set_parameters(self, **kargs):
        for k, v in list(kargs.items()):
                self.params[k] = v

    '''
        run the thread
    '''
    def run(self):
        log = logging.getLogger(__name__)
        self.setPriority(QThread.HighPriority)
        self.requestStop = False
        self.change_led.emit({'conn_led': 'yellow'})
        log.info('Ranging is initializing...')

        self.dut = RangingPlatform(board=self.params['board'],
                                   ResultFile={'max_entries': 1},
                                   usemaxbaudrate=True)

        self.dut.Timeout = self.params['timeout']

        try:
            self.dut.initialize(port=self.params['serial_port'])
        except:
            log.exception('DUT was not initialized')
            raise

        if self.dut is None or not self.dut.connected:
            self.change_led.emit({'conn_led': 'red'})
            log.warning('Connection to the %s platform has failed.' % self.params['board'])
            self.close_thread()
            return

        self.dut.set_antenna_mode(init=self.params['antinit'], refl=self.params['antrefl'])

        remotes = self.params['anchor_list'].copy()
        if self.params['init_brd'] != self.dut.BoardNumber and not self.params['init_brd'] in remotes:
            remotes.append(self.params['init_brd'])
        if self.dut.BoardNumber in remotes:
            remotes.remove(self.dut.BoardNumber)
        try:
            self.dut.set_remote_address(list(map(self.dut.boardnumber2address, remotes)))
        except ValueError:
            self.change_led.emit({'conn_led': 'red'})
            log.warning('Failed while configuring remote board list.')
            self.close_thread()
            return

        # Apply connection roles, will start connection activity
        time.sleep(0.1)  # Give some time to FW to process the configuration
        self.dut.set_role('Initiator')

        self.dut.store_parameters()

        if not self.dut.wait_on_connection():
            self.change_led.emit({'conn_led': 'red'})
            log.warning('No remote device is detected. Please check Remote Board Number and its configuration.')
            self.close_thread()
            return

        self.dut.enable_raw_data(0)
        # self.dut.write_configuration(['param tx_pwr 4'])

        self.change_led.emit({'conn_led': 'green'})
        log.info('Ranging starting..')

        board_lst = self.params['anchor_list']

        while not self.requestStop:
            for b, boardnumber in enumerate(board_lst):
                self.dut.set_measurement_mode(self.params['measurement_mode'], refl_boardnumber=boardnumber, init_boardnumber=self.params['init_brd'])

                try:
                    results = self.dut.run_range_measurement()
                except:
                    log.exception('Run_range_measurement failed.', exc_info=True)
                    results = []

                try:
                    self.datachange.emit(results)
                    self.change_led.emit({'conn_led': 'green'})
                except:
                    self.change_led.emit({'all': 'orange'})
                    log.warning('Ranging set-up lost connection.')

                if self.requestStop:
                    log.info('Ranging is stopping..')
                    self.change_led.emit({'all': 'grey'})
                    self.close_thread()
                    return

    def close_thread(self):
        if self.dut is not None:
            self.dut.ifc.close()
            self.dut = None

    '''
        stop in a controlled manner
    '''
    def stop(self):
        self.requestStop = True
        while self.isRunning():
            pass
