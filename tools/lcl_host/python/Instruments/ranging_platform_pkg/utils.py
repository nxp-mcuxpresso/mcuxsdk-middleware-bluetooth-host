# Copyright : Stichting imec Nederland (http://www.imec.nl)
#------------------------------------------------------------------------------
#  Copyright 2020-2024 NXP
#  All rights reserved.
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------

from pydoc import ispath
import sys
import os
import logging.config
import traceback
import itertools
import inspect
import math
from collections import Counter
import numpy as np
import time
from xtimer import xTimer
import copy
import threading
import matplotlib.pyplot as plt
from matplotlib import gridspec
from datetime import datetime
from pathlib import Path
from PIL import Image

if __name__ == '__main__':
    sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', 'Instruments'))
    sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', 'Generic'))

from .RangingPlatform import RangingPlatform
from .resultfile import ResultFile
from nesteddict import NestedDict


def inject_cfo(dut, auxdut, cfo_list):

    xtal_trim = [0, 0]
    if cfo_list[1] == '+':
        trim_device_index = 0
    else:
        trim_device_index = 1
    if cfo_list[0] == 'medium':
        xtal_trim[trim_device_index] = 31  # medium value
    elif cfo_list[0] == 'max':
        xtal_trim[trim_device_index] = 63  # maximum value

    dut.write_configuration(['misc xtal_trim %d' % (xtal_trim[0])])
    auxdut.write_configuration(['misc xtal_trim %d' % (xtal_trim[1])])

def ranging_result_plot_init(meas_array, result, algos, plot_y_lim, plot_x_type, plot_snapshot_nb_points, plot_slide_mode, plot_time_lim):
    global fig, rng_snapshot_plt_ax, rng_plt_ax, rng_plt_bg, fig_height, fig_width, rng_snapshot_plt_cde_ln, rng_snapshot_plt_rade_ln, rng_snapshot_plt_rade_trk_ln, rng_plt_tl, true_rng, result_cnt, snapshot_len, time_lim, slide_mode, n_stp, n_ap, meas_info
    global IQ_str, IQ_file_name, IQ_para_file_name, IQ_res_file_name, y_true_tt, TQI_file_name, chMap_file_name
    global meas_time, meas_time_array, x_time, y_cde, y_rade, y_rade_trk, IQ_store_buff,rssi_i,rssi_r, TQI_store_buff, chMap_store_buff
    global cde_enabled, rade_enabled
    
    result_cnt = 0
    true_rng = 0
    n_stp = result.get('mciq.cfg.n_stp', 79)
    n_ap = result.get('mciq.cfg.n_ap', 1)
    meas_time = 0
    meas_time_array = []
    x_time = []
    y_cde = []
    y_rade = []
    y_rade_trk = []
    y_true_tt = []
    meas_info = []
    IQ_store_buff = ''
    TQI_store_buff = ''
    chMap_store_buff = str(n_stp)+' '
    rssi_i = []
    rssi_r = []
    meas_info = input("Measurement Info?")
    true_rng = input("Please input the actual distance:")
    IQ_file_name = "IQ_" + meas_info + '_' + datetime.now().strftime('%Y%M%D%H%M%S').replace("/", "") + ".txt"
    IQ_para_file_name = "IQ_" + meas_info + '_' + datetime.now().strftime('%Y%M%D%H%M%S').replace("/", "") + "_para.txt"
    IQ_res_file_name = "IQ_" + meas_info + '_' + datetime.now().strftime('%Y%M%D%H%M%S').replace("/", "") + "_res.txt"
    TQI_file_name = "TQI_" + meas_info + '_' + datetime.now().strftime('%Y%M%D%H%M%S').replace("/", "") + ".txt"
    chMap_file_name = "chMap_" + meas_info + '_' + datetime.now().strftime('%Y%M%D%H%M%S').replace("/", "") + ".txt"

    snapshot_len = plot_snapshot_nb_points
    slide_mode = plot_slide_mode
    time_lim = plot_time_lim
    algo_embed_en_mask = algos[1]
    cde_enabled = (algo_embed_en_mask & 0x01) > 0
    rade_enabled = (algo_embed_en_mask & 0x04) > 0

    fig = plt.figure()
    if plot_slide_mode == 0:
        spec = gridspec.GridSpec(ncols=1, nrows=2, height_ratios=[4, 1])
    else:
        spec = gridspec.GridSpec(ncols=1, nrows=2, height_ratios=[102, 1])
    rng_snapshot_plt_ax = fig.add_subplot(spec[0])
    rng_plt_ax = fig.add_subplot(spec[1])
    fig_size_scal = 1.5
    fig_height = fig.get_figheight()*fig_size_scal
    fig_width = fig.get_figwidth()*fig_size_scal
    fig.set_figheight(fig_height)
    fig.set_figwidth(fig_width)
    if plot_slide_mode == 0:
        fig.tight_layout(pad=2, h_pad=4)
    else:
        fig.tight_layout(pad=4, h_pad=4)
        fig.subplots_adjust(bottom=0.01)
    fig.canvas.set_window_title('NXP Channel Sounding')
    x = list(range(0,snapshot_len))
    if plot_x_type == 1:
        ranging_interval = 0.3 #second
        x = [t*ranging_interval for t in x] #0.3
    y = -10 * np.ones(snapshot_len)
    if cde_enabled:
        if slide_mode == 0:
            (rng_snapshot_plt_cde_ln,) = rng_snapshot_plt_ax.plot(x, y, color='#BBE7FC', marker='>', markersize=5, linestyle='none', lw=1, label = 'CDE')
    if rade_enabled:
        if slide_mode != 1:
            (rng_snapshot_plt_rade_ln,) = rng_snapshot_plt_ax.plot(x, y, color='#A9A9A9', marker='v', markersize=5, linestyle='none', lw=1, label = 'RADE-embed-raw')
        if slide_mode == 0:
            (rng_snapshot_plt_rade_trk_ln,) = rng_snapshot_plt_ax.plot(x, y, color='#F36424', marker='o', markersize=7, linestyle='none', lw=2, label = 'RADE-embed-trk')
        else:
            (rng_snapshot_plt_rade_trk_ln,) = rng_snapshot_plt_ax.plot(x, y, color='#F36424', marker='o', markersize=7, lw=2, label = 'RADE-embed-trk')
    if plot_slide_mode == 0:         
        rng_snapshot_plt_ax.set_title('NXP Wireless Ranging', fontsize='14')
    else:
        rng_snapshot_plt_ax.set_title('NXP CS Algorithm RADE on KW47 w/ LCE Acceleration', fontsize='16')
    rng_snapshot_plt_ax.set_ylim([-5,plot_y_lim])
    rng_snapshot_plt_ax.spines['top'].set_linewidth(2)
    rng_snapshot_plt_ax.spines['bottom'].set_linewidth(2)
    if plot_slide_mode > 0:
        rng_snapshot_plt_ax.set_xlim([-plot_time_lim, 0.05]) #Use time as the x-axis
    rng_snapshot_plt_ax.legend(fontsize=16)
    rng_snapshot_plt_ax.set_ylabel("Distance (m)", fontsize = 14)
    rng_snapshot_plt_ax.grid(b=True, color='#A9A9A9')
    rng_snapshot_plt_ax.minorticks_on()
    rng_snapshot_plt_ax.grid(b=True, which='minor', color='#DCDCDC')
    if plot_x_type == 0:
        rng_snapshot_plt_ax.set_xlabel("Time Index",  x = 0, ha='left', fontsize = 14)
    else:
        rng_snapshot_plt_ax.set_xlabel("Time (s)",  x = 0, ha='left', fontsize = 14)
    rng_plt_tl = rng_plt_ax.set_title('')
    rng_plt_tl.set_color('#203F99')
    rng_plt_tl.set_fontsize(52)
    rng_plt_tl.set_weight('extra bold')
    if plot_slide_mode == 0:
        rng_plt_ax.set_ylabel("Distance (m)", fontsize = 14)
    rng_plt_ax.grid(b=True, color='#A9A9A9')
    rng_plt_ax.minorticks_on()
    rng_plt_ax.grid(b=True, which='minor', color='#DCDCDC')
    if plot_slide_mode > 0:
        if getattr(sys, 'frozen', False): # Bundled case
            nxp_logo_base_path = Path(sys._MEIPASS)
            logo_path = nxp_logo_base_path / 'ranging_platform_pkg' / 'nxp_logo.png' #ranging_platform_pkg is target path set in spec file
        else:
            nxp_logo_base_path = Path(__file__).parent
            logo_path = (nxp_logo_base_path / 'nxp_logo.png').resolve()
        logo = plt.imread(logo_path)
        height, width = logo.shape[0], logo.shape[1]
        new_width = int(width * 0.2)
        new_height = int(height * 0.2)
        logo_pil = Image.fromarray((logo * 255).astype('uint8'))  # Convert to PIL Image
        nxp_logo = logo_pil.resize((new_width, new_height))
        logo_ax = rng_snapshot_plt_ax.inset_axes([0.83, 0.00, 0.16, 0.16])  # [x0, y0, width, height]
        logo_ax.imshow(nxp_logo, alpha=0.7, zorder=10)
        logo_ax.axis('off')
        rng_plt_ax.set_visible(False)
    plt.show(block=False)
    plt.pause(0.01)
    rng_plt_bg = fig.canvas.copy_from_bbox(fig.bbox)

