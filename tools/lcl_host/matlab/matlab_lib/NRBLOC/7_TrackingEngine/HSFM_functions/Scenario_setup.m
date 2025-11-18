%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file implements the map of the environment for Tracking engine.
%   The output is a matrix that each row shows the start and end points of
%   a wall. One can add own map to this class providing correctly
%   implementing walls coordinates.
% Description end

classdef Scenario_setup
    properties
        
    end
    
    methods (Static)
        function Map = open_space(varargin)
            %% Scene set-up: Corridor
            % The Polygon of the map of environment should be defined
            % by the x and y coordinates of each vertex
            % A gap, e.g. door, can be defined by defining its
            % beginning and end points as two vertices and another
            % vertix with NaN values between them
            % both x and y coordinates
            % Last pairs: end
            if ~nargin
                NumberOfWalls = 4;
                Map.map_points = [0 0; 0 30; 30 30; 30 0; 0 0];
            else
                RoomPoly = varargin{1};
                NumberOfWalls = length(RoomPoly.Vertices);
                Map.map_points = [RoomPoly.Vertices; RoomPoly.Vertices(1,:)];
            end
            Map.Walls = zeros(NumberOfWalls,4);
            Map.Walls = Scenario_setup.Pgon2wall(Map.map_points);
        end
        
        function W = Pgon2wall(V)
            cnt = 1;
            beg = V(cnt,:);
            while cnt<size(V,1)
                if not(isnan(V(cnt+1,:)))
                    W(cnt,:) = [V(cnt,:), V(cnt+1,:)];
                else
                    V(cnt+1,:) = [];
                    W(cnt,:) = [V(cnt,:), beg];
                    beg = V(cnt+1,:);
                end
                cnt = cnt+1;
            end
            W(cnt,:) = [V(cnt,:), beg];
        end
    end
end
