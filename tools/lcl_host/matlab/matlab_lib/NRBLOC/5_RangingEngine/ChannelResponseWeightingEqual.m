%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file implements weighting of the channel-response.
% Description end

classdef ChannelResponseWeightingEqual < ChannelResponseWeightingAbstract
    properties
        weight = 1
    end
    methods
        function set.weight(~, ~)
            % Since all channel-responses needed to be weighted equally,
            % the weight must remain 1, whatever value is set.
            
            % -- do nothing -- 
        end
        
        function Hest = do(~, Hest)
            %   INPUT:
            %   * Hest   : One or two way channel estimation of size [nPairs x nTones]
            %
            %   OUTPUT:
            %   * Hest   : Weighted channel estimation
            
            % Do nothing, because all channel response are weighted
            % equally, therefore input equals output.
        end
    end
end