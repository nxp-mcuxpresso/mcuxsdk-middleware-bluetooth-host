%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [Pos]=GetPosition(SegmentID,PosID)
% Description
%   Outputs the position in xyz-coordinates of 'mobile'-antenna given the  
%   segment-identifier and position identifier. 
%   The segment-identifier is a combination of two letters both in the range 
%    between A,B,C,D,E,F. The PosID is an integer between 1 and x, where the
%   max value of x depends on the segment
%   This relates to the radio channel measurements conducted in P054 at
%   Imec-nl/Holst-center in 2016. A description of the measurements can be
%   found in Technical Note TN-16-WATS-TP2-196, "“Accurate localization & 
%               distance measurement capabilities of NB ISM band radios”
% Description end

if nargin==0
    LETTERS='ABCDEF';
    hdls=[];
    for cnt=1:length(LETTERS)
        for cnt2=1:length(LETTERS)
            SegmentID=[LETTERS(cnt) LETTERS(cnt2)];
            [NofPoints]=GetNofPosSegment(SegmentID);
            for PosID=1:NofPoints
                [Pos]=GetPosition(SegmentID,PosID);
                hdls=[hdls plot3(Pos(1),Pos(2),Pos(3),'bo','MarkerSize',3)];
                hold on
            end
        end
    end
    axis equal
    xlim([0 8])
    ylim([0 12])
    view(0,90)
    Pos=hdls;
else
    if ~isnatural(PosID)
        error('PosID should be a natural number larger than 0')
    end
    switch SegmentID
        case {'AB','BA'}
            [base]=GetPosPicket('A');
            [dest]=GetPosPicket('B');
        case {'AE','EA'}
            [base]=GetPosPicket('A');
            [dest]=GetPosPicket('E');
        case {'BC','CB'}
            [base]=GetPosPicket('B');
            [dest]=GetPosPicket('C');
        case {'CD','DC'}
            [base]=GetPosPicket('C');
            [dest]=GetPosPicket('D');
        case {'DA','AD'}
            [base]=GetPosPicket('D');
            [dest]=GetPosPicket('A');
        case {'EF','FE'}
            [base]=GetPosPicket('E');
            [dest]=GetPosPicket('F');
        case {'FB','BF'}
            [base]=GetPosPicket('F');
            [dest]=GetPosPicket('B');
        otherwise
            error('?')
    end
    dir=(dest-base);
    dir=dir/norm(dir);
    Pos=base+0.25*(PosID-1)*dir;
end