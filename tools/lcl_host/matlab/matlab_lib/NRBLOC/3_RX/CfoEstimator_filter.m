% Description
%   This implements a CFO-estimator which low-pass-filters the
%   CFO-estimated-values, which are estimated by the ranging platform. By
%   setting any of the filter-objects to dfilt.scalar(1), the function is
%   effectively disables.
% Description end
%
%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Pepijn Boer (pepijn.boer@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------

classdef CfoEstimator_filter < CfoEstimatorAbstract
    properties
        OutlierFilter    = dsp.MedianFilter(5);
        DecimationFilter = dsp.FIRDecimator(2, ones(32,1)/32);
    end
    
    methods
        function set.OutlierFilter(self,val)
            assert(isa(val, 'function_handle') | issystemobject(val) | isfilterobject(val), sprintf('OutlierFilter should either be function-handle, a dsp-object or a filter-object, but is a %s instead', class(val)))
            self.OutlierFilter = val;
        end
        
        function set.DecimationFilter(self,val)
            assert(isa(val, 'function_handle') | issystemobject(val) | isfilterobject(val), sprintf('LPFfilter should either be function-handle, a dsp-object or a filter-object, but is a %s instead', class(val)))
            self.LPFfilter = val;
        end
            
        function CFO = do(self, CFO_A, CFO_B, varargin)
            assert(any(size(CFO_A)==1), sprintf('Expected CFO_A to be a 1D-vector, but it has %d dimensions instead', sum(size(CFO_A)~=1)));
            assert(any(size(CFO_B)==1), sprintf('Expected CFO_B to be a 1D-vector, but it has %d dimensions instead', sum(size(CFO_B)~=1)));
            CFO_A = CFO_A(:);                                               % Create column-vector
            CFO_B = CFO_B(:);                                               % Create column-vector
            assert(length(CFO_A) == length(CFO_B), sprintf('Expected the length of CFO_A and CFO_B to be the same, but length(CFO_A)=%d and length(CFO_B)=%d', length(CFO_A), length(CFO_B)));
            x = reshape([CFO_A, -CFO_B].', [], 1);                          % Interleave the two vectors

            
            y   = self.filter(self.OutlierFilter, x);
            CFO = self.filter(self.DecimationFilter, y);
        end
        
        function reset(self)
           if self.issystemobject(self.OutlierFilter) || self.isfilterobject(self.OutlierFilter)
               self.OutlierFilter.reset;
           end 
           if self.issystemobject(self.DecimationFilter) || self.isfilterobject(self.DecimationFilter)
               self.DecimationFilter.reset;
           end
        end
        
        function release(self)
            if self.issystemobject(self.OutlierFilter)
                self.OutlierFilter.release;
            end
            if self.issystemobject(self.DecimationFilter)
                self.DecimationFilter.release;
            end            
        end
    end
    
    methods (Static, Hidden)
        function result = issystemobject(x)
            result = startsWith(class(x), 'dsp');
        end
        
        function result = isfilterobject(x)
            result = startsWith(class(x), 'dfilt') | startsWith(class(x), 'mfilt');
        end
        
        function y = filter(obj, x)
           if isa(obj, 'function_handle')
               y = feval(obj, x);
           elseif CfoEstimator_filter.issystemobject(obj)
               y = obj.step(x);
           else
               y = obj.filter(x);
           end
        end
    end
end