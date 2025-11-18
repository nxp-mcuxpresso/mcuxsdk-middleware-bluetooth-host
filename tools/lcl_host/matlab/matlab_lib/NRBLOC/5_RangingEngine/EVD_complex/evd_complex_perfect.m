%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef evd_complex_perfect < evd_superclass
    % Description
    % Implements counter-part of matlab's eig-function, using the
    % power-method. The main method is DoRun, which aside the constructor
    % and square-hermitian Covariance Matrix takes to a vector of length
    % two as input argument. The first element of this vector defines the 
    % stopping criterium for the Power Method, the second one defines what 
    % portion of the eigenvalues should be computed(0.99 denotes that at least 
    % 99 of the sum of eigenvalue is captured.) 
    % Description end
    properties
        NofIt=0;    %Used for evaluation of complexity, i.e. total number of loop in maxeig()
        MaxIt=100;
    end
    methods
        function self=evd_complex_perfect(selftest)
             if nargin==0
                selftest=0; %Allows for a self-testing of evd_complex, default(0) means no testing
            end
            if selftest==1 %Only FOR EVALUATION PURPOSES
                rng(1)
                for cnt=1:1000
                    %generate stimuli
                    L=26;
                    H_est= randn(1,80)+1i*randn(1,80);
                    [Cov]=H_2_Cov(H_est,L);
                    
                    %compute the reference
                    [V_ref,E_ref]=eig(Cov);
                    E_ref=diag(E_ref);
                    %Compute answer by evd_complex
                    [V,E]=doRun(self,Cov,1e-10,0.99, inf);
                    % evd_complex will compute less eigenvalues, compare
                    % only the length(E) largest eigenvalues.
                    N=length(E_ref);
                    I=N-length(E)+1:N;
                    A_ref=V_ref(:,I)*diag(E_ref(I))*V_ref(:,I)';
                    
                    Difference=norm(A_ref-V*diag(E)*V')/norm(A_ref);
                    disp(['evd_complex Error is :' num2str(Difference) ]);
                    if Difference>1
                        error('self evaluation of the EVD-failed')
                    end
                end
            elseif selftest==0
            else
                error('undefined self-test')
            end
            
        end
        
        function [V,d]=doRun(self,A,TOL1,TOL2,K)%TOL is a vector
            A_in=A;
            sumeig=trace(A);%Sum on main-diagonal of A equal the sum of its eigenvalue. 
            [V,d]=eig(A);
            d=diag(d);
            [d,I]=sort(d,'descend');
            V=V(:,I);
            %Note for implementation: As A is hermitian, all elements on the main-diagonal MUST BE REAL!
            self.REPORT.d_all=d;
            self.REPORT.V_all=d;
            cnt=1;
            Cap=0;
            %Note for implementation, the size of V and d are not known at forehand
            Tol2_tmp=TOL2*sumeig;
            while (cnt<=K)&(Cap<Tol2_tmp);% If CAP is smaller than Tol2_tmp, the next largest eigenvalue is searched. 
                Cap=Cap+d(cnt);
                cnt=cnt+1;
            end
            d=d(1:cnt-1);
            V=V(:,1:cnt-1);
            self.REPORT.err = self.performance(V,d,A_in);
         end
    end
end