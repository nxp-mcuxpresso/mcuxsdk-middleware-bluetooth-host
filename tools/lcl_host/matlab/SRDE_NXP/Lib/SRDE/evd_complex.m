function [V,d]= evd_complex(A,TOL1,TOL2,K,precision)
if nargin < 5
    precision = 1;
end
MaxIt=100;
if precision == 0
    A_in=single(A);
else
    A_in=A;
end
% A is hermitian square matrix. TOL is a vector with two entries using the
sumeig=trace(A_in);%Sum on main-diagonal of A equal the sum of its eigenvalue.
%Note for implementation: As A is hermitian, all elements on the main-diagonal MUST BE REAL!
cnt=1;
Cap=0;
%Note for implementation, the size of V and d are not known at forehand
if precision == 0
    Tol2_tmp=single(TOL2*sumeig);
else
    Tol2_tmp=TOL2*sumeig;
end
while (cnt<=K) && (Cap<Tol2_tmp)% If CAP is smaller than Tol2_tmp, the next largest eigenvalue is searched.
    [d(cnt),V(:,cnt),~]=maxeig(A_in,TOL1,MaxIt);
    A_in=A_in - d(cnt)*V(:,cnt)*V(:,cnt)';%Both A  and V(:,cnt)*V(:,cnt)' are hermitians, V(:,cnt) is a vector!
    Cap=Cap+d(cnt);
    cnt=cnt+1;
end
end

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
    if MaxIt<NofIt
        break
    end
end
end


