%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This implements a set of functions which is common for all evd-classes.
% Description end

classdef evd_superclass < handle & matlab.mixin.Copyable
    properties
        REPORT=[];
    end
    methods (Static)
        function err = performance(V,d,A_in)
            %%%CHECK
            %             A_comp=zeros(size(A));
            %             for cnt=1:length(d);
            %                 A_comp=A_comp+V(:,cnt)*d(cnt)*V(:,cnt)';
            %             end
            A_comp=V*diag(d)*V';
            %compute the reference
            [V_ref,E_ref]=eig(A_in);
            [E_ref,I]=sort(diag(abs(E_ref)),'descend');
            V_ref=V_ref(:,I);
            I=1:length(d);
            A_ref=V_ref(:,I)*diag(E_ref(I))*V_ref(:,I)';
            err=norm(A_ref-A_comp);
        end
    end
end