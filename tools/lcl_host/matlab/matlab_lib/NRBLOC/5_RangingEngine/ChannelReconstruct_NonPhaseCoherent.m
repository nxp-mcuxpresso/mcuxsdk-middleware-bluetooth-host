%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Implementation of the 1 directional frequency domain channel reconstruction.
%   A description of the idea behind the algorithm can be found in section 5.5 of TN196.
%   This function has at least 5 inputs, namely the phase and magnitude
%   measured at A(initiator), the phase and magnitude measured at B(reflector)
%   and the number of bits used to represent the phase.
% Description end

classdef ChannelReconstruct_NonPhaseCoherent < ChannelReconstructAbstract
    properties
        PLOT = false
    end
    
    methods
        function CRout = do(self, IQ_A, IQ_B, varargin)
            % Parse input data
            [IQ_A, IQ_B, varargin] = self.parse_inputs(IQ_A, IQ_B, varargin);
            nAnt = size(IQ_A,1);
            
            for iCh=1:nAnt
                H_est_sq = IQ_A(iCh,:).*IQ_B(iCh,:);
                [H_est, Uncertainty] = self.calc_h_est(H_est_sq, varargin{:});
                CRout.Hest(iCh,:)        = H_est;
                CRout.Uncertainty(iCh,:) = Uncertainty;
                % Probability of correct reconstruction of each tone
                ProbCorrectSign = (abs(H_est/max(abs(H_est)))>0.05);
                CRout.ProbCorrectSign(iCh,:) = ProbCorrectSign;
            end
        end
        
        function [H_est, Uncertainty] = calc_h_est(self, H_est_sq, varargin)
            if ~isempty(varargin)
                H_is=varargin{1};
                self.PLOT = true;
            end

            H_est    = sqrt(H_est_sq);
            if self.PLOT
                figure;
                subplot(2,2,1); hold on; axis equal
                plot(real(H_is),imag(H_is),'o--');
                %plot(real(H_est),imag(H_est),'x-');
                hdl(1) = subplot(2,2,2); hold on;
                plot(real(H_is),'o--');
                plot(imag(H_is),'o:');
                plot(real(H_est),'x--');
                plot(imag(H_est),'x:');
                plot(abs(H_est-H_is).^2>0.01,'r');
                
                subplot(2,2,3); hold on
                plot(20*log10(abs(H_is)),'o--');
                plot(20*log10(abs(H_est)),'x--');
            end
            I = abs(H_est(2:end)-H_est(1:end-1))> abs(H_est(2:end)+H_est(1:end-1));
            if self.PLOT
                subplot(2,2,2)
                plot(I,'c--');
            end
            for cnt=1:length(I)
                if I(cnt)==1
                    H_est(cnt+1:end)=-H_est(cnt+1:end);
                end
            end

            H_est_old        = H_est;        %Martijn, Do not implement this line
            Kf = length(H_est);
            Phase_est_coarse = mean(self.diff_unwrap(angle(H_est_sq)));
            Demod       = exp(-1i*Phase_est_coarse/2*(0:Kf-1));
            H_demod     = H_est.*Demod;
            Uncertainty = 0;

            for cnt=3:length(H_demod)-2
                H_lowside(cnt)  =  H_demod(cnt-1) + H_demod(cnt-1) - H_demod(cnt-2);
                H_highside(cnt) =  H_demod(cnt+1) + H_demod(cnt+1) - H_demod(cnt+2);
                Err_low1(cnt)   =  H_demod(cnt) - H_lowside(cnt);
                Err_low2(cnt)   = -H_demod(cnt) - H_lowside(cnt);
                Err_high1(cnt)  =  H_demod(cnt) - H_highside(cnt);
                Err_high2(cnt)  = -H_demod(cnt) - H_highside(cnt);

                Xa(cnt)=abs(Err_low1(cnt))>abs(Err_low2(cnt));      %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
                Xb(cnt)=abs(Err_high1(cnt))>abs(Err_high2(cnt));   %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
                if (Xa(cnt)==1) && (Xa(cnt-1)==0)
                    if (Xb(cnt-1)==1) || (Xb(cnt-2)==1)
                        H_est(cnt:end) = -H_est(cnt:end);
                        Uncertainty = Uncertainty+1;
                    end
                end
            end

            if self.PLOT
                subplot(2,2,1)
                plot(real(H_est),imag(H_est),'+-');
                hdl(2)=subplot(2,2,4);
                plot(real(H_is),'o--');hold on
                plot(imag(H_is),'o:');
                plot(real(H_est),'+--');hold on
                plot(imag(H_est),'+:');
                plot(abs(H_est-H_is).^2>0.01*rms(H_is).^2,'r');
                plot(abs(H_est_old-H_is).^2>0.01*rms(H_is).^2,'r--');
                linkaxes(hdl,'x')
            end
        end
    end

    methods (Static)
        function x = diff_unwrap(angles)
            x = diff(angles);
            x = x+(x<-pi)*2*pi;
            x = x-(x>pi)*2*pi;
        end
    end
end