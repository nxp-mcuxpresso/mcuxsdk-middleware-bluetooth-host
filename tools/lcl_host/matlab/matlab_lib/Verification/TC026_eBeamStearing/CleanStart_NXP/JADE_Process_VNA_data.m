%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is the main file to see the performance of the
%   joint-angle-and-distance algorthm.
%   In order to run this script, please unzip the file 
%       'P054_ULA_channel_measurements.zip' 
%   in this folder. 
% Description end
function []=JADE_Process_VNA_data() 
close all;
p = fileparts(mfilename('fullpath'));
load(fullfile(p, 'P054_ULA_channel_measurements', 'Campaign_P054_ULA_1x1x4.mat'), 'DATA', 'True_angle', 'freq_meas', 'true_distance');
% load(fullfile(p, 'Campaign_P054_ULA_2x1x4.mat'));

DATA = DATA(1:10,:);%REMOVE THIS LINE TO RUN THE FULL TEST!

RE=RangingEngine_Music_emb();
RE.PLOT=false;
RE.L=40;
RE.TOL(3)=-inf;
RE.TOL(2)=[0.999];
RE.NFFT=2*1024;
RE.FP=evd_complex_perfect(); %for a fair comparison with RE_AOA
RE.MethodSubspaceSep=8;
RE.MinLevel=0.5;

RE_AOA=RangingEngine_Music_AoA();
RE_AOA.L=40;
RE_AOA.La=1;
RE_AOA.delta_F=RE.delta_F;
RE_AOA.MethodSubspaceSep=8;
RE_AOA.MinLevel=RE.MinLevel;
RE_AOA.type='left&right';

UsedTones=2.4e9+(0:79)*RE.delta_F;


CNT=0;
CNT=CNT+1;ENGINE{CNT}={RE,UsedTones,1:4};
CNT=CNT+1;ENGINE{CNT}={RE_AOA,UsedTones,1:4};

PLOT=false;
I_AoA=[];
if ~isfield(DATA(1),'S_array2')
    NofArrays=1;
else
    NofArrays=2;
end
    

for cnt_engine=1:length(ENGINE)
    RE=ENGINE{cnt_engine}{1};
    UsedTones=ENGINE{cnt_engine}{2};
    UsedAntenna=ENGINE{cnt_engine}{3};

    I_tones=find(ismember(freq_meas,UsedTones));
    for cnt_POS=1:size(DATA,1)
        for cnt_Pol=1:size(DATA,2)
            for cnt_array=1:NofArrays
                index = cnt_Pol + (cnt_array-1)*size(DATA,2);
                
                if cnt_array==1
                    CR.Hest=DATA(cnt_POS,cnt_Pol).S_array1(UsedAntenna,I_tones);
                else
                    CR.Hest=DATA(cnt_POS,cnt_Pol).S_array2(UsedAntenna,I_tones);
                end
                [d_est(cnt_POS,index),AOA_est_tmp]=do(RE,CR);
                if ~isempty(AOA_est_tmp)
                    AOA_est(cnt_POS,index)=AOA_est_tmp;
                end
            end
        end
        disp(['progress is:' num2str(100*cnt_POS/size(DATA,1)) '%'])
    end
    if ~isempty(AOA_est_tmp)
        figure;plot(True_angle(1:size(d_est,1)),AOA_est,'.')
        err=AOA_est-True_angle(1:size(AOA_est,1))'*ones(1,size(AOA_est,2));
        err=sort(err(:));
        figure;plot(err,(1:numel(err))/numel(err))
        xlabel('angle[rad]');ylabel('CDF');grid on
    end
    figure;plot(true_distance(1:size(d_est,1)),d_est.','.');grid on
    xlabel('distance[m]');ylabel('distance estimate[m],no bias compensation');grid on
    
    err=d_est-true_distance(1:size(d_est,1))'*ones(1,size(d_est,2));
    err=sort(err(:));
    figure;plot(err,(1:numel(err))/numel(err))
    xlabel('distance[m]');ylabel('CDF');grid on
end
