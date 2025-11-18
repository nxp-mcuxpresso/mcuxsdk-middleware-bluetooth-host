%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef spectrumTestShared < handle
    % This is a mix-in class. It is expected that the main-class inherits
    % from matlab.unittest.TestCase.
    methods
        function spectrumtest_shared(self, Cfg)
            % The function is shared between testPhaseNoise and testAdditiveNoise
            fc           = 32e6;
            N            = Cfg.N;
            NFFT         = Cfg.NFFT;
            PLOT         = Cfg.PLOT;
            loglevel     = matlab.unittest.Verbosity.Detailed;            
            fs           = 1/Cfg.deltaT;
            f_low        = 1e-4 * fs;
            powerRefFreq = 15e-4 * fs;                                        % There must be enough room between f_low and the powerRefFreq, because the filter response needs some room the achieve the required slope. Most critical point: alpha=2
            f_high       = 0.2 * fs / Cfg.upsample_factor;
            
            % In this analysis we need at least a few FFT-samples before
            % pn.f_low. Otherwise the result will be wrong, due to DC. I
            % assume we need at least 6 samples before f_low.
            assert((f_low / (fs/NFFT))>6, sprintf('It is expected there are at least 6 FFT-samples before f_low, however there are only %d samples before f_low. Please increase NFFT', ceil(f_low / (fs/NFFT))));
            
            self.log(loglevel, sprintf('alpha = %.1f, dBm = %.1fdBc/Hz, f_ref = %.1fHz fs = %.1fkHz, upsample_factor = %d', Cfg.Alpha, Cfg.powerVal, powerRefFreq, 1/Cfg.deltaT/1e3, Cfg.upsample_factor))
            
            noiseObj = feval(Cfg.noiseObj, 'PowerValue', Cfg.powerVal, ...
                                     'powerRefFreq', powerRefFreq, ...
                                     'noiseColor', 'custom', ...
                                     'alpha', Cfg.Alpha, ...
                                     'impedance', Cfg.impedance, ...
                                     'nSections', 5, ...
                                     'f_low', f_low, ...
                                     'f_high', f_high, ...
                                     'fs', 1/Cfg.deltaT,...
                                     'impedance', Cfg.impedance, ...
                                     'upsample_factor', Cfg.upsample_factor);
            
            isanobj = strcmpi(func2str(Cfg.noiseObj), 'signal.AdditiveNoise');
                                 
            if isanobj
                x = Signal_BB(zeros(1,N), fc, Cfg.deltaT, Cfg.impedance);   % Generate a baseband-equivalent signal at frequency fc
            else
                x = Signal_BB(ones(1,N), fc, Cfg.deltaT, Cfg.impedance);    % Generate a baseband-equivalent signal at frequency fc
                x = SetMeanPower_dBm(x, 0);
            end                
            y = noiseObj.do(x);
            
            % The first samples are not filtered fully, because of the bandwidth of the filter.
            % Therefore those are removed.
            x = x(1e3:end);
            y = y(1e3:end);            

            [freq, Py] = pwelch(y, 'NFFT', NFFT, 'freqrange', 'centered', 'plotUnitsY', 'dBm');
            Py         = Py(freq>0);
            Px         = 0;
            if ~isanobj
                [~, Px] = pwelch(x, 'NFFT', NFFT, 'freqrange', 'centered', 'plotUnitsY', 'dBm');
                Px      = Px(freq>0);
            end            
            freq = freq(freq>0);
            
            actualNoise = 10*log10(abs(Px-Py));                             % Simulated noise
            
            if isanobj
                if strcmpi(Cfg.method, 'closed_form')
                    expectedNoise = 10*log10(noiseObj.PSD_closed_form(freq)); % Expected noise, based on filter-transfer-function
                else
                    expectedNoise  = noiseObj.powerValue - noiseObj.alpha*10*log10(freq/noiseObj.powerRefFreq);
                end
            else
                if strcmpi(Cfg.method, 'closed_form')
                    expectedNoise = 10*log10(noiseObj.Spectrum_PhaseNoise_closed_form(freq)); % Expected noise, based on filter-transfer-function
                else
                    b0_dBc_Hz     = Cfg.powerVal + Cfg.Alpha*10*log10(powerRefFreq) - 10*log10(Cfg.upsample_factor);
                    b0            = 10^(b0_dBc_Hz/10);                          % Translate the desired power-density at powerRefFreq to 1Hz
                    expectedNoise = (10*log10(b0)-Cfg.Alpha*10*log10(abs(freq)));  % Expected noise, based on configuration
                end
            end
            
            Err  = actualNoise - expectedNoise;
            indx_analysis = inrange(freq, [Cfg.f_low_scaling*noiseObj.f_low, noiseObj.f_high], 'excludeboth'); % The designed filter has a passband width of f_low. The slope of the filter is assumed to be correct at f_low_scaling*f_low.
            
            if (max(abs(Err(indx_analysis))) > Cfg.abstol) && PLOT
                Xscale    = 'log';                                          % 'log' or 'lin'
                freqscale = 1000;                                           % Frequency axis in 'kHz'
                
                figure;
                ax(1) = subplot(2,1,1); 
                plot(freq/freqscale, 10*log10(Py(freq>0)), '-', 'displayname', 'Signal (full)');
                hold on;
                plot(freq(indx_analysis)/freqscale, 10*log10(Py(indx_analysis)), '-', 'displayname', 'Signal (for analysis)');
                plot(freq(indx_analysis)/freqscale, expectedNoise(indx_analysis) + Cfg.abstol,'k-', 'linewidth', 1.5, 'displayname', 'upper-limit');
                plot(freq/freqscale, expectedNoise(freq>0),'k--', 'displayname', 'Expected');
                plot(freq(indx_analysis)/freqscale, expectedNoise(indx_analysis) - Cfg.abstol,'k-', 'linewidth', 1.5, 'displayname', 'lower-limit');
                plot(powerRefFreq/freqscale, Cfg.powerVal, 'gs', 'MarkerFaceColor', 'g', 'markersize', 6, 'displayname', 'reference freq');
                legend show;
                xlabel('Frequency [kHz]');
                ylabel('PSD [dBc/Hz]');
                title('PSD of signal');
                set(gca, 'XScale', Xscale);
                
                ax(2) = subplot(2,1,2); hold on;
                plot(freq/freqscale, abs(Err(freq>0)), 'displayname', 'Error (full)');
                plot(freq(indx_analysis)/freqscale, abs(Err(indx_analysis)), 'displayname', 'Error (for analysis)');
                plot(freq(indx_analysis)/freqscale, Cfg.abstol*ones(sum(indx_analysis),1), 'k', 'linewidth', 1.5, 'displayname', 'Tolerance');
                xlabel('Frequency [kHz]');
                ylabel('PSD [dBc/Hz]');
                title('PSD of error');
                set(gca, 'XScale', Xscale);
                linkaxes(ax, 'x')
            end
            self.log(loglevel, sprintf('The maximum absolute error is %.2f (abstol = %.2f)', max(abs(Err(indx_analysis))), Cfg.abstol));
            self.verifyEqual(actualNoise(indx_analysis), expectedNoise(indx_analysis), 'abstol', Cfg.abstol, sprintf('The maximum absolute error is %.2f', max(abs(Err(indx_analysis)))));
        end
    end
end
