%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%     This function calculates the channel response by squaring the IQ-samples.
% Description end

classdef ChannelReconstruct_square < ChannelReconstructAbstract
    methods
        function self = ChannelReconstruct_square()
            self.ScalingFactor = 0.5;                                        % The scalingFactor with which the calculated distance must be scaled.
        end
    end
    
    methods
        function CRout = do(self, IQ_A, IQ_B)
            [IQ_A, IQ_B] = self.parse_inputs(IQ_A, IQ_B);
            nAnt = size(IQ_A,1);
            
            for iCh=1:nAnt
                H_est = IQ_A(iCh,:).*IQ_B(iCh,:);
                
                CRout.Uncertainty(iCh,:) = NaN;
                CRout.Hest(iCh,:)        = H_est;
                CRout.ProbCorrectSign(iCh,:) = NaN;
            end
        end
    end
end
