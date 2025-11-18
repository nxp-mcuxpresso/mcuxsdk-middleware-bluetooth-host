%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a cascade of digital-filters. It should act as a
%   substitute for the dsp.FilterCascade-class in Matlab. The goal of this 
%   class is to have a license-free substitute.
% Description end

classdef FilterCascade < digfilt.digfiltabstract
    properties
        Stage
    end
    
    methods 
        function self = FilterCascade(varargin)    % constructor
            self.addstage(varargin{:});
        end
        
        function [num, den] = tf(self)
            % calculates the transfer-function of the cascade of filters
            num = 1;
            den = 1;

            %TODO: At this moment only a cascade of single-rate and/or interpolators are supported
            for k = find([self.Stage.ismultirate])
                assert(isa(self.Stage(k), 'digfilt.FIRInterpolator'))
            end
            
            for k=1:length(self.Stage)
                if isa(self.Stage(k), 'digfilt.FIRInterpolator')
                    I = self.Stage(k).InterpolationFactor;
                    num(1:I:I*length(num)) = num;                               % upsample the transfer-function
                    den(1:I:I*length(den)) = den;                               % upsample the transfer-function
                end
                [N, D] = self.Stage(k).tf();
                num = conv(num, N);
                den = conv(den, D);
            end
        end
        
        function reset(self)
           for k=1:length(self.Stage)
              self.Stage(k).reset(); 
           end
        end
        
        function y = step(self, x)
            y = x;
            for k=1:length(self.Stage)
                y = self.Stage(k).step(y);
            end
        end
        
        function addstage(self, varargin)
            for k=1:length(varargin)
                temp = varargin{k};
                validateattributes(temp, {'digfilt.digfiltabstract'}, {}, 'FilterCascade');
                if isa(temp, 'digfilt.FilterCascade')
                    self.Stage = [self.Stage; temp.Stage(:)];
                else
                    self.Stage = [self.Stage;temp];
                end
            end
        end
        
        function [H, w] = freq_response_calc(self, opts)
            if any([self.Stage.ismultirate])
                % Check inputs
                for k=1:length(self.Stage)
                    if self.Stage(k).ismultirate
                        assert(isa(self.Stage(k), 'digfilt.FIRInterpolator'));   % only FIRinterpolators are currently supported
                    end
                end
                I = self.all_interpolation_factors();
                
                H = ones(opts.N, 1);
                for k=1:length(self.Stage)
                    I_togo = prod(I(k+1:end)); % The total upsampling factor after this filter

                    opts_local          = opts;
                    opts_local.w        = opts.w * I_togo;
                    opts_local.freqspec = true;
                    [temp, w] = self.Stage(k).freq_response_calc(opts_local);
                    H = temp .* H;
                end
            else
                H = ones(opts.N, 1);
                for k=1:length(self.Stage)
                    [temp, w] = self.Stage(k).freq_response_calc(opts);
                    H = temp .* H;
                end
            end
        end
        
        function val = total_interpolation_factor(self)
            I(~[self.Stage.ismultirate]) = 1;
            if any([self.Stage.ismultirate])
                I([self.Stage.ismultirate])  = [self.Stage([self.Stage.ismultirate]).InterpolationFactor];
            end
            val = prod(I);
        end
        
        function I = all_interpolation_factors(self)
            I(~[self.Stage.ismultirate]) = 1;
            I([self.Stage.ismultirate])  = [self.Stage([self.Stage.ismultirate]).InterpolationFactor];
        end
   end
end