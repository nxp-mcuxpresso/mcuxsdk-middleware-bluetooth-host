%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This function plots the position of the reflectors
% Description end

function varargout = plot_pos_refl(ReflID, ReflPos, MarkerColor)
    assert(length(ReflID)==size(ReflPos,1));
    assert(size(ReflPos,2)==3);
    narginchk(2,3);
    if nargin==2
        MarkerColor = 'b';
    end
    
    Hline = zeros(length(ReflID),1);
    Htext = zeros(length(ReflID),1);

    for k = 1:length(ReflID)
        Hline(k) = plot(ReflPos(k,1),ReflPos(k,2),'s', 'MarkerEdgeColor', MarkerColor, 'MarkerFaceColor', MarkerColor, 'MarkerSize', 10);
        Htext(k) = text(ReflPos(k,1)+0.1,ReflPos(k,2)+0.1,num2str(ReflID(k)));
        hold on
    end
    varargout{1} = Hline;
    varargout{2} = Htext;
end