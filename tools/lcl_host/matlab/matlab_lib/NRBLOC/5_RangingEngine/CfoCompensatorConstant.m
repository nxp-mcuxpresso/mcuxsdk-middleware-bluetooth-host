%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file implements a CFO-compensator, which removes CFO from the 
%   IQ-values, according to a fixed value.
% Description end

classdef CfoCompensatorConstant < CfoCompensatorAbstract
    properties
        CfoConstant = 0;                                                    % (Hz) Carrier Frequency Offset
    end
    
    methods
        function self = CfoCompensatorConstant(varargin)
            p = inputParser;
            p.addParameter('Cfo', 0, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
            p.addParameter('IntraDelay', 172e-6, @(x) validateattributes(x, {'numeric'}, {'scalar', '<', 2e-3}) );          % Valid for KW36 FW254
            p.addParameter('IntraAntennaDelay', 25e-6, @(x) validateattributes(x, {'numeric'}, {'scalar', '<', 1e-4}) );    % Valid for KW36 FW254
            p.addParameter('delta_f', 1e6, @(x) validateattributes(x, {'numeric'}, {'scalar', '>' 1e5}) );
            p.parse(varargin{:});
            
            self.CfoConstant        = p.Results.Cfo;
            self.IntraDelay         = p.Results.IntraDelay;
            self.IntraAntennaDelay  = p.Results.IntraAntennaDelay;
            self.delta_f            = p.Results.delta_f;
        end
        
        function [Z_A, Z_B] = do(self, Z_A, Z_B)
            %   INPUT:
            %   * Z_A   : IQ-data from the initiator [nPairs x nTones]
            %   * Z_B   : IQ-data from the reflector [nPairs x nTones]
            %
            %   OUTPUT:
            %   * Z_A   : IQ-data from the initiator [nPairs x nTones]
            %   * Z_B   : IQ-data from the reflector [nPairs x nTones]
            validateattributes(Z_B, {'numeric'}, {'size', size(Z_A)}, 'do', 'Z_B', 2)
            
            % Correction 1: XTAL-offset correction for successive antenna sampling
            Z_A = self.apply_spatial_domain_correction(Z_A, self.CfoConstant);
            Z_B = self.apply_spatial_domain_correction(Z_B, -self.CfoConstant);
            
            % Correction 2: XTAL-offset correction for successive frequency sampling
            Z_A = self.apply_frequency_domain_correction(Z_A, self.CfoConstant);
            
            % Correction 3: Correction for the order in which the antenna�s are sampled.
            % FIXME: The flipud is here because of the order in which the antenna's are sampled.
            % This causes a negation of the aoa-value. This function has nothing to do with the CFO-Compensation and
            % should therefore be moved outside this scope.
            % Z_A = flipud(Z_A);
            % Z_B = flipud(Z_B);
        end
    end
end