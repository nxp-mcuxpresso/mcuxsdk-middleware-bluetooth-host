#------------------------------------------------------------------------------
#  Copyright 2025 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
import serial
import serial.tools.list_ports
import sys, time
import threading
import queue
import multiprocessing

class DUTConfig:
    def __init__(self, specified_port=None, baudrate=115200, debug=0):
        self.port = None
        self.serial_handle = None
        self.baudrate = baudrate
        self.debug = debug
        self.dut_response = {'marker': '[DONE]'}
        self.select_port(specified_port)
        self.FrameReceivedEvent = threading.Event()
        self.SerialTimeoutEvent = threading.Event()
        
    def close(self):
        if self.serial_handle and self.serial_handle.is_open:
            self.serial_handle.close()
            print(f"Closed port: {self.port}")
            self.stop_read_thread()
        else:
            print("Serial port is not open or already closed.")

    def list_ports(self):
        # List all available serial ports
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def select_port(self, specified_port=None):
        if specified_port:
            self.port = specified_port
        else:
            ports = self.list_ports()
            if not ports:
                print("No serial ports detected.")
                sys.exit(1)

            print("Available serial ports:")
            for i, port in enumerate(ports):
                print(f"[{i + 1}]: {port}")

            while True:
                try:
                    choice = int(input("Please select the DUT serial port by number: ")) - 1
                    if 0 <= choice < len(ports):
                        self.port = ports[choice]
                        break
                    else:
                        print("Invalid selection. Please enter a valid number.")
                except ValueError:
                    print("Invalid input. Please enter a number.")

        # Try to open the selected port
        try:
            self.serial_handle = serial.Serial(self.port, baudrate=self.baudrate, timeout=1) #Timeout setting is irrelevant to serial baudrate but must be larger than duration of a CS procedure steps at embedded side (e.g., 0.8s) considering slow CS parameters and configurtion, e.g., multi-subevent
            print(f"Successfully opened port: {self.port}")
        except serial.SerialException as e:
            print(f"Failed to open port {self.port}: {e}")
            sys.exit(1)

    def write(self, txt):
        if not self.serial_handle.is_open:
            return
        if self.debug == 2:
            print("> ", self.s.port, txt)
        self.serial_handle.write(txt.encode())

    def read(self, expected=None, timeout=None):
        if not self.serial_handle.is_open:
            return None
        if timeout is None:
            timeout = self.serial_handle.timeout
        self.serial_handle.apply_settings({'timeout': timeout})
        if expected is None:
            txt = self.serial_handle.read(2048)
        else:
            txt = self.serial_handle.read_until(expected.encode())
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
        if not self.serial_handle.is_open:
            return None
        if flush:
            self.serial_handle.flush()
            self.serial_handle.reset_input_buffer()
        if timeout is None:
            timeout = self.serial_handle.timeout
        self.serial_handle.apply_settings({'timeout': timeout})
        time.sleep(0.02)
        self.write(msg)
        time.sleep(0.02)
        return self.read(expected=expected, timeout=timeout)
    
    def read_until(self, expected='\n', timeout=0.2):
        return self.read(expected=expected, timeout=timeout)

    def readline(self):
        return self.read(expected='\n')
    
    def dut_cmd_help(self):
        help_ret = self.write_with_response('help\r\n', timeout=1)
        print(help_ret)
    
    def dut_cmd_reset(self):
        reset_ret = self.write_with_response('reset\r\n', timeout=1)
        print(reset_ret)
    
    def dut_cmd_factory_reset(self):
        factory_reset_ret = self.write_with_response('factoryreset\r\n', timeout=1)
        print(factory_reset_ret)
    
    def dut_cmd_tdm(self):
        tdm_ret = self.write_with_response('tdm 0\r\n', expected=self.dut_response['marker'], timeout=1)
        print(tdm_ret)

    def start_read_thread(self):
        """
        Start a read thread that continuously searches for multi-line strings 
        beginning with 'items:items:[' and ending with 'marker:[DONE]'.
        The found strings are returned to other threads via self.read_queue.
        """
        if self.serial_handle is None or not self.serial_handle.is_open:
            print("Serial port is not open. Cannot start read thread.")
            return
        self.read_queue = multiprocessing.Queue()
        self._stop_read_thread = threading.Event()
        self.read_thread = threading.Thread(target=self.read_worker, daemon=True)
        self.read_thread.start()

    def stop_read_thread(self):
        """
        Stop the read thread.
        """
        if hasattr(self, '_stop_read_thread'):
            self._stop_read_thread.set()
            if hasattr(self, 'read_thread'):
                self.read_thread.join()

    def read_worker(self, max_queue_size=10):
        buffer = ""
        collecting = False
        while not self._stop_read_thread.is_set():
            line = self.serial_handle.readline()
            if not line:
                self.SerialTimeoutEvent.set()
                continue
            try:
                line = line.decode(errors='ignore').replace('\r', '').replace('\n', '')
            except Exception:
                continue
            if line.startswith("items:["):
                buffer = line
                collecting = True
            elif collecting:
                buffer += "\n" + line
                if "marker:[DONE]" in line:
                    buffer += "\n" + f"Host_ts:{time.time()}"
                    self.read_queue.put(buffer)
                    if self.read_queue.qsize() >= max_queue_size:
                        self._flush_queue()
                    # print(f"Collected data: ", buffer)
                    buffer = ""
                    collecting = False
                    self.FrameReceivedEvent.set()
    
    def _flush_queue(self):
        while not self.read_queue.empty():
            try:
                self.read_queue.get_nowait()
            except queue.Empty:
                break

    def get_next_items(self, timeout=None):
        """
        Get the next 'items:items:[ ... marker:[DONE]' string from the read thread queue.
        """
        if not self.read_queue.empty():
            try:
                return self.read_queue.get(timeout=timeout)
            except queue.Empty:
                return None
        else:
            return None
    
# Example usage
if __name__ == "__main__":
    # You can pass a port name like 'COM3' or '/dev/ttyUSB0'
    dut = DUTConfig(specified_port=None)  # Replace None with 'COM3' to specify directly
    # dut.dut_cmd_help()
    # dut.dut_cmd_tdm()
    dut.start_read_thread()
    while True:
        items = dut.get_next_items(timeout=2)
        if items is not None:
            print("[test] Received items:", items)
    # dut.close()