def ranging_all_result_plot(meas_array, meas_time_array, plot_x_type): 
    rng_plt_ax.cla()
    if plot_x_type == 1:
        meas_time_array_centered = [t - meas_time_array[0] for t in meas_time_array]
        if cde_enabled:
            rng_plt_ax.plot(meas_time_array_centered, meas_array[0]["cde_array"], color='#BBE7FC', marker='>', markersize=5, linestyle='none', lw=1, label = 'CDE')
        if rade_enabled:
            rng_plt_ax.plot(meas_time_array_centered, meas_array[0]["rade_array"], color='#A9A9A9', marker='v', markersize=5, linestyle='none', lw=1, label = 'RADE-raw')
            rng_plt_ax.plot(meas_time_array_centered, meas_array[0]["rade_trk_array"], color='#F36424', marker='o', markersize=5, linestyle='none', lw=1, label = 'RADE-trk')
        if true_rng.replace(".", "", 1).isdigit():
            rng_plt_ax.plot(meas_time_array_centered, y_true_tt, 'k', lw=1, label = 'True')
        rng_plt_ax.legend(fontsize=50)
    else:
        if cde_enabled:
            rng_plt_ax.plot(meas_array[0]["cde_array"], color='#BBE7FC', marker='>', markersize=5, linestyle='none', lw=1, label = 'CDE')
        if rade_enabled:
            rng_plt_ax.plot(meas_array[0]["rade_array"], color='#A9A9A9', marker='v', markersize=5, linestyle='none', lw=1, label = 'RADE-raw')
            rng_plt_ax.plot(meas_array[0]["rade_trk_array"], color='#F36424', marker='o', markersize=5, linestyle='none', lw=1, label = 'RADE-trk')
        if true_rng.replace(".", "", 1).isdigit():
            rng_plt_ax.plot(y_true_tt, 'k', lw=1, label = 'True')
    rng_plt_ax.set_ylim([-1, y_mean * 3])
    rng_plt_ax.set_ylabel("Distance (m)", fontsize = 14)
    rng_plt_ax.legend()
    rng_plt_ax.grid(b=True, color='#A9A9A9')
    rng_plt_ax.minorticks_on()
    rng_plt_ax.grid(b=True, which='minor', color='#DCDCDC')
    rng_plt_tl.set_text('')
    rng_plt_tl.set_color('#203F99')
    rng_plt_tl.set_fontsize(52)
    rng_plt_tl.set_weight('extra bold')    
    plt.pause(0.01)

