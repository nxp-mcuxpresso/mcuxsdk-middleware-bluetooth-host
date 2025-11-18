%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Converts the ID to a position
% Description end

function [r]=PicketPos(PicketID)
if nargin==0
    close all
    
    LETTERS='ABCDEF';
    for cnt=1:length(LETTERS)
        [r]=PicketPos(LETTERS(cnt));
        plot3(r(1),r(2),r(3),'go')
        text(r(1),r(2),r(3),LETTERS(cnt))
        hold on
    end
    
    axis equal
    
else
    
    h=1.5;
    switch PicketID
        case 'A'
            r=[1.5 2+3 h];
        case 'B'
            r=[1.5+4 2+3 h];
        case 'C'
            r=[1.5+4 2+7.76 h];
        case 'D'
            r=[1.5 2+7.76 h];
        case 'E'
            r=[1.5 2 h];
        case 'F'
            r=[1.5+4 2 h];
        case 'X'
            r=[1.5+4 2 h];
        case 'Y'
            r=[1.5+4 2 h]; 
        case 'Z'
            r=[1.5+4 2 h]; 
        otherwise
            error('unknown Picket ID')
    end
end