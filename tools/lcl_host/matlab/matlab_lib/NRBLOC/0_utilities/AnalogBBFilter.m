%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef AnalogBBFilter < Util
    % Description
    % Description end
    % Description end
    % Change Log
    % Date        Author      Changes
    % 08/04/2013  Jac Romme   1e version
    
    properties
        FilterType  = 'butter';%butterworth
        Gain_dB     = 0;
        order       = 3;
        CuttOff_Hz  = 250e3; %[Hz, -3dB]
    end
    properties (Hidden)
        B
        A
        State=NaN;
        DeltaT      = 1/256e6;%[sec]
    end
    methods
        function obj=AnalogBBFilter(FilterTypeIn,Gain_dBIn,orderIn,CuttOffIn,DeltaTIn)
                if nargin>0, obj.FilterType=FilterTypeIn;   end
                if nargin>1, obj.Gain_dB=Gain_dBIn;         end
                if nargin>2, obj.order=orderIn;             end
                if nargin>3, obj.CuttOff_Hz=CuttOffIn;      end
                if nargin>4, obj.DeltaT=DeltaTIn;           end
        end
        function set.CuttOff_Hz(obj,value) % Handle class
            obj.CuttOff_Hz=value;
            obj.B=[];obj.A=[];obj.State=NaN;
        end
        function set.order(obj,value) % Handle class
            obj.order=value;
            obj.B=[];obj.A=[];obj.State=NaN;
        end
        function set.FilterType(obj,value) % Handle class
            obj.FilterType=value;
            obj.B=[];obj.A=[];obj.State=NaN;
        end
        function y=do(obj,x)
            x=set_freq(x,0);
            if x.deltaT~=obj.DeltaT
                error('input signal sample rate does not match the analog filter sample rate')
            elseif isnan(obj.State);
                obj=SetDiscreteEqFilter(obj);
                [tmp,obj.State]=filter(obj.B,obj.A,10^(obj.Gain_dB/20)*x.signal);
            else
                [tmp,obj.State]=filter(obj.B,obj.A,10^(obj.Gain_dB/20)*x.signal,obj.State);
            end
            y=x;
            y.signal=tmp;
            
        end
        function y=Test(obj)
            y=do(obj,[1 0 0 0 0 0 ]);
        end
        function obj=SetDiscreteEqFilter(obj)
            switch obj.FilterType
                case 'none'
                   obj.B=1;
                   obj.A=1;
                case 'RC'
                    RC=1/(2*pi*obj.CuttOff_Hz);
                    alpha=obj.DeltaT/(RC+obj.DeltaT);
                    obj.B=[alpha];obj.A=[1 (alpha-1)];
                case 'butter'
                    if obj.order==3 && obj.CuttOff_Hz==200e3 && obj.DeltaT==1/32e6
                        obj.A=[1 -2.921465223194893 2.845984055761115 -9.244605831864217e-01];
                        obj.B=[7.281172475004372e-06 2.184351742501312e-05 2.184351742501312e-05 7.281172475004372e-06];
                    elseif obj.order==3 && obj.CuttOff_Hz==250e3 && obj.DeltaT==1/32e6
                        obj.A=[100.0000 -290.1835  280.8429  -90.6482]/100;
                        obj.B=[14.0851   42.2554   42.2554   14.0851]/1e6;
                    elseif obj.order==3 && obj.CuttOff_Hz==500e3 && obj.DeltaT==1/32e6
                        obj.A=[100.0000 -280.3729  262.6248  -82.1660]/100;
                        obj.B=[107.4739  322.4216  322.4216  107.4739]/1e6;
                    elseif obj.order==3 && obj.CuttOff_Hz==650e3 && obj.DeltaT==1/32e6
                        obj.A=[100.0000 -274.4917  252.1338  -77.4585]/100;
                        obj.B=[229.6383  688.9150  688.9150  229.6383]/1e6;
                    else
                        [obj.B,obj.A] = butter(obj.order,obj.CuttOff_Hz*obj.DeltaT*2);
                    end
                otherwise
                    error('to be implemented')
            end
        end
        function varargout=freqz(obj,f_Hz)
            if isnan(obj.State);
                obj=SetDiscreteEqFilter(obj);
            end
            if nargin<2
                f_Hz=16*1024;
            end
                
            
            [H,f]=freqz((10^(obj.Gain_dB/20))*obj.B,obj.A,f_Hz,1/obj.DeltaT);
            if nargout==0
                plot(f/1e3,20*log10(abs(H)))
                xlabel('freq [kHz]');ylabel('Gain [dB]')
            else
                varargout{1}=H;
                varargout{2}=f; 
            end
        end
        function []=TN_plot(obj)
            close all
            FilterTypeIn='butter';Gain_dBIn=0;orderIn=3;
            f_cutoff=[150 200 250 650]*1e3;colors='rgbcmyk';
            figure
            subplot(2,1,1)
            for cnt=1:length(f_cutoff)
                A=AnalogFilter(FilterTypeIn,Gain_dBIn,orderIn,f_cutoff(cnt));
                [H,f]=freqz(A);
                semilogx(f/1e3,20*log10(abs(H)),colors(cnt));
                hold on
                clear A
            end
            legend('f_{cut}=150[kHz]','f_{cut}=200[kHz]','f_{cut}=250[kHz]','f_{cut}=650[kHz]','Location','SouthWest')
            grid on
            xlabel('f [kHz]')
            ylabel('H(f) [dB]')
            xlim([10 16000])
            ylim([-60 0])
            
            subplot(2,1,2)
            for cnt=1:length(f_cutoff)
                A=AnalogFilter(FilterTypeIn,Gain_dBIn,orderIn,f_cutoff(cnt));
                [H,f]=freqz(A);
                Phase=unwrap(angle(H))/pi*180;
                plot(f/1e3,Phase,colors(cnt));
                deltaPhase=Phase(2)/f(2);
                hold on
                plot(f/1e3,f*deltaPhase,[colors(cnt) '--']);
                clear A
            end
            grid on
            xlabel('f [kHz]')
            ylabel('angle H(f) [degrees]')
            xlim([1 1000])
            ylim([-270 0])
            set(gca,'Ytick',[-270:30:0])
            
        end
    end
end