def ranging_result_plot(meas_array, result, execAlgo, plot_mode, plot_x_type, log_path, mease_cnt, meas_num):
    global rng_plt_bg, fig_height, fig_width, rng_plt_tl, true_rng, result_cnt
    global meas_time, meas_time_array, y_true_tt, x_time, y_cde, y_rade, y_rade_trk, y_mean, IQ_store_buff, rssi_i, rssi_r, TQI_store_buff, chMap_store_buff

    meas_time = meas_time + result['meta.profiling.t_total']# second
    null_RNG = -1.23 #Two decimal digits printed
    if cde_enabled and math.isnan(result.get('mciq.result.CDE_distance', np.nan)):
        return
    if rade_enabled and math.isnan(result.get('mciq.result.RADE', np.nan)):
        return                        
    if cde_enabled:
        y_cde.append(meas_array[0]["cde_array"][-1])
    if rade_enabled:
        y_rade.append(meas_array[0]["rade_array"][-1])
        y_rade_trk.append(meas_array[0]["rade_trk_array"][-1])
    meas_time_array.append(meas_time)
    if cde_enabled or rade_enabled:
        x_time.append(meas_time_array[-1])
    rssi_i.append(result.get('info.init.sync_rssi', 0))
    rssi_r.append(result.get('info.refl.sync_rssi', 0))
    
    if plot_mode == 1:
        fig_height_n = fig.get_figheight() 
        fig_width_n = fig.get_figwidth()
        if (fig_height != fig_height_n or fig_width != fig_width_n):
            rng_plt_tl.set_text('')
            rng_plt_ax.draw_artist(rng_plt_tl)
            y_snpsht_tmp = -10 * np.ones(snapshot_len)
            if cde_enabled: 
                if slide_mode == 0:        
                    rng_snapshot_plt_cde_ln.set_ydata(y_snpsht_tmp) 
                    rng_snapshot_plt_ax.draw_artist(rng_snapshot_plt_cde_ln)
            if rade_enabled:
                rng_snapshot_plt_rade_trk_ln.set_ydata(y_snpsht_tmp)
                if slide_mode != 1:
                    rng_snapshot_plt_rade_ln.set_ydata(y_snpsht_tmp)
                    rng_snapshot_plt_ax.draw_artist(rng_snapshot_plt_rade_ln)
                rng_snapshot_plt_ax.draw_artist(rng_snapshot_plt_rade_trk_ln)
            plt.pause(0.01)
            rng_plt_bg = fig.canvas.copy_from_bbox(fig.bbox)
            fig_height = fig_height_n
            fig_width = fig_width_n
        else:
            fig.canvas.restore_region(rng_plt_bg)
        if cde_enabled or rade_enabled:
            x_time_centered = [t - x_time[0] for t in x_time]
            x_time_snpsht = np.concatenate((x_time_centered,1000 * np.ones(snapshot_len - len(y_cde))),axis = 0)
        if rade_enabled:
            if slide_mode == 0:
                y_rade_snpsht = np.concatenate((y_rade,-10 * np.ones(snapshot_len - len(y_rade))),axis = 0)
                y_rade_trk_snpsht = np.concatenate((y_rade_trk,-10 * np.ones(snapshot_len - len(y_rade_trk))),axis = 0)
                rng_snapshot_plt_rade_ln.set_ydata(y_rade_snpsht) 
                rng_snapshot_plt_rade_trk_ln.set_ydata(y_rade_trk_snpsht) 
                if plot_x_type == 1:
                    rng_snapshot_plt_rade_ln.set_xdata(x_time_snpsht) #Use time as the x-axis
                    rng_snapshot_plt_rade_trk_ln.set_xdata(x_time_snpsht)
            else:
                valid_time_indices = np.where(np.array(x_time_centered) > (x_time_centered[-1] - time_lim))[0]
                y_rade_array = np.array(y_rade)
                absTol = 1e-1
                diff = np.abs(y_rade_array[valid_time_indices] - null_RNG)
                non_null_RNG_indices = np.where(diff > absTol)[0]
                valid_time_indices = [valid_time_indices[i] for i in non_null_RNG_indices]
                y_rade_valid0 = [y_rade[i] for i in valid_time_indices]
                y_rade_trk_valid0 = [y_rade_trk[i] for i in valid_time_indices]
                x_time_valid = [x_time_centered[i]-x_time_centered[-1] for i in valid_time_indices] 
                y_rade_valid = np.concatenate((y_rade_valid0,-10 * np.ones(snapshot_len - len(y_rade_valid0))),axis = 0)
                y_rade_trk_valid = np.concatenate((y_rade_trk_valid0,-10 * np.ones(snapshot_len - len(y_rade_trk_valid0))),axis = 0)
                x_time_valid = np.concatenate((x_time_valid,1000 * np.ones(snapshot_len - len(x_time_valid))),axis = 0)
                rng_snapshot_plt_rade_trk_ln.set_ydata(y_rade_trk_valid) 
                rng_snapshot_plt_rade_trk_ln.set_xdata(x_time_valid)
                if slide_mode != 1:
                    rng_snapshot_plt_rade_ln.set_ydata(y_rade_valid) 
                    rng_snapshot_plt_rade_ln.set_xdata(x_time_valid) #Use time as the x-axis

        if cde_enabled:
            if slide_mode == 0:
                y_cde_snpsht = np.concatenate((y_cde,-10 * np.ones(snapshot_len - len(y_cde))),axis = 0)
                rng_snapshot_plt_cde_ln.set_ydata(y_cde_snpsht)
                if plot_x_type == 1:
                    rng_snapshot_plt_cde_ln.set_xdata(x_time_snpsht) #Use time as the x-axis
                rng_snapshot_plt_ax.draw_artist(rng_snapshot_plt_cde_ln)

        if rade_enabled:
            if slide_mode != 1:
                rng_snapshot_plt_ax.draw_artist(rng_snapshot_plt_rade_ln)
            rng_snapshot_plt_ax.draw_artist(rng_snapshot_plt_rade_trk_ln)

        rng_snapshot_plt_title = ''
        if rade_enabled:
            if slide_mode == 0:
                rng_snapshot_plt_title += f'{meas_array[0]["rade_array"][-1]}m' 
            else:
                rng_snapshot_plt_title += f'{y_rade_trk_valid0[-1]}m'  
        else:
            if cde_enabled: 
                rng_snapshot_plt_title += f'{meas_array[0]["cde_array"][-1]}m'
        rng_plt_tl.set_text(rng_snapshot_plt_title)
        rng_plt_ax.draw_artist(rng_plt_tl)

    y_cde_mean = 0 if len(meas_array[0]["cde_array"]) == 0 or math.isnan(np.mean(meas_array[0]["cde_array"])) else np.mean(meas_array[0]["cde_array"])
    y_rade_mean = 0 if len(meas_array[0]["rade_array"]) == 0 or len(meas_array[0]["rade_array"]) == 0 else np.mean(meas_array[0]["rade_array"])
    y_mean = max([y_cde_mean] + [y_rade_mean])
    y_mean = max(y_mean,2)
    if plot_mode == 1:
        # # Adaptive plot backgound
        # rng_snapshot_plt_ax.set_ylim([-1, y_mean * 3])
        # plt.pause(0.01)
        # # Static plot backgound
        fig.canvas.blit(fig.bbox)
        fig.canvas.flush_events()

    if execAlgo == True:
        iq_channels = result.get('hadm.stp.channels', bytearray())
        mciq_idx = result.get('hadm.stp.mciq_idx', [])
        # print(iq_channels)
        mciq_channels = []
        for idx in mciq_idx:
            mciq_channels.append(iq_channels[idx])

        fi2fl_scal = 1024.0
        initiator_i = result.get('mciq.initiator.i', []) / fi2fl_scal
        initiator_q = result.get('mciq.initiator.q', []) / fi2fl_scal
    
        reflector_i = result.get('mciq.reflector.i', []) / fi2fl_scal
        reflector_q = result.get('mciq.reflector.q', []) / fi2fl_scal

        initiator_tqi = result.get('mciq.initiator.tqi', [])
        reflector_tqi = result.get('mciq.reflector.tqi', [])
        # print(initiator_i)

        for ap_idx in range(n_ap):
            initiator_iq = ''
            reflector_iq = ''

            initiator_tqi_tmp = ''
            reflector_tqi_tmp = ''
            #initiator_iq = 'Hi data: '
            #initiator_iq += "\r\n" + str(n_ap) + " line data:\r\n"
            for i in range(79):
                if i in mciq_channels:
                    loc = mciq_channels.index(i)
                    initiator_iq += str(initiator_q[loc][ap_idx]).strip('[]') + ' '
                    initiator_iq += str(initiator_i[loc][ap_idx]).strip('[]') + ' '
                    initiator_tqi_tmp += str(initiator_tqi[loc][ap_idx]).strip('[]') + ' '
                else:
                    initiator_iq += '0.0 '
                    initiator_iq += '0.0 '
                    initiator_tqi_tmp += '3 '
            #initiator_iq += '\r\n'
            for i in range(79):
                if i in mciq_channels:
                    loc = mciq_channels.index(i)
                    reflector_iq += str(reflector_q[loc][ap_idx]).strip('[]') + ' '
                    reflector_iq += str(reflector_i[loc][ap_idx]).strip('[]') + ' '
                    reflector_tqi_tmp += str(reflector_tqi[loc][ap_idx]).strip('[]') + ' '
                else:
                    reflector_iq += '0.0 '
                    reflector_iq += '0.0 '
                    reflector_tqi_tmp += '3 '

            IQ_store_buff += initiator_iq + reflector_iq
            TQI_store_buff += initiator_tqi_tmp + reflector_tqi_tmp
            # print(initiator_iq)

        for i in range(result.get('mciq.cfg.n_stp', 79)):
            chMap_store_buff += str(mciq_channels[i]) + ' '                                  
        
    result_cnt += 1
    if true_rng.replace(".", "", 1).isdigit():
        y_true_tt.append(float(true_rng))
    if (result_cnt % snapshot_len) == 0 or (mease_cnt + 1) == meas_num:
        ranging_all_result_plot(meas_array, meas_time_array, plot_x_type)
        rng_plt_tl = rng_plt_ax.set_title('History Results')
        plt.pause(0.01)
        log_path += "IQ_res\\"
        if not os.path.isdir(log_path):
            os.mkdir(log_path)
        with open(log_path + IQ_file_name, 'a') as IQ_capture_file:
            IQ_capture_file.write(IQ_store_buff)
            IQ_capture_file.close()
        with open(log_path + IQ_para_file_name, 'a') as IQ_cap_para_file:
            IQ_cap_para_file.write(f'{n_stp} {snapshot_len if (result_cnt % snapshot_len) == 0 else (result_cnt % snapshot_len)} {true_rng} {n_ap}\n')
            IQ_cap_para_file.close()
        with open(log_path + IQ_res_file_name, 'a') as IQ_cap_res_file:
            #IQ_cap_res_file.write(str(y_rade).strip("[]").replace(",", "") + " ")
            #IQ_cap_res_file.write("TRUE RADE RADE_TRK MCIQ CDE\n")
            for i in range(max([len(y_rade), len(y_rade_trk), len(y_cde), len(rssi_i), len(rssi_r)])):
                IQ_cap_res_buff = str(true_rng) + " "
                if rade_enabled:
                    IQ_cap_res_buff += str(y_rade[i])  + " "
                else:
                    IQ_cap_res_buff += str(0)  + " "
                if i < len(y_rade_trk):
                    IQ_cap_res_buff += str(y_rade_trk[i])  + " "
                else:
                    IQ_cap_res_buff += str(0)  + " "
                if cde_enabled:
                    IQ_cap_res_buff += str(np.min(y_cde[i]))  + " "
                else:
                    IQ_cap_res_buff += str(0)  + " "
                
                IQ_cap_res_buff += str(rssi_i[i]) + " " + str(rssi_r[i]) + "\n"
                IQ_cap_res_file.write(IQ_cap_res_buff)
            IQ_cap_res_file.close()
            
        with open(log_path + TQI_file_name, 'a') as TQI_file:
            TQI_file.write(TQI_store_buff)
            TQI_file.close()
            
        with open(log_path + chMap_file_name, 'a') as chMap_file:
            chMap_file.write(chMap_store_buff)
            chMap_file.close()    

        #print("measurement count:" + str(mease_cnt))
        if (mease_cnt + 1) != meas_num:
            true_rng = input("Please input the actual distance:")
            #Update figure backgournd
            fig.canvas.restore_region(rng_plt_bg)
            y = -10 * np.ones(snapshot_len)
            if cde_enabled:
                if plot_x_type == 1:
                    rng_snapshot_plt_cde_ln.set_xdata(x_time_snpsht) #Use time as the x-axis
                rng_snapshot_plt_cde_ln.set_ydata(y) 
            if rade_enabled:
                if plot_x_type == 1:
                    rng_snapshot_plt_rade_ln.set_xdata(x_time_snpsht) #Use time as the x-axis
                rng_snapshot_plt_rade_ln.set_ydata(y) 
                rng_snapshot_plt_rade_trk_ln.set_ydata(y)
            fig.canvas.blit(fig.bbox)
            fig.canvas.flush_events()
            ranging_all_result_plot(meas_array, meas_time_array, plot_x_type)
            #Save figure background
            rng_plt_bg = fig.canvas.copy_from_bbox(fig.bbox) 
        else:
            plt.show()

        IQ_store_buff = ''
        TQI_store_buff = ''
        chMap_store_buff = ''
        x_time = []
        y_cde = []
        y_rade = []
        y_rade_trk = []
        rssi_i = []
        rssi_r = []
    
