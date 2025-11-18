#------------------------------------------------------------------------------
#  Copyright 2025 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
import numpy as np
import time, os, datetime
import json
import gzip
import threading
import sys
import multiprocessing
import queue
import argparse

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))

from app_localization_pkg.app_localization_dut_config import DUTConfig
from app_localization_pkg.app_localization_data_parser import ResultParser
from app_localization_pkg.app_localization_result_plot import ResultPlot
from nesteddict import NestedDict
from stringutils import str_to_bool

def list_str(values):
    return values.split(',')

class CSDataProcessing:
    def __init__(self, meas_num=1000, trigger_cs_enable=False, target=['json', 'json-gz', 'npz'], algos=[0,1], plotConfig=[0,15,0,100,0,20]):
        self.meas_num = meas_num
        self.trigger_cs_enable = trigger_cs_enable
        self.target=target
        self.algos = algos
        self.plotConfig = plotConfig
        self.data_store_buffer = []
        self.snapshot_len = plotConfig[3]
        self.start_time = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        current_dir = os.getcwd()
        self.result_store_folder = f"{current_dir}\\records"
        self.raw_results = []
        self.meas_cnt = 0

    @staticmethod
    def get_next_items(data_queue, timeout=None): #data_queue can be a multiprocessing queue
        if not data_queue.empty():
            try:
                return data_queue.get(timeout=timeout)
            except multiprocessing.queues.Empty:
                return None
        else:
            return None
        
    @staticmethod
    def compress_json2gz(input_json_path, output_gz_path=None):
        if output_gz_path is None:
            output_gz_path = input_json_path + '.gz'        
        # Read JSON content and compress with GZIP
        with open(input_json_path, 'rb') as f_in:
            with gzip.open(output_gz_path, 'wb') as f_out:
                f_out.write(f_in.read())        
        print("Data file saved!")
        
    def dataProc_worker(self, frame, data_queue, continue_snapshot_event, snapshot_finished_event): #data_queue can be a multiprocessing queue
        try:
            if self.meas_cnt < self.meas_num:
                items = self.get_next_items(data_queue)
                if items is not None:
                    # print("Received items:", items)                             
                    result = self.parser.parse_result(items)
                    print("New frame taken for analysis")
                    # print("Parsed result:", result)
                    if (result != None) and (len(result.__dict__) != 0):
                        self.meas_cnt += 1
                        self.plotter.update_data_to_distance_plotter(result, self.meas_cnt, self.meas_num)
                        self.data_store_buffer.append(RecordCSData.nestdict_to_dict(result))
                        self.raw_results.append(items)
                        # Save the data to files
                        if (self.meas_cnt % self.snapshot_len) == 0 or self.meas_cnt == self.meas_num:
                            data_store_path = os.path.join(self.result_store_folder, self.start_time)
                            os.makedirs(data_store_path, exist_ok=True)
                            if len(self.data_store_buffer) != 0 and ('json' in self.target or 'json-gz' in self.target):
                                json_path = os.path.join(data_store_path, f"LocDataLog_{self.start_time}.json")
                                with open(json_path, 'w', encoding='utf-8') as f:
                                    json.dump(self.data_store_buffer, f, ensure_ascii=False, indent=4)
                                if ('json-gz' in self.target):
                                    gz_path = os.path.join(data_store_path, f"LocDataLog_{self.start_time}.gz")
                                    self.compress_json2gz(json_path, gz_path)
                                if 'json' not in self.target:
                                    try:
                                        os.remove(json_path)
                                    except Exception as e:
                                        print(f"Error deleting LocDataLog_{self.start_time}.json: {e}")
                            if len(self.raw_results) != 0 and ('npz' in self.target):
                                np.savez_compressed(os.path.join(f"{self.result_store_folder}\{self.start_time}\\LocDataLog_{self.start_time}.npz"), raw_results=self.raw_results)
                            if self.meas_cnt == self.meas_num:
                                print("Data log saved. Test completed!")
                            else:
                                continue_snapshot_event.clear()
                                snapshot_finished_event.set()
                                continue_snapshot_event.wait()
        except KeyboardInterrupt:
            print("Stopping data recording.")

    def dataProc_process(self, data_queue, continue_snapshot_event, snapshot_finished_event):
        self.parser = ResultParser()
        self.plotter = ResultPlot(algos=self.algos, plotConfig=self.plotConfig)
        self.plotter.start_distance_plotter(lambda frame: self.dataProc_worker(frame, data_queue, continue_snapshot_event, snapshot_finished_event), 100)
        print("GUI Closed!")
        
