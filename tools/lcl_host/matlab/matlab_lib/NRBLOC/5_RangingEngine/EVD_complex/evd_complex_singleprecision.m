%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef evd_complex_singleprecision < evd_superclass
    % Description
    % Implements counter-part of matlab's eig-function, using the
    % power-method. The main method is DoRun, which aside the constructor
    % and square-hermitian Covariance Matrix takes to a vector of length
    % two as input argument. The first element of this vector defines the 
    % stopping criterium for the Power Method, the second one defines what 
    % portion of the eigenvalues should be computed(0.99 denotes that at least 
    % 99 of the sum of eigenvalue is captured.) 
    % This function is using single-precision floating point, whereas the
    % class evd_complex uses double-precision floating point. The
    % implementation in c++ uses single precision as well.
    % Description end
    properties
        NofIt=0;    %Used for evaluation of complexity, i.e. total number of loop in maxeig()
        MaxIt=100;
    end
    methods
        function self = evd_complex_singleprecision(selftest)
            if nargin == 0
                selftest = 0;                                               % Allows for a self-testing of evd_complex_singleprecision, default (0) means no testing
            end
            if selftest ~= 0                                                % Only FOR EVALUATION PURPOSES
                rng(1)
                for cnt=1:1
                    % Generate stimuli
                    L=26;
                    H_est = randn(1,80)+1i*randn(1,80);
                    [Cov] = H_2_Cov(H_est,L);
                    
                    % Compute the reference
                    [V_ref,E_ref] = eig(Cov);
                    E_ref         = diag(E_ref);
                    % Compute answer by evd_complex
                    [V,E]=doRun(self,Cov,1e-10,0.99, inf);
                    % evd_complex will compute less eigenvalues, compare
                    % only the length(E) largest eigenvalues.
                    N     = length(E_ref);
                    I     = N-length(E)+1:N;
                    A_ref = V_ref(:,I)*diag(E_ref(I))*V_ref(:,I)';
                    
                    Difference = norm(A_ref-V*diag(E)*V')/norm(A_ref);
                    disp(['evd_complex Error is :' num2str(Difference) ]);
                    if Difference > 1
                        error('self evaluation of the EVD-failed')
                    end
                end
            end
        end
        
        function [V,d] = doRun(self, A, TOL1, TOL2, K)                      
            A = single(A);
            A_in = A;
            self.NofIt = 0;
            % A is hermitian square matrix. TOL is a vector with two entries using the  
            sumeig = trace(A);%Sum on main-diagonal of A equal the sum of its eigenvalue. 
            %Note for implementation: As A is hermitian, all elements on the main-diagonal MUST BE REAL!
            cnt = 1;
            Cap = 0;
            
            Tol2_tmp = single(TOL2 * sumeig);
            while (cnt <= K) && (Cap < Tol2_tmp)                            % If CAP is smaller than Tol2_tmp, the next largest eigenvalue is searched. 
                [d(cnt), V(:,cnt), self.NofIt(cnt)] = self.maxeig(A, TOL1, self.MaxIt);
                A   = A - d(cnt)*V(:,cnt)*V(:,cnt)';                        % Both A  and V(:,cnt)*V(:,cnt)' are hermitians, V(:,cnt) is a vector!
                Cap = Cap + d(cnt);
                cnt = cnt + 1;
            end
            self.REPORT.err = self.performance(V,d,A_in);
        end
    end
    methods(Static)
        function  [lambda,x,NofIt] = maxeig(Cov,TOL1,MaxIt)
            % Compute the largest eigenvalue and associated eigenvector of
            % a matrix A using the power method
            % syntax: [lambda,x] = maxeig(A,TOL)
            %   A       = square-hermitian matrix whose max-eigenvalue is sought
            %   lambda  = largest eigenvalue
            %   x       = corresponding eigenvector
            NofIt=0;
            [n,~] = size(Cov);
            x = zeros(n,1);
            x(1) = 1;  % assumed to be not orthogonal to the first eigenvector
            lambda = 1;
            lambdaold = 0;
            while(abs(lambda -lambdaold) > TOL1)
                lambdaold = lambda;
                z = Cov*x;
                x = z/norm(z);
                lambda = real(x'*Cov*x);%Can be implemented more efficiently! A is hermitian! Identical to sum(sum(conj(x*x').*Cov)) %
                %I.e. do First an element-wise multiplication of the two matrices(A and conj(x*x') (Both Hermitian, #Mult can be almost halved)) and then sum all elements!
                NofIt=NofIt+1; %DO NOT IMPLEMENT:This is for my own book-keeping.
                lambdas(NofIt)=lambda;
                if MaxIt<NofIt
                    break
                end
            end
        end
    end
    
end