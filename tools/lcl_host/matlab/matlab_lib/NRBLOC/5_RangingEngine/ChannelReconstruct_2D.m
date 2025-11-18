%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Implementation of 2D channel reconstruction.
% Description end

classdef ChannelReconstruct_2D < ChannelReconstructAbstract
    properties
        PLOT = false
        CR_freq = ChannelReconstruct_NonPhaseCoherent; % the channel reconstructor in frequency dimension
    end
    
    methods
        function self = ChannelReconstruct_2D(CR_freq)
            if nargin == 1
                self.CR_freq = CR_freq;
            end
        end
        
        function CRout = do(self, IQ_A, IQ_B, varargin)
            % correct data orientation
            [IQ_A, IQ_B, varargin] = self.parse_inputs(IQ_A, IQ_B, varargin);
            nAnt = size(IQ_A,1);
            
            % reconstruct along frequency dimension first
            CRout_freq = self.CR_freq.do(IQ_A, IQ_B, varargin{:});
            
            % reconstruct along spatial dimension
            IQ_ref = IQ_A;
            for iCh=1:nAnt
                CRout.Uncertainty(iCh,:) = NaN;
                CRout.ProbCorrectSign(iCh,:) = NaN;
            end
            
            CRout.Hest = self.do_spatial(IQ_ref, CRout_freq.Hest);
        end
        
        function H_est = do_spatial(self, IQ_ref, H_est)
            Kf = size(IQ_ref,2);
            Ka = size(IQ_ref,1);
            
            IQ_ref = IQ_ref*exp(1j*(angle(H_est(1))-angle(IQ_ref(1))));
            diff_IQ_ref = IQ_ref(1:end-1,:).*conj(IQ_ref(2:end,:))./abs(IQ_ref(2:end,:));
            
            for itone = 1:Kf
                
                if self.PLOT
                    figure(12054)
                    self.plotComplex(IQ_ref(:,itone),'-x')
                    hold on
                    self.plotComplex(H_est(:,itone),'-^')
                    hold off
                    legend('IQ\_ref','H\_est')
                    title(sprintf('Raw IQ, itone=%d',itone))
                end
                
                % Overall antenna sign-flip correction
                if itone == 1
                    d_high = abs(H_est(:,itone)-IQ_ref(:,itone));
                    d_low = abs(-H_est(:,itone)-IQ_ref(:,itone));

                    for iant = 1:Ka
                        if d_high(iant) > d_low(iant)
                            H_est(iant,itone:end) = -H_est(iant,itone:end);
                        end
                    end
                    
                    if self.PLOT
                        figure(12055)
                        self.plotComplex(IQ_ref(:,itone),'-x')
                        hold on
                        self.plotComplex(H_est(:,itone),'-^')
                        hold off
                        legend('IQ\_ref','H\_est')
                        title(sprintf('Corrected IQ, itone=%d\nPress any key to continue...',itone))
                        pause
                    end
                    
                    continue
                end
                
                % antenna sign-flip correction tone-by-tone
                for iant = 1:Ka-1
                    diff_H_est = H_est(1:end-1,itone).*conj(H_est(2:end,itone))./abs(H_est(2:end,itone));
                    
                    if self.PLOT
                        figure(12056)
                        self.plotComplex(diff_IQ_ref(:,itone),'-x')
                        hold on
                        self.plotComplex(diff_H_est,'-^')
                        hold off
                        legend('\DeltaIQ\_ref','\DeltaH\_est')
                        title(sprintf('Phase diff term, itone=%d',itone))
                    end
                    
                    d_high = abs(diff_H_est-diff_IQ_ref(:,itone));
                    d_low = abs(-diff_H_est-diff_IQ_ref(:,itone));
                    
                    if d_high(iant) > d_low(iant)
                        % something wrong with the current antenna pair
                        % try to tell which antenna's fault
                        if Ka == 2 % if only 2 antenna available, the one with higher magnitude is trusted
                            [~,iant2blame] = min(abs(H_est(:,itone)));
                        elseif iant == Ka-1
                            % if we've come to the last antenna pair, it's definitely
                            % the latter antenna's fault
                            iant2blame = iant+1;
                        else
                            if d_high(iant+1) > d_low(iant+1)
                                % something also wrong with the next antenna pair
                                iant2blame = iant+1;
                            else
                                iant2blame = iant;
                            end
                        end
                        H_est(iant2blame,itone:end) = -H_est(iant2blame,itone:end);
                    end
                end
                
                if self.PLOT
                    figure(12055)
                    self.plotComplex(IQ_ref(:,itone),'-x')
                    hold on
                    self.plotComplex(H_est(:,itone),'-^')
                    hold off
                    legend('IQ\_ref','H\_est')
                    title(sprintf('Corrected IQ, itone=%d\nPress any key to continue...',itone))
                    pause
                end
            end
        end
        
        
    end
    
    
    methods (Static=true)
        function plotComplex(value,marker,color)
            if nargin == 1
                plot(value)
            elseif nargin == 2
                plot(value,marker)
            elseif nargin == 3
                plot(value,marker,'Color',color)
            end
            
            grid on
            text(real(value), imag(value), arrayfun(@(x) num2str(x),1:length(value),'UniformOutput',false))
            
            xl = max(abs(xlim)); yl = max(abs(ylim));
            
            xmax = max(max(abs(real(value))), xl);
            ymax = max(max(abs(imag(value))), yl);
            
            amax = max([xmax,ymax]);
            amax = amax*1.1;
            
            axis([-amax amax -amax amax])
        end
    end
    
end
