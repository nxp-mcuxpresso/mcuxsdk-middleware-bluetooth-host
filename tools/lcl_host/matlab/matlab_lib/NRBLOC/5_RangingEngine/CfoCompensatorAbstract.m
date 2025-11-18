%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%  This function applies three corrections:
%   1. XTAL-offset correction for successive antenna sampling
%   2. XTAL-offset correction for successive frequency sampling
%   3. Correction for the order in which the antenna’s are sampled.

%  Corrections 1 and 3 are only effective when a ULA is used and the angle-of-arrival is calculated.
%  Correction 1 is applied to both data-stream (IQ_A and IQ_B) and is needed to support the
%  channel-reconstruct to do its job.
%  Correction 2 is applied to the data-stream of the initiator only (IQ_A).
%  The 3rd correction is needed to negate the calculated angle-of-arrival and is applied to
%  both data-streams (IQ_A and IQ_B).
% Description end

classdef CfoCompensatorAbstract < Util
    properties
        Cfo = 0                                                             % (Hz) Carrier Frequency Offset
        delta_f = 1e6;                                                      % (Hz) frequency distance 
        % The two numbers delay are software dependent. They can be found
        % in the firmware implementation document. For NXP the documents can
        % be found in C07S301 and the values are in table "Measurement
        % Roles Timing elements".
        % For FW254: 
        %  * InterDelay = T_DCOCOMP+T_MSETUP+T_MACQ+T_MPROC_END 
        %  * IntraAntennaDelay = T_MACQ + T_MPROCESS
        IntraDelay        = 172e-6;                                         % (s) Intra delay; time between two consecutive samples on the initiator/reflector assuming one antenna pair
        IntraAntennaDelay = 25e-6;                                          % (s) Intra antenna delay; time between two samples from antenna pair n and n+1 on the initiator/reflector
    end

    methods
        function y = apply_spatial_domain_correction(self, x, CfoVal)
            [nPairs, ~] = size(x);
            
            y = zeros(size(x));
            for k=1:nPairs
                exp_vector = exp(-2j * pi * CfoVal*self.IntraAntennaDelay * (k-1));
                y(k,:) = x(k,:) .* exp_vector;
            end
        end
        
        function y = apply_frequency_domain_correction(self, x, CfoVal)
            [nPairs, nTones] = size(x);
            
            XtalOffset = self.cfo2xtaloffset(CfoVal);                       % (ppm)
            CfoValVect = XtalOffset * (0 : self.delta_f : (nTones-1)*self.delta_f);
            exp_vector = exp(-2j * pi * CfoValVect * self.IntraDelay);
            
            y = zeros(size(x));            
            for k=1:nPairs
                y(k,:) = x(k,:) .* exp_vector;
            end
        end
    end
    
    methods (Static)
        function xval = cfo2xtaloffset(cval, ref_frequency)
            if nargin==1
                % At the NXP KW36, the CFO is measured at f_start, which by default is set to 2.40GHz
                ref_frequency = 2.40e9;                
            end
            xval = cval ./ ref_frequency;
        end
    end
end