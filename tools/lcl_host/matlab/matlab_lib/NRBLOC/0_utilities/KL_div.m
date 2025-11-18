%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This function does the KL-divergence estimation.
% Description end

function [Q]=KL_div(Distr1,Distr2)
f =  @(x,A,B) pdf(A,x).*(log(pdf(A,x)+eps)-log(pdf(B,x)+eps));
Q = integral(@(x)f(x,Distr1,Distr2),-inf,inf);

