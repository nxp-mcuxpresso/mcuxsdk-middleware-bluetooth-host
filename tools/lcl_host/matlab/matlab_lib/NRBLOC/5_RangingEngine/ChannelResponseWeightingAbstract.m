%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file implements the interface definition of the channel-response 
%   weighting classes.
% Description end

classdef ChannelResponseWeightingAbstract < Util
    properties (Abstract)
        weight
    end
    methods (Abstract)
         Hest = do(self, Hest)
    end
    methods
        function self = ChannelResponseWeightingAbstract(varargin)
            p = inputParser;
            p.addParameter('weight', 1, @(x) validateattributes(x, {'numeric'}, {}) );
            p.parse(varargin{:});
            
            self.weight = p.Results.weight;
        end
        function CheckInputs(self, Hest)
            [nPairs, ~] = size(Hest);
            assert(numel(self.weight) == nPairs);
        end
    end
end