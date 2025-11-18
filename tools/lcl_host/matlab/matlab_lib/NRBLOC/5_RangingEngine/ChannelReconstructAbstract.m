%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is an abstract-class. It all methods and properties all
%   ChannelReconstructors should have.
% Description end


classdef ChannelReconstructAbstract < matlab.mixin.Copyable
    properties
        ScalingFactor = 1;                                                  % The scalingFactor with which the calculated distance must be scaled.
        delta_F       = 1e6;                                                %[Hz] Frequency difference between tones
    end
    methods (Abstract)
        Out = do(self, varargin);
    end
    
    methods (Static)
        function [IQ_A, IQ_B, varargout] = parse_inputs(IQ_A, IQ_B, varargin) 
            assert(all(size(IQ_A) == size(IQ_B)), 'Sizes of IQ_A and IQ_B must be the same');
            
            nRow = size(IQ_A,1);
            nCol = size(IQ_A,2);
            
            % The channel-reconstructor expects the antennae in the rows and
            % the tones in the columns
            if nRow > nCol          % expected to have more tones than antennae
                IQ_A = IQ_A.';
                IQ_B = IQ_B.';
            end            
            varargout = varargin;
        end
    end
end