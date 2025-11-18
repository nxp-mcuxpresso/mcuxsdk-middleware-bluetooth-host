%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Calculated the distance between the initiator and reflector
% Description end

function [r]=PosInit(SegmentID,PosID)
if nargin==0
    SegmentID='AB'
    PosID=0;
    [r]=PosInit(SegmentID,PosID)
else
    r0=PicketPos(SegmentID(1));
    r1=PicketPos(SegmentID(2));
    Dir=r1-r0;Dir=Dir/norm(Dir);
    if PosID<norm(r1-r0)*100
        r=r0+Dir*PosID/100;
    else
        r=NaN;
    end
end