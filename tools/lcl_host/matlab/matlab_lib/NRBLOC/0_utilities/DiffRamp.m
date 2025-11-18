%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function z=DiffRamp(x,B,b,A,a)
% Description
%   This function implements the T-function as defined in TN196, Appendix B
%   This function is used by the function semianalytic_MCPD.m
%   see also semianalytic_MCPD.m
% Description end

if b<=a
    z=zeros(size(x));
    for cnt=1:length(x)
        if x(cnt)<b
            z(cnt)=0;
        elseif x(cnt)<a
            z(cnt)=B*(x((cnt))-b);
        else
            %z(cnt)=(A-B)*(x(cnt)-b)+A*(b-a); 
            z(cnt)=(B-A)*x(cnt)+A*a-B*b; 
        end
    end
else
    z=-DiffRamp(x,A,a,B,b);
end
