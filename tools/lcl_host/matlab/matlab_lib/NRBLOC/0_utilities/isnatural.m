%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [OK]=isnatural(number)
    % Description
    %   check whether number if a natural number, i.e. 
    %   OK=true         if number is part of the set {1,2,3,4,5,...} 
    %   OK=false        otherwise
    % Description end

if number>0.5
    OK=abs(round(number)-number)<1e-16;
else
    OK=false;
end

end