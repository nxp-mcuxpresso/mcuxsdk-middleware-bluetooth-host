function  den = InvPS(Param,SigVects,omega)
%output is always a value in the range [0 Param.L]
FFTMATRIX = fftvec(Param.L,omega);
h=SigVects*FFTMATRIX;
den=Param.L-sum(abs(h).^2,1);