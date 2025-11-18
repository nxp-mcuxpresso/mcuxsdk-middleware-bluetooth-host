%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class estimated the Rician-K factor based on the estimated
%   distance and the channel-response.
% Description end

classdef QualityIndicator_RicianK < QualityIndicatorAbstract
    properties
        delta_F = 1e6;
        owcr = true;        % Hest is a one-way-channel-response
    end
    
    methods
        function set.delta_F(self, val)
            assert(any(val == [0.5e6, 1e6, 2e6, 4e6]));
            self.delta_F = val;
        end
        
        function Kest = do(self, Hest, DistEst)
            % Hest must have dimension of nPairs x nTones
            % DistEst must be a vector of length nPairs
            
            nPairs = length(DistEst);
            DistEst = DistEst(:);
            [nRow, nCol]  = size(Hest);
            if nCol == nPairs
                Hest = Hest.';
                nTones = nRow;
            else
                nTones = nCol;
            end               
            
            speed_of_light = physconst('lightspeed');
            tau            = DistEst / speed_of_light;
            if self.owcr
                Demod   = exp(2i*pi*self.delta_F * tau * (0:nTones-1));
            else
                Demod   = exp(4i*pi*self.delta_F * tau * (0:nTones-1));
            end
            H_demod = Hest.*Demod;
            % Kest is calculates over each antenna-pair
            Kest    = abs(mean(H_demod, 2)).^2 ./(mean((abs(H_demod)).^2, 2)-abs(mean(H_demod, 2)).^2)/2;
        end
    end
end
