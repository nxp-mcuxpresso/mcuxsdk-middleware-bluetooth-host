function Ns = SubSpaceSep(Param,EVs)
Ns=min(length(EVs)+1,Param.MethodSubspaceSep);
if numel(Param.TOL)>2
    I=find(diff(10*log10(EVs))< Param.TOL(3),1,'first');
    if ~isempty(I)
        Ns=min(Ns,I);
    end
end