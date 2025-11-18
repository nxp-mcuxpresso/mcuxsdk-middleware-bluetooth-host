%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [Pos]=GetBSPos(BSID)
% Description
%   Outputs the position in xyz-coordinates of the anchor/BS identied by BSID. 
%   The BSID is an integer equal to 1,2,3,4.   
%   This relates to the radio channel measurements conducted in P054 at
%   Imec-nl/Holst-center in 2016. A description of the measurements can be
%   found in Technical Note TN-16-WATS-TP2-196, "“Accurate localization & 
%               distance measurement capabilities of NB ISM band radios”
% Description end

if nargin==0
    for cnt=1:4
        [Pos]=GetBSPos(cnt);
        hdls(cnt,1)=plot3(Pos(1),Pos(2),Pos(3),'x');
        hold on
        hdls(cnt,2)=text(Pos(1),Pos(2),Pos(3),num2str(cnt));
    end
    axis equal
    xlim([0 8])
    ylim([0 12])
    view(0,90)
    Pos=hdls;
else
    switch BSID
        case 1
            Pos=[0.5 11.76-0.5];
        case 2
            Pos=[7-0.5 11.76-0.5];
        case 3
            Pos=[7-0.5 0.5];
        case 4
            Pos=[0.5 0.5];
        otherwise
    end
    h=1.5;
    Pos=[Pos h];
end