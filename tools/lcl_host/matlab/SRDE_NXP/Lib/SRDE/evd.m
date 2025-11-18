function [V,d]=evd(A,TOL1,TOL2,K)%TOL is a vector
sumeig=trace(A);%Sum on main-diagonal of A equal the sum of its eigenvalue. 
[V,d]=eig(A);
d=diag(d);
[d,I]=sort(d,'descend');
V=V(:,I);
cnt=1;
Cap=0;
%Note for implementation, the size of V and d are not known at forehand
Tol2_tmp=TOL2*sumeig;
while (cnt<=K) && (Cap<Tol2_tmp) % If CAP is smaller than Tol2_tmp, the next largest eigenvalue is searched. 
    Cap=Cap+d(cnt);
    cnt=cnt+1;
end
d=d(1:cnt-1);
V=V(:,1:cnt-1);