%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [Pos]=GetPosPicket(PicketID)
% Description
%   Outputs the position in xyz-coordinates of the START of the segment 
%   which we refer to as a PICKET. The PICKET. The PicketID is a letter equal to 
%   A,B,C,D,E,F.  Note that the END of a given segment always coincided with
%   the start of another one. In other words, 2 PicketsID form a segment. 
%   This relates to the radio channel measurements conducted in P054 at
%   Imec-nl/Holst-center in 2016. A description of the measurements can be
%   found in Technical Note TN-16-WATS-TP2-196, "“Accurate localization & 
%               distance measurement capabilities of NB ISM band radios”
% Description end

if nargin==0
    LETTERS='ABCDEF';
    for cnt=1:length(LETTERS)
        [Pos]=GetPosPicket(LETTERS(cnt));
        hdls(cnt,1)=plot3(Pos(1),Pos(2),Pos(3),'x');
        hold on
        hdls(cnt,2)=text(Pos(1),Pos(2),Pos(3),LETTERS(cnt));
    end
    axis equal
    xlim([0 8])
    ylim([0 12])
    view(0,90)
    Pos=hdls;
else
    switch PicketID
        case 'A'
            Pos=[1.5 2.005+3];
        case 'B'
            Pos=[1.5+4 2.005+3];
        case 'C'
            Pos=[1.5+4 2.005+7.75];
        case 'D'
            Pos=[1.5 2.005+7.75];
        case 'E'
            Pos=[1.5 2.005];
        case 'F'
            Pos=[1.5+4 2.005];
        otherwise
    end
    h=1.5;
    Pos=[Pos h];
end