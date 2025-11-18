%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Creat a trajectory for testing the GM modeling and on-the-fly
%   calibration
% Description end

classdef trajectory_gen
    
    properties
        shape
        step_size
        radius
        Anch
        pos_xy
    end
    
    methods
        function obj = trajectory_gen(shape,step_size,Anch,radius)
            
            if nargin<1
                shape='square';
            end
            
            if nargin<2
                step_size=0.5;
            end
            
            if nargin<3
                error(message('Anchor positions should be provided!'));
            end
            
            if nargin<4
                radius=max(max(Anch))-1;   
            end              
            
            obj.shape = shape;
            obj.step_size = step_size;
            obj.Anch = Anch;
            obj.radius = radius;            
            
            if strcmp(shape,'square')
                obj=square_trajectory(obj);
            elseif strcmp(shape,'circle')
                obj=circle_trajectory(obj);
            else
                error(message('Shape format is not supported!'))
            end
            
        end
        
        function obj = square_trajectory(obj)
            
            
                min_x=-obj.radius;
                min_y=-obj.radius;                
                max_x=obj.radius;
                max_y=obj.radius;                
                
                X_grid=[min_x:obj.step_size:max_x];
                Y_grid=[min_y:obj.step_size:max_y];
                x_is=[X_grid(1)*ones(numel(Y_grid(:)),1) Y_grid(:)];
                x_is=[x_is;[X_grid(2:end).' (Y_grid(end))*ones(numel(X_grid(:))-1,1)]];
                x_is=[x_is;[(X_grid(end))*ones(numel(Y_grid(:))-1,1) Y_grid(end-1:-1:1).']];
                x_is=[x_is;[X_grid(end-1:-1:1).' (Y_grid(1))*ones(numel(X_grid(:))-1,1)]];
                obj.pos_xy=x_is;

        end
        
        function obj = circle_trajectory(obj)
            
            center=mean(obj.Anch);
            length_circ=2*pi*obj.radius/obj.step_size;
            samp_vec=(1:length_circ)'/length_circ;
            x_is=center+obj.radius*[cos(2*pi*samp_vec) sin(2*pi*samp_vec)];
            obj.pos_xy=x_is;

        end        
        
        
        
        
    end
end

