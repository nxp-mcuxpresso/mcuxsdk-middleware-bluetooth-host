%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Ranging using the phase-difference approach.
% Description end

classdef RangingEngine_phasediff < RangingEngineAbstract
    properties
        delta_F                                                             %[Hz]   frequency step size
        PLOT                 = false;
    end
    
    properties (Constant)
        SupportsMultiAntenna = false;                                        % Indicates whether multi-antenna input is supported
    end
    
    methods
        function defaults(self)
            defaults@RangingEngineAbstract(self);
            self.delta_F = RangingEngineAbstract.default_delta_F;           %[Hz]   frequency step size
        end

        function self = RangingEngine_phasediff(varargin)
            p = inputParser();
            p.KeepUnmatched = true;
            p.addOptional('delta_F', RangingEngineAbstract.default_delta_F, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', '>=', 500e3, '<=' 4e6}, mfilename, 'delta_F', 1));
            p.addParameter('PLOT', false, @(x) validateattributes(x, {'logical'}, {'scalar'}, mfilename, 'PLOT'));
            p.parse(varargin{:});
            
            self.delta_F = p.Results.delta_F;
            self.PLOT    = p.Results.PLOT;
        end
        function [dist_est, AoA] = do(self, CRout)
            Hest = CRout.Hest;
            if size(Hest,1)>1
                error(['Currently there is no solution for ' ,class(self), ' with multiple channels input']);
            else
                AoA           = NaN;                                             % Unused output
                MeasPhase     = angle(Hest);
                diff_phase    = diff(MeasPhase);
                MeanPhase     = angle(sum(exp(1i*diff_phase),2));
                if self.delta_F>0
                    MeanPhase = MeanPhase-2*pi*(MeanPhase>0);
                else
                    MeanPhase = MeanPhase-2*pi*(MeanPhase<0);
                end
                tau_LOS = -(MeanPhase)/(2*pi*self.delta_F);
                dist_est = tau_LOS*physconst('lightspeed');
%                 dist = self.distance_unwrap(Dist_est);
                
                if self.PLOT
                    figure; hold on;
                    plot(angle(cumsum(exp(1i*diff_phase),2)), 'b.-');
                    plot([0, length(diff_phase)], MeanPhase * [1,1], 'r-');
                end
            end
        end
    end
end