def populate_distance_results(meas_array, board_pair, mciq_dist, tof_dist, rade_dist, rade_trk_dist, cde_dist):
    found_pair = 0

    for meas in meas_array:
        if meas['pair'] == board_pair:
            found_pair = 1
            break

    if found_pair == 0:
        meas = {
            "pair": board_pair,
            "mciq_array": [],
            "tof_array": [],
            "rade_array": [],
            "rade_trk_array": [],
            "cde_array": [],
            "mciq_avg": np.nan,
            "tof_avg": np.nan,
            "rade_avg": np.nan,
            "cde_avg": np.nan,
            "mciq_stdev": np.nan,
            "tof_stdev": np.nan,
            "rade_stdev": np.nan,
            "cde_stdev": np.nan,
        }
        meas_array.append(meas)

    if not math.isnan(mciq_dist):
        meas['mciq_array'].append(mciq_dist)
    if not math.isnan(tof_dist):
        meas['tof_array'].append(tof_dist)
    if not math.isnan(rade_dist):
        meas['rade_array'].append(rade_dist)
    if not math.isnan(rade_trk_dist):
        meas['rade_trk_array'].append(rade_trk_dist)
    if not math.isnan(cde_dist):
        meas['cde_array'].append(cde_dist)


# board: platform type (e.g. 'kw36', 'at86rf215')
# initiator/reflector: node dictionary
# - key:comport (optional ('COMdd', '/dev/ttyACMdd', 'none' or 'auto'), automatic otherwise)
# - key:comserial (optional, any otherwise)
# - key:unique_id (optional, automatic otherwise)
# - key:configuration (optional, default otherwise)
def platform_connect(board, *args, **kwargs):
    duts = {'dut': None, 'peer': None, }
    log = logging.getLogger(__name__)

    cominfo = []
    serial_lst = []
    for n, arg in enumerate(args):
        if not isinstance(arg, dict):
            raise Exception('argument must be a dictionary')
        arg.setdefault('comport', 'auto')
        arg.setdefault('unique_id', 'auto')
        arg.setdefault('configuration', None)
        arg.setdefault('Name', 'dut' if n == 0 else 'peer')
        arg.setdefault('Connected', False)
        arg.setdefault('Message', 'No serial-port found')
        if arg.get('comport') != 'none':
            cominfo.append(arg)
        if arg.get('comserial'):
            serial_lst.append(arg.get('comserial'))

    if len(cominfo) == 0:
        msg = 'Configuration error, nothing to connect..'
        log.error(msg)
        raise Exception(msg)

    xdut = RangingPlatform(board=board, debug=kwargs.get('debug', 0),
                           usemaxbaudrate=kwargs.get('usemaxbaudrate', True), cal=kwargs.get('cal'), log=log)
    xdut.DefaultConfig.append(f"system verbosity {kwargs.get('verbosity', 0)}")

    port_lst = xdut.get_serial_ports(serial_lst=serial_lst)
    for port in port_lst:
        if all([com.get('Connected') for com in cominfo]):
            break
        if xdut is None:
            xdut = RangingPlatform(board=board, debug=kwargs.get('debug', 0),
                                   usemaxbaudrate=kwargs.get('usemaxbaudrate', True), cal=kwargs.get('cal'), log=log)

        # find matching com in cominfo instead of searching connected port com
        cominfo_portlist = [c.get('comport') for c in cominfo]
        com = None
        if port in cominfo_portlist:
            com = cominfo[cominfo_portlist.index(port)]
        else:
            # else get the first not connected cominfo element with 'auto' comport
            for c in cominfo:
                if (not c.get('Connected')) and (c.get('comport') == 'auto'):
                    com = c
        # skip current port if no available cominfo
        if com == None:
            continue

        try:
            xdut.connect(port)
        except Exception as e:
            com['Message'] = 'port: %s - %s' % (port, e)
            msg = 'Failed to connect RangingPlatform_%s as %s (%s)!' % (board, com['Name'], com['Message'])
            log.warning(msg)
            continue
        connect = False
        if xdut.UniqueId is not None:
            if com['comport'] == 'auto' and com['unique_id'] == 'auto':
                connect = True
            elif com['comport'] == port and com['unique_id'] == 'auto':
                connect = True
            elif com['comport'] == 'auto' and com['unique_id'] == xdut.UniqueId:
                connect = True
            if connect:
                log.info("Configuring RangingPlatform_%s as %s on port: %s with unique_id: %s" % (board, com['Name'], port, xdut.UniqueId))
                xdut.initialize(defaults=True, role=com['Name'], configuration=com['configuration'])
                xdut.parser.debug = kwargs.get('debug', 0)
                xdut.parser.log = log
                duts[com['Name']] = xdut
                xdut.Name = com['Name']
                com['Connected'] = True
                xdut = None
        if not connect:
            xdut.disconnect()
    # check if all connection are ready
    for com in cominfo:
        if not com.get('Connected'):
            msg = 'Failed to connect RangingPlatform_%s as %s (%s)!' % (board, com['Name'], com['Message'])
            log.error(msg)
            raise Exception(msg)

    return duts

