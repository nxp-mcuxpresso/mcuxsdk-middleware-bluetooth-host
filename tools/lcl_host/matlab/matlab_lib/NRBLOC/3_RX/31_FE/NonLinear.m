%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef NonLinear < Util
    % Description
    % Non-linear models are used to characterize the behavior of real-world 
    % devices found in analog radio front-ends. Typically, the data-rate is 
    % small compared to the carrier frequency, such that these devices are 
    % adequately modeled using a memoryless non-linear function. The non-linear 
    % behavior of a device is often quantified by gain (G [dB]), 
    % Third-Order Output Intercept Point (OIP3) [dBm]). The input-output relationship 
    % of weakly non-linear devices for baseband-equivalent signals x(t), is 
    % approximated by
    % Description end
    
    properties
        Gain_dB=10      %[dB]
        OIP3_dBm=10     %[dBm]
    end
    properties (Hidden)
    end
    methods
        function NL=NonLinear(G,IP3)%Constructor
            if nargin>0
                NL.Gain_dB=G;
            end
            if nargin>1
                NL.OIP3_dBm=IP3;
            end
        end

        function y=do(NL,x)
            alpha=SetAlphas(NL);
            if alpha(3)~=0 %Non-linear system
                envelope=abs(x);
                phase=angle(x);
                tmp=alpha(1)*envelope+alpha(3)*envelope.^3;
                tmp=tmp.*exp(1i*phase);
            else %Speed up simulations iff linear
                tmp=alpha(1)*x.signal;
            end
            y=x;
            y.signal=tmp;
        end
        
        function alpha=SetAlphas(NL)
            alpha(1)=10^(NL.Gain_dB/20);
            OIP3_lin=10^(NL.OIP3_dBm/10-3);
            alpha(3)=-2/3*alpha(1)^3/OIP3_lin;
        end
        
    end
end
