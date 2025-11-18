function [x, fval,x_all,fval_all] = FirstPeak_PS(Param,SigVects)
CONSTANT=(2*pi*Param.delta_F)/3e8;
tolx=Param.PS_approx*CONSTANT;
v1 =  Param.InitPos*(2*pi*Param.delta_F)/3e8 ; % initial guess, could be zero..
fv1 =  InvPS(Param,SigVects,v1);
step = 2*pi/Param.NFFT;      %Initial step-size in forward (increasing omega direction).
itercount = 1;
IsUnderThresshold=false;
x_all = [];
fval_all = [];
while itercount < Param.maxiter
    x_all = [x_all v1];
    fval_all = [fval_all fv1];
    v2 = v1+step;
    fv2 = InvPS(Param,SigVects,v2);
    if IsUnderThresshold==false
        if fv2<Param.MinLevel*Param.L
            %We have reached it
            IsUnderThresshold=true;
        end
    else
        if fv2>=fv1
            step=-step/2;
        end
    end
    if abs(step) <= tolx
        %If the step-size(abs(step)) is below tolx, we stop.
        break
    else
        fv1=fv2;
        v1=v2;
    end
    itercount=itercount+1;
end
[x_all,idx] = sort(x_all);
fval_all = fval_all(idx);
%We select the best (lowest in value)
if fv1>fv2
    x=fv2;
    fval=v2;
else
    x = v1;
    fval = fv1;
end
end