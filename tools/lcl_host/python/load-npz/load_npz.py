import sys
import os
import re
from pathlib import Path
import glob
import logging.config
import argparse
import traceback
from importlib import import_module

import math
import numpy as np
import json
from json import JSONEncoder

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', 'Instruments'))
sys.path.append(os.path.join(BaseDir, '..', 'Generic'))
logging.config.fileConfig(os.path.join(BaseDir, '..', 'logging.ini'))

def get_npz_file_path(path):
    parsed_files = [os.path.join(root, file) for root, dirs, files in os.walk(path) for file in files if file.endswith(".npz")]
    file_path = ""
    if len(parsed_files) == 0:
        print("No npz file in current folder or sub folder.")
        exit()
    elif len(parsed_files) == 1:
        file_path = parsed_files[0]
    else:
        print(f"\nFind {len(parsed_files)} nzp files in {os.path.abspath(path)}:\n")
        for i in range(len(parsed_files)):
            print(f"[{i}]: " + os.path.basename(parsed_files[i]))
        file_index = input("Please select the npz file index (type 'a' to select all):")
        if (file_index == 'a'):
            file_path = parsed_files
        else:
            if int(file_index) < len(parsed_files):
                file_path = [parsed_files[int(file_index)]]
            else:
                print("Out of index.")
                exit()

    return file_path
    
def load_npz_file(npz_file):
    results = np.load(npz_file, allow_pickle = True)
    results = results['resultfile']
    # print(results[0])
    snapshot_len = results[0].get('mciq.cfg.n_stp', 79)#79
    antenna_path = results[0].get('mciq.cfg.n_ap', 1)
    IQ_store_buff = ""
    TQI_store_buff = ''
    SCAL_FACT = 1.0 / 2048.0

    for result in results:
        has_mciq_info = result.get('meta.has_mciq', False)
        is_error = result['meta.error_msg'] != ''

        if not is_error and has_mciq_info:
            iq_channels = result.get('hadm.stp.channels', bytearray())
            mciq_idx = result.get('hadm.stp.mciq_idx', [])
            mciq_channels = []
            for idx in mciq_idx:
                mciq_channels.append(iq_channels[idx])

            initiator_i = result.get('mciq.initiator.i', []) * SCAL_FACT
            initiator_q = result.get('mciq.initiator.q', []) * SCAL_FACT

            reflector_i = result.get('mciq.reflector.i', []) * SCAL_FACT
            reflector_q = result.get('mciq.reflector.q', []) * SCAL_FACT

            initiator_tqi = result.get('mciq.initiator.tqi', [])
            reflector_tqi = result.get('mciq.reflector.tqi', [])

            for n_ap in range(antenna_path):
                initiator_iq = ''
                reflector_iq = ''

                initiator_tqi_tmp = ''
                reflector_tqi_tmp = ''
                for i in range(snapshot_len):
                    loc = mciq_channels.index(i)
                    initiator_iq += str(initiator_q[loc][n_ap]).strip('[]') + ' '
                    initiator_iq += str(initiator_i[loc][n_ap]).strip('[]') + ' '
                    initiator_tqi_tmp += str(initiator_tqi[loc][n_ap]).strip('[]') + ' '

                for i in range(snapshot_len):
                    loc = mciq_channels.index(i)
                    reflector_iq += str(reflector_q[loc][n_ap]).strip('[]') + ' '
                    reflector_iq += str(reflector_i[loc][n_ap]).strip('[]') + ' '
                    reflector_tqi_tmp += str(reflector_tqi[loc][n_ap]).strip('[]') + ' '

                IQ_store_buff += initiator_iq + reflector_iq
                TQI_store_buff += initiator_tqi_tmp + reflector_tqi_tmp

    print("Output log file in " + os.path.dirname(npz_file))
    with open(os.path.splitext(npz_file)[0] + "_IQ.txt", 'w') as IQ_capture_file:
        IQ_capture_file.write(IQ_store_buff)
        IQ_capture_file.close()

    with open(os.path.splitext(npz_file)[0] + "_TQI.txt", 'w') as TQI_file:
        TQI_file.write(TQI_store_buff)
        TQI_file.close()

## Main funciton 
npz_file = ""
npz_FOLDER = 'records'
if len(sys.argv) == 1:
    npz_file = get_npz_file_path(os.path.join(os.getcwd(),npz_FOLDER))
elif len(sys.argv) == 2:
    npz_file = sys.argv[1]
else:
    print("Arguments error.")
    exit(0)
# Load npz file(s)
for i in range(len(npz_file)):
    print(npz_file[i])
    load_npz_file(npz_file[i])