%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file implements the range-estimator, which takes the measurements
%   from the front-end and converts it into a distance, velocity, angle and/or
%   angular velocity, depending on the configuration of the
%   range-estimator.
% Description end

classdef RangeEstimator < Util
    properties
        ChannelReconstructor
        RangingEngine
        CfoEstimator
        CfoCompensator
        TrackingEngineDistance
        TrackingEngineAngle
        ChannelResponseWeighting
        QiRicianK
        QiReciprocity
        QiIq
        Diagnostics
    end
    
    properties (Dependent, Hidden)
        deltaF
    end
    
    methods
        function val = get.deltaF(self)
           val = self.RangingEngine.delta_F;
        end
        
        function set.deltaF(self, val)
            self.RangingEngine.delta_F        = val;
            self.ChannelReconstructor.delta_F = val;
            self.CfoCompensator.delta_f       = val;
            self.QiRicianK.delta_F            = val;
        end
        
        function self = RangeEstimator(varargin)
            p = inputParser;
            p.addParameter('ChannelReconstructor', ChannelReconstruct_NonPhaseCoherent_cpp(), @(x) validateattributes(x, {'ChannelReconstructAbstract'}, {}) );
            p.addParameter('RangingEngine', RangingEngine_Music_cpp(), @(x) validateattributes(x, {'RangingEngineAbstract'}, {}) );
            p.addParameter('CfoEstimator', CfoEstimator_filter(), @(x) validateattributes(x, {'CfoEstimatorAbstract'}, {}) );
            p.addParameter('CfoCompensator', CfoCompensatorConstant(), @(x) validateattributes(x, {'CfoCompensatorAbstract'}, {}) );
            p.addParameter('TrackingEngineDistance', Estimator_Passthrough(), @(x) validateattributes(x, {'TrackingEngineAbstract'}, {}) );
            p.addParameter('TrackingEngineAngle', Estimator_Passthrough(), @(x) validateattributes(x, {'TrackingEngineAbstract'}, {}) );
            p.addParameter('ChannelResponseWeighting', ChannelResponseWeightingEqual(), @(x) validateattributes(x, {'ChannelResponseWeighting'}, {}) );
            p.addParameter('QiRicianK', QualityIndicator_RicianK(), @(x) validateattributes(x, {'QualityIndicatorAbstract'}, {}) );
            p.addParameter('QiReciprocity', QualityIndicator_Reciprocity(), @(x) validateattributes(x, {'QualityIndicatorAbstract'}, {}) );
            p.addParameter('QiIq', QualityIndicator_IQ(), @(x) validateattributes(x, {'QualityIndicatorAbstract'}, {}) );
            p.addParameter('deltaF', 1e6, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', '>=', 500e3, '<=' 4e6}, mfilename, 'deltaF'));
            p.parse(varargin{:});
            
            self.ChannelReconstructor = p.Results.ChannelReconstructor;
            self.RangingEngine = p.Results.RangingEngine;
            self.CfoEstimator = p.Results.CfoEstimator;
            self.CfoCompensator = p.Results.CfoCompensator;
            self.TrackingEngineDistance = p.Results.TrackingEngineDistance;
            self.TrackingEngineAngle = p.Results.TrackingEngineAngle;
            self.ChannelResponseWeighting = p.Results.ChannelResponseWeighting;
            self.QiRicianK = p.Results.QiRicianK;
            self.QiReciprocity = p.Results.QiReciprocity;
            self.QiIq = p.Results.QiIq;
            self.deltaF = p.Results.deltaF;
        end
        
        function varargout = do(self, varargin)
            p = inputParser;
            % Check that required inputs are numeric, not empty, and do not contain any Inf or NaN 
            p.addRequired('Z_I', @(x) validateattributes(x, {'numeric'}, {'nonempty', 'nonnan', 'finite'}, 'do', 'Z_I', 1) );
            p.addRequired('Z_R', @(x) validateattributes(x, {'numeric'}, {'nonempty', 'nonnan', 'finite'}, 'do', 'Z_R', 2) );
            p.addOptional('CFO', [0,0], @(x) validateattributes(x, {'numeric'}, {'nonempty', 'nonnan', 'finite'}, 'do', 'CFO', 3) );
            p.parse(varargin{:});
            
            % Do some more checks and prepare the input data.
            [Z_I, Z_R] = self.parseInputs(p.Results.Z_I, p.Results.Z_R);
            [nPairs, ~] = size(Z_I);
            
            % Do quality-estimation based on raw-input-data
            CfoEst   = self.CfoEstimator.do(p.Results.CFO(1), p.Results.CFO(2));
            QIoutput = self.QiIq.do(Z_I, Z_R);
            temp     = self.QiReciprocity.do(Z_I, Z_R);
            QIoutput = self.mergeStructs(QIoutput, temp);
                        
            % Do CFO correction
            self.CfoCompensator.Cfo = CfoEst;
            [Z_I_cfo, Z_R_cfo] = self.CfoCompensator.do(Z_I, Z_R);
            
            % Do Channel-reconstruction
            CRout = self.ChannelReconstructor.do(Z_I_cfo, Z_R_cfo);
            
            % Do weighting
            CRout.Hest = self.ChannelResponseWeighting.do(CRout.Hest);
            
            % Calculate distance and possibly angle-of-arrival
            [dist_raw, aoa_raw] = self.RangingEngine.do(CRout);
            dist_raw = self.distance_unwrap(dist_raw, self.RangingEngine.delta_F, self.ChannelReconstructor.ScalingFactor);
            
            % Do RicianK estimation
            self.QiRicianK.owcr = self.ChannelReconstructor.ScalingFactor==1;    % Is one-way channel-response?
            QIoutput.RicianK = self.QiRicianK.do(CRout.Hest, dist_raw*ones(nPairs,1));
            
            % Execute distance- and angle-tracking
            Ts = nan;   %FIXME: where should Ts come from? 
            [dist, velocity] = self.TrackingEngineDistance.do(dist_raw, Ts);
            [aoa, ang_velocity] = self.TrackingEngineAngle.do(aoa_raw, Ts);                  

            QIoutput = self.mergeStructs(QIoutput, CRout);
            QIoutput.Distance = dist;
            QIoutput.Velocity = velocity;
            QIoutput.AoA = aoa;
            QIoutput.AngularVelocity = ang_velocity;
            QIoutput.CfoEst = CfoEst;
            QIoutput.DistanceRaw = dist_raw;
            QIoutput.AoARaw = aoa_raw;
            
            varargout{1} = dist;
            varargout{2} = QIoutput;
            varargout{3} = CRout.Hest;            
        end
        
        function S = str(self)
            % This function creates a string representation of this class
            CRstr = class(self.ChannelReconstructor);
            REstr = class(self.RangingEngine);
            CRstr = strrep(CRstr, 'ChannelReconstruct', 'CR');
            CRstr = strrep(CRstr, '_', '-');
            REstr = strrep(REstr, 'RangingEngine', 'RE');
            REstr = strrep(REstr, '_', '-');
            S = sprintf('%s / %s', CRstr, REstr);
        end
%         function CP = copy(self)
%             CP = RangeEstimator(copy(self.ChannelReconstructor), copy(self.RangingEngine));
%         end
    end
    
    methods (Static)
        function [Z_A, Z_B] = parseInputs(varargin)
            % This function makes sure that the inputs are:
            % 1) of type double
            % 2) of the same size
            % 3) oriented correctly, with nPairs x nTones
            
            % 1) convert to double
            Z_A = double(varargin{1});                                      % Input might still be an int32
            Z_B = double(varargin{2});
            
            % 2) Check shape of both inputs
            validateattributes(Z_B, {'numeric'}, {'size' size(Z_A)}, 'parseInputs', 'Z_B', 2);
            
            % 3) Transpose if wrong orientation
            if size(Z_A,1)>size(Z_A,2)
                Z_A = Z_A.';
                Z_B = Z_B.';
            end
        end
        
        function s1 = mergeStructs(s1, s2)
            % append fields from s2 to s1. Overwrites existing values.
            f = fieldnames(s2);
            for i = 1:length(f)
                s1.(f{i}) = s2.(f{i});
            end
        end
        
        function dist = distance_unwrap(dist, delta_f, scaling_factor)
            dist = dist*scaling_factor;
            if scaling_factor == 1 % one-way-channel response
                amb_bound = physconst('lightspeed') / (2*delta_f);
            else
                amb_bound = physconst('lightspeed') / delta_f;
            end
            if dist>amb_bound
                dist = mod(dist, amb_bound);
            end
        end
    end
end
