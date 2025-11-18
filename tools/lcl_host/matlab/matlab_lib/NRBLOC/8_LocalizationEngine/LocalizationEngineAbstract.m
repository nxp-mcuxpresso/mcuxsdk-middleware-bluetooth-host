%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is the abstract-class for localization algorithms.
% Description end
classdef (Abstract) LocalizationEngineAbstract < Util
    properties
        anchors_pos
    end
    
    properties (Dependent)
        nAnchors
        nDim
    end
    
    methods
        function val = get.nAnchors(self)
            val = size(self.anchors_pos, 2);
        end
        
        function val = get.nDim(self)
            val = size(self.anchors_pos,1);
        end
        
        function reset(~)
            % do nothing
        end
        
        function self = LocalizationEngineAbstract(varargin)
            p = inputParser();
            p.addParameter('anchors_pos', [0 0; 1 0; 1 1; 0 1].', @(x) validateattributes(x, {'numeric'}, {'real', '2d', 'nonsparse', 'nonempty'}, mfilename, 'anchors_pos'));
            p.parse(varargin{:});
            
            % Assign the properties dynamically
            FN = fieldnames(p.Results);
            for k=1:length(FN)
                self.(FN{k}) = p.Results.(FN{k});
            end
        end
        
        function value = NumDim(self)
            warning('Please start using the property nDim, this function may be removed in later revisions');
            value = self.nDim;
        end
        
        function value = NumAnchors(self)       % number of anchors
            warning('Please start using the property nAnchors, this function may be removed in later revisions');
            value = self.nAnchors;
        end
        
        function set.anchors_pos(self, P) % set position of anchors
            % Each column of P contains the location of 1 anchor.
            if ~ismember(size(P,1),[2 3])
                error('only suited for 2 or 3D localization')
            else
                self.anchors_pos = P;
            end
        end        
    end
    
    methods (Abstract)
        Out = do(self, distance_est);
    end    
end