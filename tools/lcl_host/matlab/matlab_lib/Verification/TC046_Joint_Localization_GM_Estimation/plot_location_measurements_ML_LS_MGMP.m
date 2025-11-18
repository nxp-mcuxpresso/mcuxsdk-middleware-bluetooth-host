%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Plots the locations for each type.
% Description end


function [RMSE, Hfig] = plot_location_measurements_ML_LS_MGMP(T, MeasCfg, Initial_S, Title, Hax) %#ok<STOUT>

ColorGreen = [0, 234, 117]/255;
if nargin==4
    Hfig = figure('Name', sprintf('Location - %s', Title)); clf;
else
    axes(Hax); cla;
    Hfig = gcf;
end
hold on; grid on;

% Plot anchor positions
ReflIDList = MeasCfg.ID(~MeasCfg.IsInit);
ReflList   = MeasCfg.Position(~MeasCfg.IsInit, :);
plot_pos_refl(ReflIDList, ReflList, 'b');

% Plot actual positions
UniquePos = unique(T(:, {'segmentID', 'PosID'}), 'rows', 'stable');
PosTag = zeros(height(UniquePos), 3);
for k=1:height(UniquePos)
    PosTag(k,:) = PosInit(UniquePos.segmentID{k},UniquePos.PosID(k));
end
plot(PosTag(:,1), PosTag(:,2), 'o', 'Color', ColorGreen);

% Plot corners
Corners = {'AB', 'BC', 'CD', 'DA', 'EF', 'FB'};
PosCorners = zeros(length(Corners), 3);
for k=1:length(Corners)
    PosCorners(k,:) = PosInit(Corners{k}, 0);
end
plot(PosCorners(:,1), PosCorners(:,2), 'o', 'Color', ColorGreen, 'MarkerFaceColor', ColorGreen);
text(PosCorners(:,1)+[-0.25, 0.1, 0.1, -0.25, -0.25, 0.1].', PosCorners(:,2)+[0, 0, 0.1, 0.1, -0.1, -0.1].', {'A', 'B', 'C', 'D', 'E', 'F'});
axis equal;
xlim([-1, 8]);
ylim([0, 12]);

% Plot estimated distances
nAnchors = length(unique(T.ReflID));
nMeasurements = length(unique(T.MeasurementNumber));
if ismember('AntennaPair', T.Properties.VariableNames)
    nAntennaPairs = length(unique(T.AntennaPair));
else
    nAntennaPairs = 1;      % This happens at the moment, when the data from all antennae is processed
end


EstPos_ML = NaN(height(UniquePos) * nMeasurements, 2);
EstPos_LStype = NaN(height(UniquePos) * nMeasurements, 2);
EstPos_onethefly = NaN(height(UniquePos) * nMeasurements, 2);

ActPos = NaN(height(UniquePos) * nMeasurements, 3);

cnt = 0;
multiWaitbar(Title, 'Value', 0);

for k=1:height(UniquePos)
    multiWaitbar(Title, 'Value', (k-1)/height(UniquePos));
    segment = UniquePos.segmentID{k};
    pos     = UniquePos.PosID(k);
    indx_pos = strcmp(T.segmentID, segment) & T.PosID == pos;
    
    % X-axis grid for init of opt.
    X_grid = min(ReflList(:,1)):0.1:max(ReflList(:,1));
    
    % Y-axis grid for init of opt.
    Y_grid = min(ReflList(:,2)):0.1:max(ReflList(:,2));
    
    % lower_bound for init of opt
    lb = [min(ReflList(:,1)) ;min(ReflList(:,2))];
    % upper_bound for init of opt
    ub =  [max(ReflList(:,1)) ;max(ReflList(:,2))];
    
    % On-the-fly class gen/Anch measurment update
    Anchors = ReflList(:,1:2);
    if Initial_S.opti_flag
        MGMP = on_the_fly_MGMP(Initial_S.gm, Anchors, Initial_S.optcomp.type, Initial_S.optcomp.bias, X_grid, Y_grid, lb, ub);
    else
        MGMP = on_the_fly_MGMP(Initial_S.gm, Anchors, Initial_S.optcomp.type, Initial_S.optcomp.bias, X_grid, Y_grid);
    end
    range_err_Anch = rand(Initial_S.Err_model_is,[1 ((size(Anchors,1))*(size(Anchors,1)-1))/2]);
    MGMP = MGMP.GM_up_anch(range_err_Anch);
    
    % For each measurement at this position, calculate location
    for n = unique(T.MeasurementNumber).'
        cnt = cnt + 1;
        indx = indx_pos & T.MeasurementNumber==n;
        Distances = NaN(nAnchors,nAntennaPairs);
        for m = 1:nAnchors
            temp = T(indx & T.ReflID==ReflIDList(m),:).Distance.';
            if length(temp)==nAntennaPairs
                Distances(m,:) = temp;
            end
        end
        if ~any(isnan(Distances))
            ActPos(cnt,:) = PosInit(segment, pos);
            
            % True position (only used in ML)
            x_is = ActPos(cnt,:);
            
            % Momentum parameter
            beta_com = 0.5*(nAnchors)./[((nAnchors*(nAnchors-1))/2)+(cnt)*nAnchors]; %#ok<NBRAK>
            
            % MGMP routine
            [MGMP,EstPos_onethefly(cnt,:)] = MGMP.do(Distances,[],beta_com);
            
            % Computing the best rms of errors with perfect knowledge of channel/ML approach
            [EstPos_ML(cnt,:)] = MGMP.ML_pos(Distances, x_is, Initial_S.Err_model_is);
            
            % Computing the rms of errors with no knowledge of channel/LS-type approach
            [EstPos_LStype(cnt,:)] = MGMP.LS_pos(Distances);
            
        end
    end
end
multiWaitbar('CloseAll');

EstPos_ML(isnan(EstPos_ML(:,1)),:) = [];
EstPos_LStype(isnan(EstPos_LStype(:,1)),:) = [];
EstPos_onethefly(isnan(EstPos_onethefly(:,1)),:) = [];
ActPos(isnan(ActPos(:,1)),:) = [];


plot([ActPos(:,1), EstPos_ML(:,1)].',[ActPos(:,2), EstPos_ML(:,2)].','g:');
plot(EstPos_ML(:,1).', EstPos_ML(:,2).', 'g+');

plot([ActPos(:,1), EstPos_LStype(:,1)].',[ActPos(:,2), EstPos_LStype(:,2)].','b:');
plot(EstPos_LStype(:,1).', EstPos_LStype(:,2).', 'bo');

plot([ActPos(:,1), EstPos_onethefly(:,1)].',[ActPos(:,2), EstPos_onethefly(:,2)].','r:');
plot(EstPos_onethefly(:,1).', EstPos_onethefly(:,2).', 'rs');

xlabel('X [m]');
ylabel('Y [m]');

RMSEML = sqrt(mean(sum((EstPos_ML - ActPos(:,1:2)).^2,2)));
RMSELS = sqrt(mean(sum((EstPos_LStype - ActPos(:,1:2)).^2,2)));
RMSEonthefly = sqrt(mean(sum((EstPos_onethefly - ActPos(:,1:2)).^2,2)));
title(sprintf('RMSE ML = %.2fm, RMSE LS = %.2fm, RMSE On-the-fly = %.2fm', RMSEML,RMSELS,RMSEonthefly));
end