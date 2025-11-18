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
classdef ChannelReconstruct_NonPhaseCoherent_cpp < ChannelReconstructAbstract
    properties
    end
    
    methods
        function CRout = do(self, IQ_A, IQ_B, varargin)
            [IQ_A, IQ_B, ~] = self.parse_inputs(IQ_A, IQ_B, varargin);
            nAnt = size(IQ_A,1);
            
            for iCh=1:nAnt
                [H_est, ~, Uncertainty] = mexCrNonPhaseCoherent(IQ_A(iCh,:),IQ_B(iCh,:));
                
                % Probability of correct reconstruction of each tone
                ProbCorrectSign = (abs(H_est/max(abs(H_est)))>0.05);

                CRout.Uncertainty(iCh,:) = Uncertainty;
                CRout.Hest(iCh,:)        = H_est;
                CRout.ProbCorrectSign(iCh,:) = ProbCorrectSign;
           end
        end
    end
end
