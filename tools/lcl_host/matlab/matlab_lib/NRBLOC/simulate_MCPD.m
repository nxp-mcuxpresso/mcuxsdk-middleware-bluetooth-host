%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [IQ_B2A_sim,IQ_A2B_sim]=simulate_MCPD(sys,tau,h,NofSim,PLOT)
    % Description
    % This is a simulation model for MCPD with amplitude info, taking into
    % account. It is called semi-analytical, because partially it is 
    % deterministic(taking into account carrier frequency offset,
    % sample timing drift, channel response, etc) 
    % and partially simulation-driven (phase noise and thermal noise).
    % see also semianalytic_MCPD
    % Description end

if nargin<4
    NofSim=1;
end
if nargin<5
    PLOT=false;
end
DeltaT=sys.DeltaT;
t=0:DeltaT:sys.T_sim;

IQ_B2A_sim = zeros(NofSim, length(sys.A.PhaseMeasUnit.t_sample));
IQ_A2B_sim = zeros(size(IQ_B2A_sim));
for SIM=1:NofSim
    A_LO=Signal_BB(exp(1i*phase(sys.A.LO,t)),0,DeltaT);
    B_LO=Signal_BB(exp(1i*phase(sys.B.LO,t)),0,DeltaT);
    
    A_LO=do(sys.A.LO.PNS,A_LO);%Add phase-noise
    B_LO=do(sys.B.LO.PNS,B_LO);%Add phase-noise
    
    %Generate the TX signals
    A_TX=Signal_BB(env(sys.A.PA,t).*exp(1i*angle(A_LO).'),0,DeltaT);
    B_TX=Signal_BB(env(sys.B.PA,t).*exp(1i*angle(B_LO).'),0,DeltaT);
    
    % tau=ChanData(cnt_chan).tau;
    % h=ChanData(cnt_chan).h;
    
    A2B_RX=ImecChanfilter(tau,h,A_TX);
    B2A_RX=ImecChanfilter(tau,h,B_TX);
%     A2B_RX2=ImecChanfilter_TimeVariant(tau,h,A_TX);
    
    A2B_RX=do(sys.B.LNA,A2B_RX);%Add thermal Noise(white)
    B2A_RX=do(sys.A.LNA,B2A_RX);%Add thermal Noise(white)
    
    A2B_IF=A2B_RX.*exp(-1i*angle(B_LO));%RX mixer @B
    B2A_IF=B2A_RX.*exp(-1i*angle(A_LO));%RX mixer @A
    
    A2B_IF=do(sys.B.ABB,A2B_IF);%Noise no longer white, signal bandwidth will be small
    B2A_IF=do(sys.A.ABB,B2A_IF);%Noise no longer white, signal bandwidth will be small
    
    IQ_B2A_sim(SIM,:)  = interp1(t,double(B2A_IF),sys.A.PhaseMeasUnit.t_sample,'linear');%store data to allow for evaluation later one
    IQ_A2B_sim(SIM,:)  = interp1(t,double(A2B_IF),sys.B.PhaseMeasUnit.t_sample,'linear');
    
    if PLOT
        plot_tone_exchange(t, sys, A_TX, B_TX, A2B_IF, B2A_IF, IQ_A2B_sim(SIM,:), IQ_B2A_sim(SIM,:));
%         plot_tone_exchange_trial(t, sys, A_TX, B_TX, A2B_IF, B2A_IF, IQ_A2B_sim(SIM,:), IQ_B2A_sim(SIM,:));
    end
end
end

function plot_tone_exchange(t, sys, A_TX, B_TX, A2B_IF, B2A_IF, IQ_AB, IQ_BA)
ColorMap = get(0,'DefaultAxesColorOrder');
scale_us  = 1e6;
scale_MHz = 1/1e6;

Hfig = figure('Position', [100, 100, 1135, 789]);
if exist('sgtitle','file')  % available from Matlab 2018b
	sgt = sgtitle('Tone exchange visualization','Color','k');
	sgt.FontSize = 20;
end
hdl_sp(1) = subplot(3,1,1); hold on
plot(t * scale_us, freq(sys.B.LO,t) * scale_MHz, 'DisplayName', '@B');
plot(t * scale_us, freq(sys.A.LO,t) * scale_MHz, 'DisplayName', '@A');
xlabel('time [\mus]');
ylabel('Frequency [MHz]');
title('LO-frequency');
legend('show', 'Location', 'best')

hdl_sp(2) = subplot(3,1,2); hold on
plot(t * scale_us, abs(B_TX), 'DisplayName', '@B');
plot(t * scale_us, abs(A_TX), 'DisplayName', '@A');
xlabel('time [\mus]');
ylabel('TX output magnitude');
title('PA activity');
legend('show', 'Location', 'Best');

hdl_sp(3) = subplot(3,1,3); hold on
plot(t * scale_us, abs(A2B_IF),'b', 'DisplayName', '|B|');
plot(t * scale_us, real(A2B_IF),'--', 'Color', ColorMap(3,:), 'DisplayName', 'B_r');
plot(sys.B.PhaseMeasUnit.t_sample * scale_us, real(IQ_AB),'s', 'Color', ColorMap(3,:), 'MarkerFaceColor', ColorMap(3,:), 'Displayname', 'sample B_r');
plot(t * scale_us, imag(A2B_IF),'-.', 'Color', ColorMap(4,:),  'DisplayName', 'B_i');
plot(sys.B.PhaseMeasUnit.t_sample * scale_us, imag(IQ_AB),'o', 'Color', ColorMap(4,:), 'MarkerFaceColor', ColorMap(4,:), 'Displayname', 'sample B_i');

plot(t * scale_us, abs(B2A_IF),'r', 'DisplayName', '|A|');
plot(t * scale_us, real(B2A_IF),'--', 'Color', ColorMap(5,:), 'DisplayName', 'A_r');
plot(sys.A.PhaseMeasUnit.t_sample * scale_us, real(IQ_BA),'s', 'Color', ColorMap(5,:), 'MarkerFaceColor', ColorMap(5,:), 'Displayname', 'sample A_r');
plot(t * scale_us, imag(B2A_IF),'-.', 'Color', ColorMap(6,:), 'DisplayName', 'A_i');
plot(sys.A.PhaseMeasUnit.t_sample * scale_us, imag(IQ_BA),'o', 'Color', ColorMap(6,:), 'MarkerFaceColor', ColorMap(6,:), 'Displayname', 'sample A_i');
xlabel('time [\mus]');
ylabel('Magnitude');
title('IF signals');
legend('show', 'Location', 'Best');

linkaxes(hdl_sp,'x');

% Create textbox with configuration info
TextBox = {sprintf('T_f = %.0f\\mus', sys.SW.T_f/1e-6), ...
    sprintf('\\Deltaf=%.1fMHz', sys.SW.delta_F/1e6), ...
    sprintf('K_f=%d', sys.SW.NofFreq), ...
    sprintf('XTAL_{offset, initial}: A = %.2f, B = %.2fppm', sys.PPM_A*1e6, sys.PPM_B*1e6), ...
    sprintf('XTAL_{offset, residue}: A = %.2f, B = %.2fppm', sys.PPM_A_tilde*1e6, sys.PPM_B_tilde*1e6)};
Hann = annotation('textbox', [.2 .5 .3 .3], 'String', TextBox, 'FitBoxToText', true, 'BackgroundColor', [1,1,1], 'Position', [0.031, 0.83, 0.25, 0.16], 'FaceAlpha', 0.6);
Hfig.UserData = Hann;
set(Hfig, 'SizeChangedFcn', {@fig_size_changed_cb, Hann});
end

function fig_size_changed_cb(~, ~, Htb)
    % When the figure size is changed, the FitBoxToText-property of the
    % annotation-box is set to false. This is enabled again here.
    Htb.FitBoxToText = true;
end

function plot_tone_exchange_trial(t, sys, A_TX, B_TX, A2B_IF, B2A_IF, IQ_AB, IQ_BA) %#ok<*DEFNU>
%This piece of code makes use of the stackedplot function. The problem
%is that there is no way of controlling the linespec of individual lines in
%a subplot.
%--------------------------------------------------------------------------
        % Create a table, to support using stackedplot
        time = t.' * scale_us;
        freq_B_LO = freq(sys.B.LO,t).' * scale_MHz;
        freq_A_LO = freq(sys.B.LO,t).' * scale_MHz;
        filler1 = nan(size(B2A_IF));
        tbl1 = table(time, freq_A_LO, freq_B_LO, abs(A_TX), abs(B_TX), abs(A2B_IF), real(A2B_IF), imag(A2B_IF), abs(B2A_IF), real(B2A_IF), imag(B2A_IF), filler1, filler1, filler1, filler1);
        tbl1.Properties.VariableNames(1:15) = {'time', 'freq_A_LO', 'freq_B_LO', 'PA_A', 'PA_B', 'abs_B', 'B_r', 'B_i', 'abs_A', 'A_r', 'A_i', 'sample_B_r', 'sample_B_i', 'sample_A_r', 'sample_A_i'};
        tbl1.Properties.VariableUnits(1:5) = {'\mus', 'MHz', 'MHz', 'PA-active', 'PA-active'};
        
        filler2 = nan(size(sys.B.PhaseMeasUnit.t_sample(:)));
        tbl2 = table(sys.B.PhaseMeasUnit.t_sample(:) * scale_us, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, real(IQ_AB).', imag(IQ_AB).', filler2, filler2);
        tbl2.Properties.VariableNames(1:15) = {'time', 'freq_A_LO', 'freq_B_LO', 'PA_A', 'PA_B', 'abs_B', 'B_r', 'B_i', 'abs_A', 'A_r', 'A_i', 'sample_B_r', 'sample_B_i', 'sample_A_r', 'sample_A_i'};
        tbl2.Properties.VariableUnits(1) = {'\mus'};

        tbl3 = table(sys.A.PhaseMeasUnit.t_sample(:) * scale_us, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, filler2, real(IQ_BA).', imag(IQ_BA).');
        tbl3.Properties.VariableNames(1:15) = {'time', 'freq_A_LO', 'freq_B_LO', 'PA_A', 'PA_B', 'abs_B', 'B_r', 'B_i', 'abs_A', 'A_r', 'A_i', 'sample_B_r', 'sample_B_i', 'sample_A_r', 'sample_A_i'};
        tbl3.Properties.VariableUnits(1) = {'\mus'};
        
        tbl = [tbl1; tbl2; tbl3];

        % What to plot in stackedplot function
        plot1 = {'freq_A_LO', 'freq_B_LO'};
        plot2 = {'PA_A', 'PA_B'};
        plot3 = {'abs_A', 'abs_B', 'A_r', 'A_i', 'B_r', 'B_i'};
        Ylabels = {'freq LO-s [MHz]', 'PA-active', 'IF-signals'};
        TitleStr = 'Tone exchange visualization';
        
        figure('Position', [100, 100, 1135, 789]);
        s = stackedplot(tbl, {plot1, plot2, plot3}, 'DisplayLabels', Ylabels, 'GridVisible', true, 'Title', TitleStr, 'XVariable', 'time');
        
        % Change line-colors of first two lines
        ColorMap = get(0,'DefaultAxesColorOrder');
        s.LineProperties(1).Color        = ColorMap(1:2,:);
        s.LineProperties(2).Color        = ColorMap(1:2,:);
        s.LineProperties(3).Color(1:2,:) = ColorMap(1:2,:);
        
        % Create textbox with configuration info
        TextBox = {sprintf('T_f = %.0f\\mus', sys.SW.T_f/1e-6), ...
                   sprintf('\\Deltaf=%.1fMHz', sys.SW.delta_F/1e6), ...
                   sprintf('K_f=%d', sys.SW.NofFreq), ...
                   sprintf('XTAL_{offset, initial}: A = %.2f, B = %.2fppm', sys.PPM_A*1e6, sys.PPM_B*1e6), ...
                   sprintf('XTAL_{offset, residue}: A = %.2f, B = %.2fppm', sys.PPM_A_tilde*1e6, sys.PPM_B_tilde*1e6)};
        annotation('textbox', [.2 .5 .3 .3], 'String', TextBox, 'FitBoxToText', 'on', 'BackgroundColor', [1,1,1], 'Position', [0.030837004405287 0.830418255294382 0.237004398485638 0.158428386023743]);
end
