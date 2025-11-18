%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef testWindow < matlab.unittest.TestCase
    properties (TestParameter)
        FuncHandles1 = struct('Hann', {{@hann, @signal.hann}}, ...
                             'Hamming', {{@hamming, @signal.hamming}}, ...
                             'Blackman', {{@blackman, @signal.blackman}}, ...
                             'BlackmanHarris', {{@blackmanharris, @signal.blackmanharris}})   % No rectwin, because it does not support sflag input
        FuncHandles2 = struct('Hann', {{@hann, @signal.hann}}, ...
                             'Hamming', {{@hamming, @signal.hamming}}, ...
                             'Blackman', {{@blackman, @signal.blackman}}, ...
                             'BlackmanHarris', {{@blackmanharris, @signal.blackmanharris}}, ...
                             'Rectwin', {{@rectwin, @signal.rectwin}}, ...
                             'Tukeywin', {{@tukeywin, @signal.tukeywin}});

        L = num2cell([0, 2.^(0:5), 3.^(1:5)])      % even and odd lengths, also length 0 and length 1
        R = num2cell(-1:0.5:2);
        symflag = {'symmetric', 'periodic'}
    end
   
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testWindowType(self, FuncHandles1, L, symflag)
            w_ml = feval(FuncHandles1{1}, L, symflag);
            w_imec = feval(FuncHandles1{2}, L, symflag);
            self.verifyEqual(w_ml, w_imec, 'abstol', 1e-15);
        end
        
        function testWindow_no_Sflag(self, FuncHandles2, L)
            w_ml = feval(FuncHandles2{1}, L);
            w_imec = feval(FuncHandles2{2}, L);
            self.verifyEqual(w_ml, w_imec, 'abstol', 1e-15);
        end
        
        function testWindow_tukey(self, L, R)
            w_ml = tukeywin(L, R);
            w_imec = signal.tukeywin(L, R);
            self.verifyEqual(w_ml, w_imec, 'abstol', 1e-15);
        end
    end
end