class RecordCSData:
    def __init__(self, specified_port=None, debug=0, meas_num=1000, trigger_cs_enable=False, role='reflector', target=['json', 'json-gz','npz'], algos=[0,1], plotConfig=[0,15,0,100,0,20], multi_process=1):
        self.multi_process = multi_process
        if multi_process == 1:
            self.CSDataProc = CSDataProcessing(meas_num=meas_num, trigger_cs_enable=trigger_cs_enable, target=target, algos=algos, plotConfig=plotConfig)
            self.continue_snapshot_event = multiprocessing.Event()
            self.continue_snapshot_event.clear()
            self.snapshot_finished_event = multiprocessing.Event()
            self.snapshot_finished_event.clear()
            self.set_snapshot_event_thread = threading.Thread(target=self.set_next_snapshot_event, daemon=True)
        self.dut = DUTConfig(specified_port=specified_port, baudrate=1000000, debug=debug)
        self.meas_num = meas_num
        self.trigger_cs_enable = trigger_cs_enable
        self.target = target
        self.algos = algos
        self.plotConfig = plotConfig
        if self.trigger_cs_enable:
            self.trigger_cs_proc_thread = threading.Thread(target=self.send_cs_trigger_command, daemon=True)
        self.dut.start_read_thread()
        self.parser = ResultParser()
        self.plotter = ResultPlot(algos=algos, plotConfig=plotConfig)
        self.data_store_buffer = []
        self.snapshot_len = plotConfig[3]
        self.start_time = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        current_dir = os.getcwd()
        self.result_store_folder = f"{current_dir}\\records"
        self.raw_results = []
        self.meas_cnt = 0
        if role == 'initiator':
            self.dut.write("role 0\r\n")
        elif role == 'reflector':
            self.dut.write("role 1\r\n")
        else:
            print('invalid CS role')
        time.sleep(0.1)
        self.dut.write("setnumprocs 0 0x0001\r\n")
        time.sleep(0.1)
        print('setnumprocs to 1')
        self.dut.write("sb\r\n")
        time.sleep(0.1)
        print('Start advertising')

    def send_cs_trigger_command(self):
        while True:
            frame_received = self.dut.FrameReceivedEvent.wait(timeout=0.2) #Limit the time slice of this thread
            if frame_received:
                self.dut.FrameReceivedEvent.clear()
                self.dut.write("tdm 0\r\n") #Send CS procedure triggering command
            else:
                time_out_occured = self.dut.SerialTimeoutEvent.wait(timeout=0)
                if time_out_occured:
                    self.dut.SerialTimeoutEvent.clear()
                    self.dut.write("tdm 0\r\n") #Send CS procedure triggering command

    def set_next_snapshot_event(self):
        while True:
            self.snapshot_finished_event.wait()
            input("One snapshot finished. Press any key to continue...\n")
            self.continue_snapshot_event.set()
            self.snapshot_finished_event.clear()
    @staticmethod
    def nestdict_to_dict(dict_str: NestedDict):
        """
        Convert NestedDict to a standard JSON-serializable dict.
        Returns a Python dict, not a JSON string.
        """
        def convert(obj):
            if isinstance(obj, NestedDict):
                return {k: convert(v) for k, v in obj.items()}
            elif isinstance(obj, dict):
                return {k: convert(v) for k, v in obj.items()}
            elif isinstance(obj, list):
                return [convert(i) for i in obj]
            elif isinstance(obj, np.ndarray):
                return obj.tolist()
            else:
                return obj

        return convert(dict_str)
    
    def process_result(self, *args):
        try:
            if self.meas_cnt < self.meas_num:
                items = self.dut.get_next_items()
                if items is not None:
                    # print("Received items:", items)                
                    result = self.parser.parse_result(items)
                    print("New frame taken for analysis")
                    # print("Parsed result:", result)
                    if (result != None) and (len(result.__dict__) != 0):
                        self.meas_cnt += 1
                        self.plotter.update_data_to_distance_plotter(result, self.meas_cnt, self.meas_num)
                        self.data_store_buffer.append(self.nestdict_to_dict(result))
                        self.raw_results.append(items)
                        # Save the data to files
                        if (self.meas_cnt % self.snapshot_len) == 0 or self.meas_cnt == self.meas_num:
                            os.makedirs(self.result_store_folder, exist_ok=True)
                            if len(self.data_store_buffer) != 0 and ('json' in self.target):
                                with open(os.path.join(f"{self.result_store_folder}\\LocDataLog_{self.start_time}.json"), 'w', encoding='utf-8') as f:
                                    json.dump(self.data_store_buffer, f, ensure_ascii=False, indent=4)
                            if len(self.raw_results) != 0 and ('npz' in self.target):
                                np.savez_compressed(os.path.join(f"{self.result_store_folder}\\LocDataLog_{self.start_time}.npz"), raw_results=self.raw_results)
                            if self.meas_cnt == self.meas_num:
                                print("Data log saved. Test completed!")
                            else:
                                input("One snapshot finished. Press any key to continue...\n")
        except KeyboardInterrupt:
            print("Stopping data recording.")

    def run(self):
        if self.trigger_cs_enable:
            self.trigger_cs_proc_thread.start()
        if self.multi_process == 1:
            self.set_snapshot_event_thread.start()
            locDataProc_process = multiprocessing.Process(target=self.CSDataProc.dataProc_process, args=(self.dut.read_queue, self.continue_snapshot_event, self.snapshot_finished_event))
            locDataProc_process.start()
            locDataProc_process.join() #wait for termination of child process
        else:
            self.plotter.start_distance_plotter(self.process_result)

