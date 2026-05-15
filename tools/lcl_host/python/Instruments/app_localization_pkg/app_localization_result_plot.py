#------------------------------------------------------------------------------
#  Copyright 2025-2026 NXP
#
#  SPDX-License-Identifier: BSD-3-Clause
#------------------------------------------------------------------------------
import matplotlib.pyplot as plt
from matplotlib import gridspec
import numpy as np
from pathlib import Path
from PIL import Image
import math
import matplotlib.animation as animation
import sys
import os

BaseDir = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(BaseDir, '..', '..', 'Generic'))
from nesteddict import NestedDict


class ResultPlot:
    def __init__(self, algos=[0,1], show_rssi=0, plotConfig=[0,10,0,200,0,25]):
        #plotConfig: [plot_mode, y_lim (m), x-type, snapshot_nb_points,slide_mode,time_lim]
        #Plot mode: 0:No plotting, 1:Realtime + History block plotting, 2: History block plotting || x-type: 0:time index, 1:time in seconds || slide_en: 0:static, 1:sliding display rade_trk, 2: rade_raw+rade_trk|| time_lim: time span of sliding display
        self.y_cde                  = []
        self.y_rade                 = []
        self.y_rade_trk             = []
        self.x_time                 = []
        self.rssi_i                 = []
        self.rssi_r                 = []
        self.meas_cde_array         = []
        self.meas_rade_array        = []
        self.meas_rade_trk_array    = []
        self.meas_time_array        = []
        self.result_start_ts = 0

        self.cde_enabled            = int(algos[0])
        self.rade_enabled           = int(algos[1])
        self.show_rssi              = int(show_rssi)

        self.plot_mode              = int(plotConfig[0])
        self.plot_y_lim             = int(plotConfig[1])
        self.plot_x_type            = int(plotConfig[2])
        self.snapshot_len           = int(plotConfig[3])
        self.plot_slide_mode        = int(plotConfig[4])
        self.plot_time_lim          = int(plotConfig[5])

        self.rng_snapshot_plt_rade_trk_ln = None
        self.rng_snapshot_plt_cde_ln = None
        self.rng_plt_ax = None
        self.rng_snapshot_plt_ax = None
        self.rng_snapshot_plt_rade_ln = None
        self.rng_plt_bg = None
        self.rng_text_box = None
        self.ani = None
        self.fig_height = 0
        self.fig_width = 0

        self.fig = plt.figure()
        self.fig.canvas.mpl_connect('close_event', self.on_close)
    
    def distance_plotter_init(self,rng_time_interval=0.5):
        # self.fig = plt.figure()
        if self.plot_slide_mode == 0:
            spec = gridspec.GridSpec(ncols=1, nrows=2, height_ratios=[4, 1])
        else:
            spec = gridspec.GridSpec(ncols=1, nrows=2, height_ratios=[102, 1])
        self.rng_snapshot_plt_ax = self.fig.add_subplot(spec[0])
        self.rng_plt_ax = self.fig.add_subplot(spec[1])
        # The text box is algin to the unvisable subplot.
        self.rng_text_box = self.rng_snapshot_plt_ax.text(
            0.5, 1, '', transform=self.rng_plt_ax.transAxes,
            fontsize=52, verticalalignment='bottom', horizontalalignment='center')
        self.rng_text_box.set_color('#203F99')
        self.rng_text_box.set_weight('extra bold')
        
        fig_size_scal = 1.5
        self.fig_height = self.fig.get_figheight()*fig_size_scal
        self.fig_width = self.fig.get_figwidth()*fig_size_scal
        self.fig.set_figheight(self.fig_height)
        self.fig.set_figwidth(self.fig_width)
        if self.plot_slide_mode == 0:
            self.fig.tight_layout(pad=2, h_pad=4)
        else:
            self.fig.tight_layout(pad=4, h_pad=4)
            self.fig.subplots_adjust(bottom=0.01)
        self.fig.canvas.manager.set_window_title('NXP Channel Sounding')
        x = list(range(0,self.snapshot_len))
        if self.plot_x_type == 1:
            x = [t*rng_time_interval for t in x]
        y = -10 * np.ones(self.snapshot_len)
        if self.cde_enabled:
            if self.plot_slide_mode == 0:
                (self.rng_snapshot_plt_cde_ln,) = self.rng_snapshot_plt_ax.plot(x, y, color='#BBE7FC', marker='>', markersize=5, linestyle='none', lw=1, label = 'CDE')
        if self.rade_enabled:
            if self.plot_slide_mode != 1:
                (self.rng_snapshot_plt_rade_ln,) = self.rng_snapshot_plt_ax.plot(x, y, color='#A9A9A9', marker='v', markersize=5, linestyle='none', lw=1, label = 'RADE-embed-raw')
            if self.plot_slide_mode == 0:
                (self.rng_snapshot_plt_rade_trk_ln,) = self.rng_snapshot_plt_ax.plot(x, y, color='#F36424', marker='o', markersize=7, linestyle='none', lw=2, label = 'RADE-embed-trk')
            else:
                (self.rng_snapshot_plt_rade_trk_ln,) = self.rng_snapshot_plt_ax.plot(x, y, color='#F36424', marker='o', markersize=7, lw=2, label = 'RADE-embed-trk')
        if self.plot_slide_mode == 0:         
            self.rng_snapshot_plt_ax.set_title('NXP CS Localization APP', fontsize='14')
        else:
            self.rng_snapshot_plt_ax.set_title('NXP CS Algorithm RADE on KW47/MCXW72 with LCE Acceleration', fontsize='16')
        self.rng_snapshot_plt_ax.set_ylim([-5,self.plot_y_lim])
        self.rng_snapshot_plt_ax.spines['top'].set_linewidth(2)
        self.rng_snapshot_plt_ax.spines['bottom'].set_linewidth(2)
        if self.plot_slide_mode > 0:
            self.rng_snapshot_plt_ax.set_xlim([-self.plot_time_lim, 0.05]) #Use time as the x-axis
        self.rng_snapshot_plt_ax.legend(fontsize=16)
        self.rng_snapshot_plt_ax.set_ylabel("Distance (m)", fontsize = 14)
        self.rng_snapshot_plt_ax.grid(visible=True, color='#A9A9A9')
        self.rng_snapshot_plt_ax.minorticks_on()
        self.rng_snapshot_plt_ax.grid(visible=True, which='minor', color='#DCDCDC')
        if self.plot_x_type == 0:
            self.rng_snapshot_plt_ax.set_xlabel("Time Index",  x = 0, ha='left', fontsize = 14)
        else:
            self.rng_snapshot_plt_ax.set_xlabel("Time (s)",  x = 0, ha='left', fontsize = 14)
        if self.plot_slide_mode == 0:
            self.rng_plt_ax.set_ylabel("Distance (m)", fontsize = 14)
        self.rng_plt_ax.grid(visible=True, color='#A9A9A9')
        self.rng_plt_ax.minorticks_on()
        self.rng_plt_ax.grid(visible=True, which='minor', color='#DCDCDC')
        if self.plot_slide_mode > 0:
            if getattr(sys, 'frozen', False): # Bundled case
                nxp_logo_base_path = Path(sys._MEIPASS)
                logo_path = nxp_logo_base_path / 'nxp_logo.png' #ranging_platform_pkg is target path set in spec file
            else:
                nxp_logo_base_path = Path(__file__).parent
                logo_path = (nxp_logo_base_path / 'nxp_logo.png').resolve()
            logo = plt.imread(logo_path)
            height, width = logo.shape[0], logo.shape[1]
            new_width = int(width * 0.2)
            new_height = int(height * 0.2)
            logo_pil = Image.fromarray((logo * 255).astype('uint8'))  # Convert to PIL Image
            nxp_logo = logo_pil.resize((new_width, new_height))
            logo_ax = self.rng_snapshot_plt_ax.inset_axes([0.83, 0.00, 0.16, 0.16])  # [x0, y0, width, height]
            logo_ax.imshow(nxp_logo, alpha=0.7, zorder=10)
            logo_ax.axis('off')
            self.rng_plt_ax.set_visible(False)
        plt.show(block=False)
        self.rng_plt_bg = self.fig.canvas.copy_from_bbox(self.fig.bbox)
    
    def history_distance_plotter(self): 
        self.rng_plt_ax.cla()
        if self.plot_x_type == 1:
            meas_time_array_centered = [t - self.meas_time_array[0] for t in self.meas_time_array]
            if self.cde_enabled:
                self.rng_plt_ax.plot(meas_time_array_centered, self.meas_cde_array, color='#BBE7FC', marker='>', markersize=5, linestyle='none', lw=1, label = 'CDE')
            if self.rade_enabled:
                self.rng_plt_ax.plot(meas_time_array_centered, self.meas_rade_array, color='#A9A9A9', marker='v', markersize=5, linestyle='none', lw=1, label = 'RADE-raw')
                self.rng_plt_ax.plot(meas_time_array_centered, self.meas_rade_trk_array, color='#F36424', marker='o', markersize=5, linestyle='none', lw=1, label = 'RADE-trk')
            self.rng_plt_ax.legend(fontsize=50)
        else:
            if self.cde_enabled:
                self.rng_plt_ax.plot(self.meas_cde_array, color='#BBE7FC', marker='>', markersize=5, linestyle='none', lw=1, label = 'CDE')
            if self.rade_enabled:
                self.rng_plt_ax.plot(self.meas_rade_array, color='#A9A9A9', marker='v', markersize=5, linestyle='none', lw=1, label = 'RADE-raw')
                self.rng_plt_ax.plot(self.meas_rade_trk_array, color='#F36424', marker='o', markersize=5, linestyle='none', lw=1, label = 'RADE-trk')
        self.rng_plt_ax.set_ylim([-1, self.plot_y_lim])
        self.rng_plt_ax.set_ylabel("Distance (m)", fontsize = 14)
        self.rng_plt_ax.legend()
        self.rng_plt_ax.grid(visible=True, color='#A9A9A9')
        self.rng_plt_ax.minorticks_on()
        self.rng_plt_ax.grid(visible=True, which='minor', color='#DCDCDC')

    def update_data_to_distance_plotter(self, new_result:NestedDict, meas_cnt, meas_num):
        try:
            self.meas_time_array.append(float(new_result.get("host.timestamp", 0)))
            # Get the start time of the result.
            if len(self.x_time) == 0:
                self.result_start_ts = self.meas_time_array[-1]
            self.x_time.append(round(self.meas_time_array[-1] - self.result_start_ts, 3))

            null_RNG = -1.23 #Two decimal digits printed
            if self.cde_enabled and math.isnan(new_result.get('mciq.result.CDE_distance', np.nan)):
                return
            else:
                self.meas_cde_array.append(new_result.get('mciq.result.CDE_distance', np.nan))
                self.y_cde.append(self.meas_cde_array[-1])
            if self.rade_enabled and (math.isnan(new_result.get('mciq.result.RADE', np.nan)) or math.isnan(new_result.get('mciq.result.RADE_trk', np.nan))):
                return
            else:
                self.meas_rade_array.append(new_result.get('mciq.result.RADE', np.nan))
                self.meas_rade_trk_array.append(new_result.get('mciq.result.RADE_trk', np.nan))
                self.y_rade.append(self.meas_rade_array[-1])
                self.y_rade_trk.append(self.meas_rade_trk_array[-1])

            self.rssi_i.append(new_result.get('info.init.sync_rssi', 0))
            self.rssi_r.append(new_result.get('info.refl.sync_rssi', 0))

            if self.plot_mode == 1:
                fig_height_n = self.fig.get_figheight() 
                fig_width_n = self.fig.get_figwidth()
                if (self.fig_height != fig_height_n or self.fig_width != fig_width_n):
                    y_snpsht_tmp = -10 * np.ones(self.snapshot_len)
                    if self.cde_enabled: 
                        if self.plot_slide_mode == 0:        
                            self.rng_snapshot_plt_cde_ln.set_ydata(y_snpsht_tmp) 
                    if self.rade_enabled:
                        self.rng_snapshot_plt_rade_trk_ln.set_ydata(y_snpsht_tmp)
                        if self.plot_slide_mode != 1:
                            self.rng_snapshot_plt_rade_ln.set_ydata(y_snpsht_tmp)
                    self.rng_plt_bg = self.fig.canvas.copy_from_bbox(self.fig.bbox)
                    self.fig_height = fig_height_n
                    self.fig_width = fig_width_n
                else:
                    self.fig.canvas.restore_region(self.rng_plt_bg)
                if self.cde_enabled or self.rade_enabled:
                    x_time_centered = [t - self.x_time[0] for t in self.x_time]
                    x_time_snpsht = np.concatenate((x_time_centered,1000 * np.ones(self.snapshot_len - len(self.y_cde))),axis = 0)
                if self.rade_enabled:
                    if self.plot_slide_mode == 0:
                        y_rade_snpsht = np.concatenate((self.y_rade,-10 * np.ones(self.snapshot_len - len(self.y_rade))),axis = 0)
                        y_rade_trk_snpsht = np.concatenate((self.y_rade_trk,-10 * np.ones(self.snapshot_len - len(self.y_rade_trk))),axis = 0)
                        self.rng_snapshot_plt_rade_ln.set_ydata(y_rade_snpsht) 
                        self.rng_snapshot_plt_rade_trk_ln.set_ydata(y_rade_trk_snpsht) 
                        if self.plot_x_type == 1:
                            self.rng_snapshot_plt_rade_ln.set_xdata(x_time_snpsht) #Use time as the x-axis
                            self.rng_snapshot_plt_rade_trk_ln.set_xdata(x_time_snpsht)
                    else:
                        valid_time_indices = np.where(np.array(x_time_centered) > (x_time_centered[-1] - self.plot_time_lim))[0]
                        y_rade_array = np.array(self.y_rade)
                        absTol = 1e-1
                        diff = np.abs(y_rade_array[valid_time_indices] - null_RNG)
                        non_null_RNG_indices = np.where(diff > absTol)[0]
                        valid_time_indices = [valid_time_indices[i] for i in non_null_RNG_indices]
                        y_rade_valid0 = [self.y_rade[i] for i in valid_time_indices]
                        y_rade_trk_valid0 = [self.y_rade_trk[i] for i in valid_time_indices]
                        x_time_valid = [x_time_centered[i]-x_time_centered[-1] for i in valid_time_indices] 
                        y_rade_valid = np.concatenate((y_rade_valid0,-10 * np.ones(self.snapshot_len - len(y_rade_valid0))),axis = 0)
                        y_rade_trk_valid = np.concatenate((y_rade_trk_valid0,-10 * np.ones(self.snapshot_len - len(y_rade_trk_valid0))),axis = 0)
                        x_time_valid = np.concatenate((x_time_valid,1000 * np.ones(self.snapshot_len - len(x_time_valid))),axis = 0)
                        self.rng_snapshot_plt_rade_trk_ln.set_ydata(y_rade_trk_valid) 
                        self.rng_snapshot_plt_rade_trk_ln.set_xdata(x_time_valid)
                        if self.plot_slide_mode != 1:
                            self.rng_snapshot_plt_rade_ln.set_ydata(y_rade_valid) 
                            self.rng_snapshot_plt_rade_ln.set_xdata(x_time_valid) #Use time as the x-axis

                if self.cde_enabled:
                    if self.plot_slide_mode == 0:
                        y_cde_snpsht = np.concatenate((self.y_cde,-10 * np.ones(self.snapshot_len - len(self.y_cde))),axis = 0)
                        self.rng_snapshot_plt_cde_ln.set_ydata(y_cde_snpsht)
                        if self.plot_x_type == 1:
                            self.rng_snapshot_plt_cde_ln.set_xdata(x_time_snpsht) #Use time as the x-axis

                rng_snapshot_plt_title = ''
                if self.rade_enabled:
                    if self.plot_slide_mode == 0:
                        rng_snapshot_plt_title += f'{self.y_rade_trk[-1]:.2f}m' 
                    else:
                        rng_snapshot_plt_title += f'{y_rade_trk_valid0[-1]:.2f}m'
                        if self.show_rssi:
                            mod0_refl_rssi = new_result.get('md0.refl.rssi', np.nan)
                            subevt_sts_refl = new_result.get('mciq.reflector.sts', np.nan)
                            if mod0_refl_rssi[0] < 10 and subevt_sts_refl[0] == 0:
                                rng_snapshot_plt_title += f' | {mod0_refl_rssi[0]:d}dBm'
                else:
                    if self.cde_enabled: 
                        rng_snapshot_plt_title += f'{self.y_cde[-1]}m'
                self.rng_text_box.set_text(rng_snapshot_plt_title)
            if (meas_cnt % self.snapshot_len) == 0 or meas_cnt == meas_num:
                self.history_distance_plotter()
                plt.pause(0.01)
                self.y_cde      = []
                self.y_rade     = []
                self.y_rade_trk = []
                self.x_time     = []
                self.rssi_i     = []
                self.rssi_r     = []
        except Exception as e:
            print(f"Error accessing result data: {e}")
    def on_close(self, event):
        if self.ani is not None and self.ani.event_source:
            self.ani.event_source.stop()
        else:
            print("Animation not initialized or event_source is None")
    
    def start_distance_plotter(self, func_called:callable, interval:int=200): #smaller animation intervals will affect the time slices available to other threads
        # Need to check if self.event_source is none in the line 1469 of the animation.py of the matplotlib 3.10.5.
        self.ani = animation.FuncAnimation(fig=self.fig, 
                                        func=func_called, 
                                        init_func=self.distance_plotter_init, 
                                        interval=interval, 
                                        cache_frame_data=False)
        plt.show()



        