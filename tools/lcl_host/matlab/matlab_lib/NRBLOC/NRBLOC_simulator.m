%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is a main file to analyze the performance of multicarrier-phase-difference approach in combination
%   with super-resolution algorithms.
%   It allows to analyze the impact of digital clock-offset, residual CFO,
%   phase-Noise Thermal Noise, etc.
%   This file executes both a discrete-time equivalent simulation as well
%   as a semi-analytical system model.
% Description end
function NRBLOC_simulator()
close all;
DO_SIM=true;    %Full system simulation, makes it (very) slow 
DO_PLOT=true;   %If true, the pseudo-spectra will be plotted 

cnt_chan=1;
% % load('\\unixnl\nrbrpoc\projectdata\measurement_data\WP1_measurement\WP1_P051_ChannelData'); %Load the channel data, description can be found in WP1_P051_ChannelData_description.m
load('WP1_P051_ChannelData.mat'); %Load the channel data, description can be found in WP1_P051_ChannelData_description.m

%Restructure channel-date and make it suitabel for simulation
for cnt=1:length(S_Data)
    for cnt2=1:4
        H_full=S_Data(cnt).H(cnt2,:);
        H_full=H_full(1,:);
        DeltaT=1/400e6; %Default sample period at which the simulator is running
        [ChanData(cnt_chan).tau,ChanData(cnt_chan).h] = H_to_CIR_DT(freq_meas,H_full,2.4e9,DeltaT);
        ChanData(cnt_chan).H_full=H_full;
        ChanData(cnt_chan).DistIs=norm(GetPosition(SegmentID_all{cnt},PosID_all(cnt))-GetBSPos(BSID_all(cnt)));
        ChanData(cnt_chan).ChanID=cnt;
        cnt_chan=cnt_chan+1;
    end
end
NofChan = 1;%length(ChanData);
NofSim  = 1;

RE(1) = RangeEstimator('ChannelReconstruct', ChannelReconstruct_NonPhaseCoherent, 'RangingEngine', RangingEngine_phasediff);
%RE(2) = RangeEstimator('ChannelReconstruct', ChannelReconstruct_NonPhaseCoherent, 'RangingEngine', RangingEngine_Music_emb);
%RE(3) = RangeEstimator('ChannelReconstruct', ChannelReconstruct_NonPhaseCoherent, 'RangingEngine', RangingEngine_Music_cpp);

nTones = 8;
IQ_B2A_sim   = NaN(NofSim, nTones);
IQ_A2B_sim   = NaN(size(IQ_B2A_sim));
IQ_B2A_th    = NaN(size(IQ_B2A_sim));
IQ_A2B_th    = NaN(size(IQ_B2A_sim));
Dist_est_SIM = NaN(NofChan, NofSim, length(RE));
Dist_est_th  = NaN(size(Dist_est_SIM));
Dist_Err_SIM = NaN(size(Dist_est_SIM));
Dist_Err_th  = NaN(size(Dist_est_SIM));

for cnt_chan=1:NofChan
    disp([num2str(cnt_chan) ' of ' num2str(NofChan) ])
    
    [sys] = TRX_settings('NofFreq', nTones); %Note that the time-offset, ppm-offset, etc are internally random generated!
    %[sys] = TRX_settings_1WR(); 
    
    for SIM=1:NofSim
        disp(['SIM = ' num2str(SIM) ' of ' num2str(NofSim)])
        
        if DO_SIM %Simulation part
            [IQ_B2A_sim(SIM,:),IQ_A2B_sim(SIM,:)] = simulate_MCPD(sys,ChanData(cnt_chan).tau,ChanData(cnt_chan).h,1,1);
            if DO_PLOT
                sfigure(10101);clf;
            end
            for RE_cnt=1:length(RE)
                Dist_est_SIM(cnt_chan,SIM,RE_cnt) = RE(RE_cnt).do(IQ_A2B_sim(SIM,:), IQ_B2A_sim(SIM,:));
                if DO_PLOT && isprop(RE(RE_cnt).RangingEngine,'Report') && isfield(RE(RE_cnt).RangingEngine.Report,'s')
                    plot(RE(RE_cnt).RangingEngine.Report.w/(2*pi*RE(RE_cnt).RangingEngine.delta_F)*3e8,log10(abs(RE(RE_cnt).RangingEngine.Report.s)))
                    hold on
                    xlabel('Distance [m]');
                    ylabel('Pseudo-spectrum');
                end
            end
            Dist_Err_SIM(cnt_chan,SIM,:) = Dist_est_SIM(cnt_chan,SIM,:)-ChanData(cnt_chan).DistIs;
        end
        %Semi-analytical-part
        [IQ_B2A_th(SIM,:),IQ_A2B_th(SIM,:),sys] = semianalytic_MCPD(sys,ChanData(cnt_chan).tau,ChanData(cnt_chan).h);
        if DO_PLOT
            sfigure(10102);clf;
        end
        for RE_cnt=1:length(RE)
            Dist_est_th(cnt_chan,SIM,RE_cnt) = RE(RE_cnt).do(IQ_A2B_th(SIM,:).', IQ_B2A_th(SIM,:).');
            if DO_PLOT && isprop(RE(RE_cnt).RangingEngine,'Report') && isfield(RE(RE_cnt).RangingEngine.Report,'s')
                plot(RE(RE_cnt).RangingEngine.Report.w/(2*pi*RE(RE_cnt).RangingEngine.delta_F)*3e8,log10(abs(RE(RE_cnt).RangingEngine.Report.s)))
                hold on
                xlabel('Distance [m]');
                ylabel('Pseudo-spectrum');
            end
        end
        Dist_Err_th(cnt_chan,SIM,:) = Dist_est_th(cnt_chan,SIM,:) - ChanData(cnt_chan).DistIs;
    end
    sfigure(10);clf
    bins=-100:0.02:200;
    for RE_cnt=1:length(RE)
        Err=squeeze(Dist_Err_th(:,:,RE_cnt));
        [NofElemInBin1,bins]=hist(Err(:),bins);
        hdls(RE_cnt)=plot(bins,cumsum(NofElemInBin1)/sum(NofElemInBin1));
        hold on
        legendTXT{RE_cnt}=str(RE(RE_cnt));
    end
    legend(hdls,legendTXT,'interpreter','none')
    xlim([-1 9])
    drawnow
%     SIM
end
end





