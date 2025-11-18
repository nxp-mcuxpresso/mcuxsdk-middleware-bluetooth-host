%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a headed social force model for human motion
%   simulation in a predefined environment. The map and the path planning
%   should be given as inputs to the algorithm. The path planning should
%   give points that can be connected together with straight lines.
%   functions include:
%   HSFM_main: first parameters and other definitions
%   AgentProperties: you can set the poitns to be passed by the agent as
%   well as agent size information (weight and radius)
%   GenerateMap: defines the environment map, note that it can be any shape
%   but each two consecutive points to be passed should be able to be
%   reached by a straight line
% Description end

classdef AgentMotion < TrackingEngineAbstract
    properties %all the properties are public
        HSFMsetup;% = HSFM_setup.SetupDefinition();
        Environment;
        AgentsSetup;
        Agent;
        DestinationReach;
        room_max_x = 7;
        room_max_y = 11.75;
        SamplingTimeStd = 15e-3;
    end
    %%
    methods
        %%
        function self = AgentMotion(varargin)
            % Sampling time
            if nargin == 1
                In = varargin{1};
                if isfield(In,'SamplingTime')
                    self.SamplingTime = In.SamplingTime;
                end
                if isfield(In,'NumberAgents')
                    self.Number_Agents = In.NumberAgents;
                end
                if isfield(In,'RoomPoly')
                    RoomPoly = In.RoomPoly;
                else
                    RoomPoly = polyshape([0 0 self.room_max_x self.room_max_x; ...
                        0 self.room_max_y self.room_max_y 0]');
                end
%             else
%                 Ts = 0.1;
%                 N_Agent = self.Number_Agents;
            end
            
            self.HSFMsetup = HSFM_setup('Ts',self.SamplingTime,'N_Agent',self.Number_Agents);
            self.Environment = Scenario_setup.(self.HSFMsetup.Scenario)(RoomPoly);%(self.HSFMsetup.Scenario);
            self.AgentsSetup = AgentMotion.AgentProperties(self,In);
            self.Agent = AgentInitialize(self);
            self.DestinationReach = zeros(1,self.Number_Agents);
        end
        
        %%
        function [Agent]  = AgentInitialize(self)
            %% preallocate
            Agent = struct();
            for cnt = 2:self.HSFMsetup.N_Agent
                Agent(cnt) = struct();
            end
            Na = self.Number_Agents;
            for cnt = 1:Na
                Agent(cnt).m = self.AgentsSetup(cnt).m;
                Agent(cnt).r = self.AgentsSetup(cnt).r;
                Agent(cnt).Inertia = 0.5*(Agent(cnt).r)^2;
                Agent(cnt).vd = self.AgentsSetup(cnt).vd;
                %                 Agent(cnt).InitialPos =
                %% Initialization
                Agent(cnt).X0 = self.AgentsSetup(cnt).X0;
                Agent(cnt).X_dest = self.AgentsSetup(cnt).Xf(:,end);
                Agent(cnt).X(:,1) = self.AgentsSetup(cnt).X0;
                Agent(cnt).q(:,1) = self.AgentsSetup(cnt).q0;
                % vel(:,1) = self.AgentsSetupProp.vel0;
                Agent(cnt).vB(:,1) = [self.AgentsSetup(cnt).vel0 0]';
                th = Agent(cnt).q(1,1);
                rot = [cos(th) -sin(th); sin(th) cos(th)];
                Agent(cnt).v(:,1) = rot*Agent(cnt).vB(:,1);
                Agent(cnt).Xf = self.AgentsSetup(cnt).Xf;
                % omega(cnt) = vel(2,cnt);
                Agent(cnt).DestPoint = 1;
                Agent(cnt).Xnext = self.AgentsSetup(cnt).Xf(:,1);
                Agent(cnt).Xpassed = Agent(cnt).X0;
            end
        end
        
        %%
        function x_true = do(self)
            walls = self.Environment.Walls;
            Ts = self.HSFMsetup.Ts;
            Aw = self.HSFMsetup.A_w;
            Bw = self.HSFMsetup.B_w;
            Ai = self.HSFMsetup.A_i;
            Bi = self.HSFMsetup.B_i;
            k1 = self.HSFMsetup.k1;
            k2 = self.HSFMsetup.k2;
            ko = self.HSFMsetup.ko;
            kd = self.HSFMsetup.kd;
            Na = self.HSFMsetup.N_Agent;
            klambda = self.HSFMsetup.klambda;
            alpha = self.HSFMsetup.alpha;
            Agents = self.Agent;
            %% preallocate
            x_true = nan(4,Na);
            for cnt= 1: Na
                if not(self.DestinationReach(cnt))
                    omega = Agents(cnt).q(2);
                    th = Agents(cnt).q(1);
                    rot = [cos(th) -sin(th); sin(th) cos(th)];
                    rotf = rot(:,1);
                    roto = rot(:,2);
                    %                     vf = Agents(cnt).vB(1);
                    vo = Agents(cnt).vB(2);
                    % esired direction of motion
                    DesDir = (Agents(cnt).Xnext-Agents(cnt).X)/sqrt(sum((Agents(cnt).Xnext-Agents(cnt).X).^2));
                    v0 = (Agents(cnt).vd+self.HSFMsetup.sigma_vd*randn)*DesDir;
                    f0 = Agents(cnt).m*(v0-Agents(cnt).v)/self.HSFMsetup.tau;
                    f_ij = zeros(2,1);
                    f_iw = zeros(2,1);
                    for j = 1:Na
                        if j~=cnt
                            r_ij = (Agents(cnt).r+Agents(j).r)*2;
                            d_ij = norm(Agents(cnt).X-Agents(j).X);
                            n_ij = (Agents(cnt).X-Agents(j).X)/d_ij;
                            n_ij = [n_ij(1),-n_ij(2)]';
                            t_ij = [n_ij(2),n_ij(1)]';    % maybe t_ij = [-n_ij(2),n_ij(1)]';
                            Dv_ji = (Agents(j).v-Agents(cnt).v)'*t_ij;
                            f_ij = f_ij+Ai*exp(-abs(r_ij-d_ij)/Bi)*(n_ij);
                            if r_ij>d_ij
                                 f_ij = f_ij+ k1*(r_ij-d_ij)*n_ij;
                                f_ij = f_ij+k2*(r_ij-d_ij)*Dv_ji*t_ij;
                            end
                        end
                    end
                    for iwall = 1:size(walls,1)
                        % Distance from the segment
                        [d_obst, closest_point] = AgentMotion.DistancePersonWall(Agents(cnt).X, walls(iwall,:));
                        
                        n_iW = (Agents(cnt).X-closest_point)/d_obst;
                        t_iW = [-n_iW(2),n_iW(1)]';
                        Dv_Wi = -Agents(cnt).v'*t_iW;
                        push_f = max(0, Agents(cnt).r-d_obst);
                        f_iw = f_iw+(Aw*exp((Agents(cnt).r-d_obst)/Bw)+k1*push_f)*n_iW-k2*push_f*Dv_Wi*t_iW;
                        %                         end
                    end
                    fe = f_ij+f_iw;
                    uf = ((f0+fe)'*rotf);
                    uo = (ko*fe'*roto-kd*vo);
                    uB = [uf, uo]';
                    % torque
                    f0_phase = atan2(DesDir(2),DesDir(1));%angle(complex(f0(1),f0(2)));
                    f0_mag = abs(complex(f0(1),f0(2)));
                    kth=Agents(cnt).Inertia*klambda*f0_mag;
                    kom=Agents(cnt).Inertia*(1+alpha)*sqrt(klambda*f0_mag/alpha);
                    % evaluate desired rotation
                    ang = (mod(th-f0_phase+pi,2*pi)-pi);
                    %                     max_ang_diff = self.HSFMsetup.MaxAbsOmega*Ts;
                    uth = (-kth*(ang)-kom*omega);
                    force(cnt).uB = uB;
                    force(cnt).uth = uth;
%                     [Agents(cnt).X, Agents(cnt).vB, Agents(cnt).q, Agents(cnt).v] = AgentMotion.dynamic_recursive_state(Agents(cnt).X, Agents(cnt).vB, Agents(cnt).q, uB, uth, Ts,Agents(cnt).m, Agents(cnt).Inertia);
%                     x_true(:,cnt) = [Agents(cnt).X; Agents(cnt).v];
%                     EucD = sqrt(sum((Agents(cnt).X-Agents(cnt).Xnext).^2));
%                     if EucD<=3
%                         Agents(cnt).vd = 1;
%                     end
%                     maxdis2dest = max(self.HSFMsetup.maxdis2dest,Agents(cnt).vd*Ts);
%                     if EucD<=maxdis2dest
%                         if Agents(cnt).DestPoint<size(self.AgentsSetup(cnt).Xf,2)
%                             Agents(cnt).Xpassed = self.AgentsSetup(cnt).Xf(:,Agents(cnt).DestPoint);
%                             Agents(cnt).DestPoint = Agents(cnt).DestPoint+1;
%                             Agents(cnt).Xnext = self.AgentsSetup(cnt).Xf(:,Agents(cnt).DestPoint);
%                         else
%                             self.DestinationReach(cnt) = 1;
%                         end
%                     end
                end
            end
            for cnt = 1:Na
                if not(self.DestinationReach(cnt))
                    
                    [Agents(cnt).X, Agents(cnt).vB, Agents(cnt).q, Agents(cnt).v] = AgentMotion.dynamic_recursive_state(Agents(cnt).X, Agents(cnt).vB, Agents(cnt).q, force(cnt).uB, force(cnt).uth, Ts,Agents(cnt).m, Agents(cnt).Inertia);
                    x_true(:,cnt) = [Agents(cnt).X; Agents(cnt).v];
                    EucD = sqrt(sum((Agents(cnt).X-Agents(cnt).Xnext).^2));
                    if EucD<=3
                        Agents(cnt).vd = 1;
                    end
                    maxdis2dest = max(self.HSFMsetup.maxdis2dest,Agents(cnt).vd*Ts);
                    if EucD<=maxdis2dest
                        if Agents(cnt).DestPoint<size(self.AgentsSetup(cnt).Xf,2)
                            Agents(cnt).Xpassed = self.AgentsSetup(cnt).Xf(:,Agents(cnt).DestPoint);
                            Agents(cnt).DestPoint = Agents(cnt).DestPoint+1;
                            Agents(cnt).Xnext = self.AgentsSetup(cnt).Xf(:,Agents(cnt).DestPoint);
                        else
                            self.DestinationReach(cnt) = 1;
                        end
                    end
                end
                
            end
            self.Agent = Agents;
        end
    end
    %%
    methods (Static)
        %%
        function [X_new, vB_new, q_new, v_new] = dynamic_recursive_state(X, vB, q, uB, uth, Ts, mAgent, AgentInertia)
            vB_new = vB+Ts*uB/mAgent;
            A = [0 1; 0 0];
            b = [0 1/AgentInertia]';
            q_new = q+Ts*(A*q+b*uth);
            q_new(1) = mod(q_new(1)+pi,2*pi)-pi;
            th = q_new(1);
            rot = [cos(th) -sin(th); sin(th) cos(th)];
            v_new = rot*vB_new;
            X_new = X+Ts*v_new;
        end
        %%
        function [dist_obst, closest_point] = DistancePersonWall(X,wall)
            x1 = wall(1);
            y1 = wall(2);
            x2 = wall(3);
            y2 = wall(4);
            x = X(1);
            y = X(2);
            % Distances
            A = x - x1;
            B = y - y1;
            C = x2 - x1;
            D = y2 - y1;
            
            % Dot product
            dot = A * C + B * D;
            
            % Length of the segment
            len_sq = C * C + D * D;
            
            % Projection of the point with respect to the segment (between 0 and 1 is inside)
            param = -1;
            if not(len_sq == 0)
                param = dot / len_sq;
            end
            
            if (param < 0)
                xx = x1;
                yy = y1;
            else
                if (param > 1)
                    xx = x2;
                    yy = y2;
                else
                    xx = x1 + param * C;
                    yy = y1 + param * D;
                end
            end
            
            dx = x - xx;
            dy = y - yy;
            
            dist_obst = sqrt(dx^2 + dy^2);
            closest_point = [xx; yy];
        end
        %%
        function AgentSetup = AgentProperties(self,In)
            HSFMsetup = self.HSFMsetup;
            %% preallocate
            AgentSetup = struct();
            for cnt = 2:HSFMsetup.N_Agent
                AgentSetup(cnt) = struct();
            end
            %% agents properties
            for cnt = 1:HSFMsetup.N_Agent
                % agent radii
                AgentSetup(cnt).r = HSFMsetup.rMin;%+(HSFMsetup.rMax-HSFMsetup.rMin)*rand;
                % agent mass
                AgentSetup(cnt).m = HSFMsetup.mMin;%+(HSFMsetup.mMax-HSFMsetup.mMin)*rand;
                % agent inertia
                AgentSetup(cnt).inertia = 0.5*AgentSetup(cnt).m*(AgentSetup(cnt).r)^2;
                % desired speed
                AgentSetup(cnt).vd = HSFMsetup.vd;
            end
            %% room properties
            min2wall = HSFMsetup.rMax+HSFMsetup.r_obst;
            walls = self.Environment.Walls;
            x_min = min(min(walls(:,[1,3])))+min2wall;
            x_max = max(max(walls(:,[1,3])))-min2wall;
            y_min = min(min(walls(:,[2,4])))+min2wall;
            y_max = max(max(walls(:,[2,4])))-min2wall;
            if isfield(In,'TrajectoryPoints')
                x2go = In.TrajectoryPoints;
            else
                x2go = [];
            end
            if isfield(In,'InitialHeading')
                Theta = In.InitialHeading;
            else
                Theta = [];
            end
            
            %% initial states
            for cnt = 1:HSFMsetup.N_Agent
                AgentSetup(cnt).vel0 = 0;%HSFMsetup.v_max*rand/3;
                if isempty(Theta)
                    AgentSetup(cnt).q0 = [-pi+2*pi*rand 0]';
                else
                    AgentSetup(cnt).q0 = [Theta(cnt) 0]';
                end
                if length(x2go)>=cnt && not(isempty(x2go{cnt}))
                    ff = x2go{cnt}';
                    %                     ff = reshape(ff,2,length(ff)/2);
                    inp = inpolygon(ff(1,:)',ff(2,:)',self.Environment.map_points(:,1),self.Environment.map_points(:,2));
                    c = find(inp==0);
                    if not(isempty(c))
                        error(['The position [',num2str(ff(:,c(1))'),'] is out of the defined environment']);
                    else
                        AgentSetup(cnt).X0 = ff(:,1);
                        AgentSetup(cnt).Xf = ff(:,2:end);
                    end
                else
                    eucdis = 0;
                    flagIsIn = 0;
                    while eucdis<10 || ~flagIsIn
                        n_points = randi(2);
                        AgentSetup(cnt).X0 = [x_min+(x_max-x_min)*rand, y_min+(y_max-y_min)*rand]';
                        for kk = 1:n_points
                            AgentSetup(cnt).Xf(:,kk) = [x_min+(x_max-x_min)*rand, y_min+(y_max-y_min)*rand]';
                        end
                        ff = [AgentSetup(cnt).X0, AgentSetup(cnt).Xf];
                        inp = inpolygon(ff(1,:)',ff(2,:)',self.Environment.map_points(:,1),self.Environment.map_points(:,2));
                        flagIsIn = sum(inp)==length(inp);
                        eucdis = 0;
                        for kk = 1:n_points
                        eucdis = eucdis+norm(ff(:,kk+1)-ff(:,kk));
                        end
                    end
                end
            end
        end
        
    end
    
end