def main(argv):
    parser = argparse.ArgumentParser(description='Run measurements on RangingPlatform')
    parser.add_argument('-n', action='store', type=int, default=16, 
                        help='number of measurements to execute')
    parser.add_argument('--trigger_cs_enable', action='store', type=str_to_bool, default=True,
                        help='Automatically trigger CS procedures')
    parser.add_argument('--target', action='append', type=str, default=[],
                        help='add one or more output targets (npz, json; default: json)')
    # parser.add_argument('--algos', action='store', type=list_str, default=[0,1],
    #                     help='Algorithms results to illustrate: [CDE, RADE]')
    parser.add_argument('--csrole', action='store', type=str, default="initiator",
                        help='init | refl')
    parser.add_argument('--plot', action='store', type=list_str, default=[1,10,0,200,0,25],
                        help='Plot ranging measurements ([plot_mode, y_lim (m), x-type, snapshot_nb_points,slide_mode,time_lim]. Plot mode: 0:No plotting, 1:Realtime + History block plotting, 2: History block plotting, default: 0, x-type: 0:time index, 1:time in seconds, slide_en: 0:static, 1:sliding display rade_trk, 2: sliding display rade_raw+rade_trk, time_lim: time span of sliding display).')

    args = parser.parse_args(argv)
    # ------------------------------------------------------------------------------------------------------------------
    # Run measurements
    # ------------------------------------------------------------------------------------------------------------------  
    #algos_int = [int(x) for x in args.algos]
    plot_int = [int(x) for x in args.plot]
    recorder = RecordCSData(specified_port=None, meas_num=args.n, trigger_cs_enable=args.trigger_cs_enable, role='initiator', target=args.target, plotConfig=plot_int) #plotConfig: [plot_mode, y_lim (m), x-type, snapshot_nb_points,slide_mode,time_lim], e.g., snapshot: [1,15,0,100,0,20], sliding mode: [1,20,1,50000,1,20]
    recorder.run()    

if __name__ == "__main__":
    # +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # THE MAIN FUNCTIONALITY OF THIS TEST-FUNCTION
    # +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    main(sys.argv[1:])

