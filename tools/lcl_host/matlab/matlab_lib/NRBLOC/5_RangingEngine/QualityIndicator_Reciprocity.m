%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class calculates the reciprocity of IQ_A and IQ_B. 
% Description end

classdef QualityIndicator_Reciprocity < QualityIndicatorAbstract
    properties
    end

    methods
        function Out = do(self, IQ_A, IQ_B, varargin)
            IQsize = size(IQ_A);
            [IQ_A, IQ_B, varargin] = self.parse_inputs(IQ_A, IQ_B, varargin);
            ignore = false(size(IQ_A));
            if nargin==4
                assert(all(size(IQ_A) == size(varargin{1})), 'Sizes of IQ_A and IGN must be the same');
                ignore = varargin{1};
            end
            Out = self.calc_reciprocity(IQ_A, IQ_B, ignore);

            if all(size(Out.Reciprocity) ~= IQsize)
                % Assuming IQ and Reciprocity_raw has the same size
                Out.Reciprocity = Out.Reciprocity.';
            end
        end

        function Out = calc_reciprocity(self, IQ_A, IQ_B, ignore)
            % IQ_A, IQ_B; row = measurement, col= nTones
            norma = mean(abs(IQ_A), 2);
            normb = mean(abs(IQ_B), 2);
            magn_a = 20*log10(abs(IQ_A) ./ norma);
            magn_b = 20*log10(abs(IQ_B) ./ normb);
            % using the ratio between magn_a and magn_b has the major
            % drawback that it is very sensitive to the absolute size. i.e.
            % small signals will show a large delta ratio with only small 
            % signal changes. While large signals will show a very small delta ratio.
            % Ideally the reciprocity is more sensitive to variation on
            % large signals than small signals.
            % To cope with that we use a sigmoid function, where x = -6+2*diff_db.
            Out.Reciprocity = 1. ./ (1 + exp(-6+2*abs(magn_a - magn_b)));
        end
    end
end
