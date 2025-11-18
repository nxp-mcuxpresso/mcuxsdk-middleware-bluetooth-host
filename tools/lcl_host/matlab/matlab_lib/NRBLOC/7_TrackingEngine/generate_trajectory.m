%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function out = generate_trajectory(TrajectoryPoints,SamplingTime,alpha,RoomPoly)
% Description
% A short code to generate the reference points to test the Tracking
% algorithms
In.SamplngTime = SamplingTime;
In.TrajectoryPoints = TrajectoryPoints;
if nargin == 4
    In.RoomPoly = RoomPoly;
end
GNDtruth=AgentMotion(In);

%% initialize the tracking engine
x_true = [GNDtruth.Agent.X0;GNDtruth.Agent.v];
x_rel = [sqrt(sum((x_true(1:2)-GNDtruth.fix_anchor_pos).^2)); 0];
d_MCPD = x_rel(1)+alpha*x_rel(2)+GNDtruth.dss_std*randn;
cnt=1;
%%
DestinationReach = 0;
while ~all(DestinationReach)
    cnt=cnt+1;
    % Update actual Position of Agent in 2D
    x_true(:,cnt) = GNDtruth.do();
    % compute the actual position in 1D
    d_rel_old = x_rel(1,cnt-1);
    x_rel(:,cnt) = translate_abs_to_rel_of_anchor(x_true(:,cnt), GNDtruth.fix_anchor_pos,d_rel_old,SamplingTime);
    % Phase based range measruement
    d_MCPD(:,cnt) = x_rel(1,cnt)+alpha*x_rel(2,cnt)+GNDtruth.dss_std*randn;
    DestinationReach = GNDtruth.DestinationReach;
end

out.x_true = x_true;
out.x_rel = x_rel;
out.d_MCPD = d_MCPD;
end


function x_rel = translate_abs_to_rel_of_anchor(x, anchor_pos,d_old, Ts)
d_rel = sqrt(sum((x(1:2)-anchor_pos).^2));
v_rel = (d_rel-d_old)/Ts;
x_rel = [d_rel, v_rel]';
end
