%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class has functions to calculate the tones that are dipping and/or
%   clipping.
% Description end

classdef QualityIndicator_IQ < QualityIndicatorAbstract
    properties
        PLOT = false;
        clip_limit = 0.925;
        dip_limit = 0.025;
    end

    methods
        function Out = do(self, IQ_A, IQ_B, varargin)
            IQsize = size(IQ_A);
            [IQ_A, IQ_B] = self.parse_inputs(IQ_A, IQ_B, varargin);

            for iCh=1:size(IQ_A,1)
                magna_max = self.calc_magn_max(IQ_A(iCh,:));
                magnb_max = self.calc_magn_max(IQ_B(iCh,:));
                Out.Clipping(iCh,:) = self.calc_clipping(magna_max) | self.calc_clipping(magnb_max);
                Out.Dipping(iCh,:) = self.calc_dipping(magna_max) | self.calc_dipping(magnb_max);

                if self.PLOT
                    figure
                    plot(abs(IQ_A(iCh,:)), 'DisplayName', 'Magnitude IQ_A')
                    grid on
                    hold on
                    plot(abs(IQ_B(iCh,:)), 'DisplayName', 'Magnitude IQ_B')
                    % clipping
                    c=max(abs(IQ_A(iCh,:)), abs(IQ_B(iCh,:)));
                    c(find(Out.Clipping(iCh,:) == 0)) = NaN;
                    plot(c, '^r', 'DisplayName', 'Clipping IQ');
                    % dipping
                    c=min(abs(IQ_A(iCh,:)), abs(IQ_B(iCh,:)));
                    c(find(Out.Dipping(iCh,:) == 0)) = NaN;
                    plot(c, 'vr', 'DisplayName', 'Dipping IQ');
                    legend('Location', 'best')
                    title('IQ Indicators')
                end
            end
            
            if all(size(Out.Clipping) ~= IQsize)
                % Assuming Clipping and Dipping have same size
                % Assuming IQ and Clipping has the same size
                Out.Clipping = Out.Clipping.';
                Out.Dipping = Out.Dipping.';
            end
        end

        function clipping = calc_clipping(self, magn_max)
            clipping = magn_max > self.clip_limit;
        end

        function dipping = calc_dipping(self, magn_max)
            dipping = magn_max < self.dip_limit;
        end
    end
    
    methods (Static)
        function magn = calc_magn_max(IQ)
            magn = max(abs(real(IQ)), abs(imag(IQ)));
        end
    end
end
