%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef NRBLOC_PA
% Description
%  Simple Power amplifier model, the TX power can be set as well 
%  as the time-instants at which the PA is being activated (t_activateStart)
%  and for how long(t_duration). 
%  The output of the env-method is the signal voltage at Impedance (default
%  50 Ohm.)
%  Note that there is no check whether t_duration-intervals overlap! 
% Description end
    
    properties
        t_activateStart
        t_duration
        Power_dBm
        Impedance
    end
    
    methods
        function self = NRBLOC_PA(varargin)
            p = inputParser();
            p.KeepUnmatched = true;
            p.addParameter('t_activateStart', [250e-6, 500e-6], @(x) validateattributes(x, {'numeric'}, {'vector', 'positive'}) );
            p.addParameter('t_duration', 30e-6, @(x) validateattributes(x, {'numeric'}, {'scalar','positive'}) );
            p.addParameter('Power_dBm', 0, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
            p.addParameter('Impedance', 50, @(x) validateattributes(x, {'numeric'}, {'scalar','positive'}) );
            p.parse(varargin{:});
            self.t_activateStart = p.Results.t_activateStart;
            self.t_duration = p.Results.t_duration;
            self.Power_dBm = p.Results.Power_dBm;
            self.Impedance = p.Results.Impedance;
        end        
        
        function self = delay(self, varargin)
            % Delays the activation of the PA
            p = inputParser();
            p.KeepUnmatched = true;
            p.addRequired('tau', @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
            p.parse(varargin{:});
           
            self.t_activateStart = self.t_activateStart + p.Results.tau;
        end
        
        function env = env(self, t)
            % Calculate the signal-envelope at each time in vector t
            validateattributes(t, {'numeric'}, {'vector'}, 'env', 't', 1);
            
            Env_Volt = 10^((self.Power_dBm-30)/20)*sqrt(self.Impedance);    % (Vrms)
            I_old    = 1;
            env      = zeros(size(t));
            for cnt_t = 1:length(self.t_activateStart)
                I1         = find( t(I_old:end)>self.t_activateStart(cnt_t), 1, 'first') + I_old - 1;
                I2         = find( t(I1:end)<=self.t_activateStart(cnt_t)+self.t_duration, 1, 'last') + I1 - 1;
                env(I1:I2) = Env_Volt;
                I_old = I2;
            end
        end        
    end
end

