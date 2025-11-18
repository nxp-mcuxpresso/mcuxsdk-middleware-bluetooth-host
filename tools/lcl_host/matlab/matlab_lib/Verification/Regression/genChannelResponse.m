%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class can be used as mixin-class to provide the function to 
%   generate a channel-response.
% Description end

classdef genChannelResponse < handle
    properties
        deltaF
        nTones
        f_start
        RicianK
        nAntPairs
        DistActual
        NrTests
    end
    
    properties (SetAccess = private)
        Hactual
        IQ_A_nc         % Non-phase-coherent data as measured by device A
        IQ_B_nc         % Non-phase-coherent data as measured by device B 
        IQ_A_c          % Phase-coherent data as measured by device A
        IQ_B_c          % Phase-coherent data as measured by device B
        Channel_2D
    end        
    
    methods
        function self = genChannelResponse(varargin)
            p = inputParser;
            p.KeepUnmatched = true;
            p.addParameter('DistActual', 10, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'DistActual') );
            p.addParameter('RicianK', 1, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'RicianK') );
            p.addParameter('NrTests', 100, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'NrTests') );
            p.addParameter('nAntPairs', 1, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'nAntPairs') );
            p.addParameter('deltaF', 1e6, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'deltaF') );
            p.addParameter('nTones', 80, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'nTones') );
            p.addParameter('f_start', 2.4e9, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar'}, mfilename, 'f_start') );
            p.parse(varargin{:});

            % Assign the properties dynamically
            FN = fieldnames(p.Results);
            for k=1:length(FN)
                self.(FN{k}) = p.Results.(FN{k});
            end   
        end
        
        function self = generate_channel(self)
            Out = generate_channel(self.NrTests, self.DistActual, self.RicianK, 'delta_F', self.deltaF, 'Kf', self.nTones, 'f_start', self.f_start);
            
            self.Hactual = Out.Hactual;
            self.IQ_A_nc = Out.IQ_A_nc;
            self.IQ_B_nc = Out.IQ_B_nc;
            self.IQ_A_c  = Out.IQ_A_c;
            self.IQ_B_c  = Out.IQ_B_c;
        end
        
        function self = generate_channel_2D(self)
            [Out_2D, Out_1D] = generate_channel_2D(self.NrTests, self.DistActual, self.RicianK, self.nAntPairs, 'delta_F', self.deltaF, 'Kf', self.nTones, 'f_start', self.f_start);
            self.Channel_2D  = Out_2D;

            self.Hactual = Out_1D.Hactual;
            self.IQ_A_nc = Out_1D.IQ_A_nc;
            self.IQ_B_nc = Out_1D.IQ_B_nc;
            self.IQ_A_c  = Out_1D.IQ_A_c;
            self.IQ_B_c  = Out_1D.IQ_B_c;            
        end        
    end
end