res2 = {}


# Wrapper function for threaded execution
def execute_range_measurement(dut, overlay, results):
        results[0] = dut.run_range_command(overlay=overlay)


# The function has the following arguments:
# filepath      (file location of result files, extension is implicit; if any; default: './result/out)
# targets       (list of stored file types; possible types: 'npz', 'mat', 'txt', 'csv'; default 'npz')
# dut           (RangingPlatform instantiation (configured and connected))
# overlay       (dictionary to overlay on each result dict)
# parameters:   (a list of dictionaries describing what to do for each parameter
#                the first element has the highest priority and is the outer loop. all subsequent parameters
#                have a lower priority then the previous parameter and are nested inside the previous loop)
#
# a parameter dictionary has the following keys:
# - name (a string describing the parameter name; mandatory)
# - range (a list of integers over which the parameter should iterate; mandatory)
# - func (a function, with one integer argument and returning a boolean indicating the success or None if success cannot
#         be determined, that will  be executed to apply the parameter to the system; use None to disable; optional)
# - hidden (if True parameter will not be logged, default False; ie logging enabled)
# - unit (a string describing the unit of the parameter; optional; e.g. 'kHz', 'dB')
# - fmt  (determines how to format the parameter value; optional; e.g. '%d' (default), '%04d')
# - save (determines when results are saved; can be used to save intermediate results; if no
#         parameter has this key all results are stored in one file; highest priority parameter first;
#         optional; e.g. True)
# - log  (determines when result and status is shown in the log; if no parameter has this key all results are shown;
#         highest priority parameter first; optional; e.g. True)
# example: params = [{'name': 'cfo', 'unit': 'kHz', 'func': None, 'range': range(0, 1000, 100)}]
def platform_measurement(parameters, **kwargs):
    log = logging.getLogger(__name__)
    filepath = kwargs.get('filepath', './result/out')
    targets = kwargs.get('targets', ['npz'])
    dut = kwargs.get('dut')
    auxdut = kwargs.get('auxdut', None)
    execAlgo = kwargs.get('execAlgo', True)
    pause = kwargs.get('pause', False)
    rangeCmdTimeout = kwargs.get('rangeCmdTimeout', 4.0)
    plotConfig = kwargs.get('plotConfig', [0, 0, 0])
    plot_mode = int(plotConfig[0])
    plot_y_lim = int(plotConfig[1])
    plot_x_type = int(plotConfig[2])
    plot_snapshot_nb_points = int(plotConfig[3])
    plot_slide_mode = int(plotConfig[4])
    plot_time_lim = int(plotConfig[5])
    algos = kwargs.get('algos', [0, 0])
    overlay = NestedDict(kwargs.get('overlay', {}))
    debug = kwargs.get('debug', 0)
    # Log to file
    fh = logging.FileHandler(filepath + "_main.log", mode='a')
    fh.setLevel(logging.INFO)
    log.addHandler(fh)

    # check if parameters is in the correct format
    for item in parameters:
        item.setdefault('hidden', False)
        if not isinstance(item.get('name'), str):
            raise Exception('name key must be a string')
        if not isinstance(item.get('unit', ''), str):
            raise Exception('unit key must be a string')
        if not isinstance(item.get('save', False), bool):
            raise Exception('save key must be a bool')
        if not isinstance(item.get('log', False), bool):
            raise Exception('fmt key must be a bool')
        if not isinstance(item.get('fmt', ''), str):
            raise Exception('fmt key must be a string')
        if not item.get('fmt', None) is None:
            try:
                item.get('fmt') % 10
            except:
                raise Exception('fmt key must be a valid string format')
        if not isinstance(item.get('range'), list) and not isinstance(item.get('range'), range):  # range is a new type in python 3
            raise Exception('range key must be a list')
        if not isinstance(item.get('range')[0], int):
            raise Exception('range list must consist of integers')
        if not item.get('func', None) is None:
            if not inspect.isfunction(item.get('func')):
                raise Exception('parameter func must be a callable function')
            fnc_args = inspect.getfullargspec(item.get('func')).args
            if 'arg' not in fnc_args or len(fnc_args) != 1:
                raise Exception('parameter func must have one argument called arg')

    # calculate parameter ranges product
    parameters_product = list(itertools.product(*[x.get('range') for x in parameters]))
    cur_val = [None] * len(parameters)
    func_res = [None] * len(parameters)

    log.info('Starting measurement with %d parameters and %d measurements' % (len(parameters), len(parameters_product)))
    log.info('%s:\t%s\t%s' % (dut.ifc.port, dut.bleRole, dut.csrole))
    if (auxdut is not None):
        log.info('%s:\t%s\t%s' % (auxdut.ifc.port, auxdut.bleRole, auxdut.csrole))
    for i, item in enumerate(parameters):
        log.info('- parameter%d - %s has %d values' % (i, item.get('name'), len(item.get('range'))))

    log_level = -1
    save_level = -1
    for i, item in enumerate(parameters):
        if item.get('log', False) and log_level == -1:
            log_level = i
        if item.get('save', False) and save_level == -1:
            save_level = i
    log_modulo = 1
    save_modulo = 1
    for i, item in enumerate(parameters):
        if i > log_level > -1:
            log_modulo = log_modulo * len(item.get('range'))
        if i > save_level or save_level == -1:
            save_modulo = save_modulo * len(item.get('range'))

    all_result_lst = []
    dut.ifc.flush()
    dut.resultfile.clear()

    meas_array = []

    for n, parameter_values in enumerate(parameters_product):
        log_lst = []
        save_lst = []

        for i, item in enumerate(parameters):
            # determine if we need to execute a function to apply the parameter
            parameter_postfix = ''
            if parameter_values[i] != cur_val[i]:
                if not item.get('func') is None:
                    try:
                        parameter_postfix = '*'
                        func_res[i] = item.get('func')(parameter_values[i])
                    except:
                        raise Exception('Failed executing parameter function[%d]' % i)
            cur_val[i] = parameter_values[i]
            if len(item.get('range')) <= 0 or item.get('hidden'):  # do not log parameters with 0 value
                continue
            if log_level >= i or log_level == -1:
                log_lst.append('%s%s = %s%s' % (item.get('name'), parameter_postfix,
                                                item.get('fmt', '%d') % (cur_val[i]), item.get('unit', '')))
            if save_level >= i:
                save_lst.append('%s=%s' % (item.get('name'), item.get('fmt', '%d') % (cur_val[i])))

        for i, item in enumerate(parameters):
            if item.get('hidden'):
                continue
            overlay['meta.testcase.%s' % item.get('name')] = parameter_values[i]

        # Actual measurement trigger
        task_tmr = xTimer(average=False, logger=None)
        auxresults = [None,]
        th = None
        if auxdut is not None:
            th = threading.Thread(target=execute_range_measurement, args=(auxdut, overlay, auxresults))
            th.start()
            time.sleep(0.2)
        results = dut.run_range_command(overlay=overlay, timeout=rangeCmdTimeout, pause=pause)
        # time.sleep(1)
        if th is not None:
            th.join()
        task_tmr.checkpoint('t_measurement')

        # Combine auxiliary (reflector) dut results with main dut results
        if auxdut is not None:
            auxresults = auxresults[0]
            for result, auxresult in zip(results, auxresults):
                result["hidden_refl"] = auxresult["hidden"]
                # Merge error_msg if some error happened on reflector
                if auxresult['meta.error_msg'] != '' and result['meta.error_msg'] == '':
                    result['meta.error_msg'] = auxresult['meta.error_msg']
                for hierarchy in list(itertools.product(['meta', 'info', 'md0', 'tof', 'mciq', 'dbg'], ['reflector', 'refl'])):
                    key = '.'.join(hierarchy)
                    if key in auxresult:
                        result[key] = auxresult[key]
            # Check HADM config is the same on both devices...
            if result.get('hadm.cfg', False) and auxresult.get('hadm.cfg', False):
                if auxresult['hadm.cfg'] != result['hadm.cfg']:
                    result['meta.error_msg'] = 'hadmcfgmismatch'

        # Process measurements results
        results = dut.process_range_result(task_tmr, results=results, overlay=overlay, execAlgo=execAlgo)

        # Print Results
        if True:
            for result in results:
                try:
                    if dut.diagnose:
                        dut.compute_diagnose_data(result)

                    duration = int((result['meta.profiling.t_total'] + 0.001) * 1e3)
                    has_mciq_info = result.get('meta.has_mciq', False)
                    has_tof_info = result.get('meta.has_tof', False)
                    is_error = result['meta.error_msg'] != ''
                    board_pair = (result['meta.initiator.board_nr'], result['meta.reflector.board_nr'])
                    tof_distance = result.get('tof.result.distance', np.nan)
                    slope_distance = result.get('mciq.result.distance', np.nan)
                    rade_distance = result.get('mciq.result.RADE', np.nan)
                    rade_trk_distance = result.get('mciq.result.RADE_trk', np.nan)
                    cde_distance = result.get('mciq.result.CDE_distance', np.nan)
                    rssi_init = result.get('info.init.sync_rssi', np.nan)
                    rssi_refl = result.get('info.refl.sync_rssi', np.nan)
                    cfo = result.get('info.init.sync_cfo', np.nan)
                    if cfo is not np.nan:
                        cfo = cfo / 1000


                    if not has_mciq_info and not has_tof_info:
                        is_error = True

                    if is_error :
                        dut.ifc.flush()
                        dut.wait_on_connection()
                        rmsg = ' => Failure (%s)' % result['meta.error_msg']
                    else:
                        rtp_distance = np.nan
                        rmsg = ' =>'
                        if not math.isnan(cde_distance):
                            rmsg += ' CDE-distance = %5.2fm' % (cde_distance)
                            rtp_distance = cde_distance
                        elif not math.isnan(slope_distance):
                            rmsg += ' slope-distance = %5.2fm' % (slope_distance)
                            rtp_distance = slope_distance
                        if not math.isnan(rade_distance):
                            rmsg += ' RADE-distance = %5.2fm' % rade_distance
                            rtp_distance = rade_distance
                        if not math.isnan(rade_trk_distance):
                            rmsg += ' RADE-TRK-distance = %5.2fm' % rade_trk_distance
                            rtp_distance = rade_trk_distance
                        if not math.isnan(tof_distance):
                            rmsg += ' RTT-distance = %5.2fm (%3.0f%%)' % \
                                   (tof_distance, result.get('tof.result.successrate', np.nan))
                        if rssi_init is not np.nan and rssi_refl is not np.nan:
                            rmsg += ', RSSI = %03.0fdBm/%03.0fdBm' % (rssi_init, rssi_refl)
                        if cfo is not np.nan:
                            rmsg += ', CFO = %3.2fkHz' % (cfo)
                        rmsg += ', Duration = %03dms' % (duration)
                        populate_distance_results(meas_array, board_pair, rtp_distance, tof_distance, rade_distance, rade_trk_distance, cde_distance)
                        
                        if plot_mode > 0 and 'fig' not in globals():
                            ranging_result_plot_init(meas_array, result, algos, plot_y_lim, plot_x_type, plot_snapshot_nb_points, plot_slide_mode, plot_time_lim)

                    if (n % log_modulo) == 0 and len(log_lst):
                        log_lst.reverse()
                        msg = 'Iteration - '
                        if log_modulo != 1:
                            msg += '%4d..%-4d' % (n, n + log_modulo - 1)
                        else:
                            msg += '%4d' % n
                        msg += '/%-4d' % (len(parameters_product))
                        if len(log_lst):
                            msg += ' @(%s)' % (', '.join(log_lst))
                        if log_modulo == 1:
                            msg += rmsg
                        if is_error:
                            log.error(msg)
                        else:
                            log.info(msg)

                    # plot results
                    if is_error == False:
                        if plot_mode > 0:
                            ranging_result_plot(meas_array, result, execAlgo, plot_mode, plot_x_type, filepath, n,len(parameters_product))

                except Exception as e:
                    # Catch any exception and log it in order to continue execution
                    print(traceback.format_exc())

        if ((n+1) % save_modulo) == 0 and len(targets) > 0 and len(dut.resultfile) > 0:
            save_lst.reverse()
            msg = 'Saving data '
            if save_modulo != 1:
                msg += '%d..%d' % (n-save_modulo+1, n)
            else:
                msg += '%d' % n
            msg += '/%d to %s' % (len(parameters_product), filepath)
            if len(save_lst):
                msg += ' @(%s)' % (', '.join(save_lst))
            msg += ' => ' + ', '.join(targets)
            log.info(msg)
            filename = filepath
            if save_level != -1:
                filename += '_' + '_'.join(save_lst)
                filename = filename.replace('=', '_')
            for ext in targets:
                rf_filename = f'{filename}.{ext}'
                try:
                    dut.resultfile.save(rf_filename)
                except Exception as e:
                    log.error('Failed saving %s (target = %s)' % (rf_filename, ext))
                else:
                    log.info('Saved %s (target = %s)' % (rf_filename, ext))
                # Save additional txt file if auxdut exists
                if ext == 'txt' and auxdut is not None:
                    try:
                        rf_filename = f'{filename}_{auxdut.ifc.port}_auxdut.{ext}'
                        auxdut.resultfile.save(rf_filename)
                    except:
                        log.error('Failed saving %s (target = %s)' % (rf_filename, ext))
                    else:
                        log.info('Saved %s (target = %s)' % (rf_filename, ext))
            if save_level != -1:
                all_result_lst.extend(list(dut.resultfile))
                dut.resultfile.clear()

    if len(dut.resultfile):
        all_result_lst.extend(list(dut.resultfile))

    strstat = "\n==============Measurement stats ======================\n"
    for meas in meas_array:
        strstat += "Results for board pair " + str(meas['pair']) + ":\n"
        if len(meas['tof_array']) > 0:
            meas['tof_avg'] = np.average(meas['tof_array'])
            meas['tof_stdev'] = np.std(meas['tof_array'])
            strstat += "RTT : avg={:.2f}, stdev={:.2f}, min={:.2f}, max={:.2f}\n".format(meas['tof_avg'], meas['tof_stdev'], np.min(meas['tof_array']), np.max(meas['tof_array']))
        if len(meas['mciq_array']) > 0:
            meas['mciq_avg'] = np.average(meas['mciq_array'])
            meas['mciq_stdev'] = np.std(meas['mciq_array'])
            strstat += "RTP: avg={:.2f}, stdev={:.2f}, min={:.2f}, max={:.2f}\n".format(meas['mciq_avg'], meas['mciq_stdev'], np.min(meas['mciq_array']), np.max(meas['mciq_array']))
        if len(meas['rade_array']) > 0:
            meas['rade_avg'] = np.average(meas['rade_array'])
            meas['rade_stdev'] = np.std(meas['rade_array'])
            strstat += "RADE: avg={:.2f}, stdev={:.2f}, min={:.2f}, max={:.2f}\n".format(meas['rade_avg'], meas['rade_stdev'], np.min(meas['rade_array']), np.max(meas['rade_array']))
    strstat += "======================================================"
    log.info(strstat)
    if dut.Calibrate is not None:
        assert (len(meas_array) == 1)
        meas = meas_array[0]
        strstat = "\n======================================================\n"
        if (meas['tof_stdev'] >= 2) or (meas['mciq_stdev'] >= 0.5):
            strstat += "Calibration FAILED!!\nMeasurement stdev is too high. Please check your setup and try again\n"
        else:
            strstat += "Calibration PASSED for board pair ({}) at {} meters:\n".format(board_pair, dut.Calibrate)
            strstat += "RTP Zero-distance calibration value = {:.4f} m (Q10: {})\n".format(meas['mciq_avg'] - dut.Calibrate, int((meas['mciq_avg'] - dut.Calibrate)*(2**10)))
            strstat += "RTT Zero-distance calibration value = {:.4f} m (Q10: {})\n".format(meas['tof_avg'] - dut.Calibrate, int((meas['tof_avg'] - dut.Calibrate)*(2**10)))
        strstat += "======================================================"
        log.info(strstat)

    absfilepath = os.path.abspath(os.path.join(os.getcwd(), filepath))
    log.info(f'Finished all measurements. The files are stored in {absfilepath}.')
    log.removeHandler(fh)
    fh.close()
    return all_result_lst

