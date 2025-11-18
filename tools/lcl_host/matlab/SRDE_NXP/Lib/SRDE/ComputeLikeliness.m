function Likeliness=ComputeLikeliness(omega,eigenvects,EVs,TRACE,Ns)
    if iscolumn(EVs)
        EVs=EVs.';
    end
    L = size(eigenvects,1);     % Size of the covariance matrix (LxL)
    FFTMATRIX = exp(-1i*((0:size(eigenvects,1)-1)'*omega));
    SigVects  = eigenvects(:,1:Ns-1).';
    P         = zeros(1,Ns-1);
    for cnt=1:Ns-1
        h = SigVects(cnt,:)*FFTMATRIX;
        P(cnt) = (abs(h).^2)/L;
    end
    t = P*EVs(1:Ns-1).';            
    Likeliness = t/TRACE;
end