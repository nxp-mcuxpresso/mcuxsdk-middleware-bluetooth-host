%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef RV_chanmodel
    % Description
    %     This model describes a simple non-clustered Saleh-Valenzuela channel
    %     model, which has been extended to support AoA. The AoA can be used to
    %     simulation locally-coherent time-variant channels, where the number of
    %     paths/ray does not change, but the phase of the individual rays does.
    %     Example is Antenna Array or Doppler-shifts.
    % Description end
    
    properties
        A=2/10;         %[dB/ns] %A is the decay of the NLOS-PDP starting from the LOS-component
        Delay_LOS=20;   %[ns]
        K=1;            %[linear] ratio between LOS_power/NLOS_power (Average/expected)
        lambda=1/4;     %lambda is the ray arrival rate [1/ns]
        P_total=1;      %By default the expected/average \ME[\abs( H(f) )^2]= 1
        phase_0=0;      %Phase shift of the LOS component
    end
    properties (Dependent)
        P_los;
        P_nlos;
        gamma
        F
        DS_rms_NLOS
    end
    methods
        function string=disp(self)
            string=['Ricean-ChanModel,K=' num2str(round(10*10*log10(self.K))/10) '[dB],DS_{nlos}=' num2str(round(self.DS_rms_NLOS)) '[ns],ExpRayInt=' num2str(round(1/self.lambda)) '[ns]'];
        end
        function self=set.P_total(self,value)
            self.P_total=value;
        end
        function value = get.P_los(self)
            if isinf(self.K)
                value=self.P_total;
            else
                value=self.K*self.P_total/(self.K+1);
            end
        end
        function value = get.P_nlos(self)
            value=self.P_total-self.P_los;
        end
        
        function value = get.gamma(self)
            value=log(10)*self.A/10;
        end
        function value = get.F(self)
            value=self.gamma*self.P_nlos;
        end
        function DS_rms_NLOS=get.DS_rms_NLOS(self)
            DS_rms_NLOS=1/self.gamma;
        end
        function self=set.DS_rms_NLOS(self,DS)
            self.A=10/DS/log(10);
        end
        
        function [h,tau]=do(self)
            Tmax=20/(self.gamma);                                                       % At Tmax, we truncate the channel response
            tau_diff=exp_RV(1/self.lambda,1,round(1*Tmax*self.lambda));                 % Assume poisson-distribution for the multipath rays
            tau=cumsum([self.Delay_LOS tau_diff]);                                      % tau is vector containing the ray arrival times. The first path is the LOS
            WhiteNoise=[1 1i]*randn(2,length(tau))/sqrt(2);                             % Rayleigh fading(Mag) and Random Phase on each individual ray
            h=sqrt(self.F/self.lambda)*sqrt(exp(-self.gamma*(tau-tau(1)))).*WhiteNoise; % of which the expected value depend on exponential decay( tau-(tau of LOS) )
            h(1)=sqrt(self.P_los);                                                      % amplitude is of LOS-ray is deterministic
            I=find(20*log10(abs(h))-20*log10(abs(h(1)))>-200);                          % Truncate based on magnitudes
            tau=tau(I);                                                                 % ...    
            h=h(I);                                                                     % ...
        end
        function [AoA,r_x,r_y]=do_AoA(self,tau)
            theta=2*pi*rand(length(tau));
            tau=tau*1e-9*3e8;
            c0=-tau(1)/2;
            for cnt_tau=1:length(tau)
                a = tau(cnt_tau)/2;
                b = sqrt((tau(cnt_tau)/2)^2 -(tau(1)/2)^2);
                r_x(cnt_tau) = a*cos(theta(cnt_tau));
                r_y(cnt_tau) = b*sin(theta(cnt_tau));
                AoA(cnt_tau)= atan2(r_y(cnt_tau),(r_x(cnt_tau)-c0));
            end
        end
        function plot_AOA(self,data)
            tau=data.tau;
            h=data.h;
            AoA=data.AoA;
            r_x=data.pos_reflect(:,1);
            r_y=data.pos_reflect(:,2);
            
            COLORS=-3:-1:-40;
            clrmp=hot(length(COLORS));
           
            t_all=(0:0.01:1)*2*pi;
            c0=3e8;
            x0=-tau(1)/2*c0;
            x1=tau(1)/2*c0;
            for cnt_tau=length(tau):-1:1
                Pwr_db=20*log10(abs(h(cnt_tau)));
                ColorIndex=interp1(COLORS,clrmp,Pwr_db);
                if ~sum(isnan(ColorIndex))
                    for cnt_t=1:length(t_all)
                        a = tau(cnt_tau)/2;
                        b = sqrt((tau(cnt_tau)/2)^2 -(tau(1)/2)^2);
                        x(cnt_t) = a*cos(t_all(cnt_t));
                        y(cnt_t) = b*sin(t_all(cnt_t));
                    end
                    figure(1);hdl=plot(x*c0,y*c0,':');hold on
                    hdl2=plot([x0 r_x(cnt_tau) x1],[0 r_y(cnt_tau) 0]);
                    set([hdl hdl2],'color',ColorIndex)
                end
            end
            axis equal
        end
        
        function [data]=do_n(self,NofChannels)
            for cnt=1:NofChannels
                [h,tau]=do(self);
                [AoA,r_x,r_y]=do_AoA(self,tau);
                data(cnt).tau=tau*1e-9;
                data(cnt).h=h;
                data(cnt).AoA=AoA;
                data(cnt).pos_reflect=[r_x' r_y'];
                dist=3e8*data(cnt).tau(1);
                data(cnt).pos_tx=[-dist/2 0];
                data(cnt).pos_rx=[dist/2 0];
                data(cnt).pos_reflect(1,:)=data(cnt).pos_tx;
            end
        end
        function [PDP_out]=PDP_dB(self,tau)
            PDP_out=(10*log10(self.F)-self.A*(tau-self.Delay_LOS));
            [~,I]=min(abs(tau-self.Delay_LOS));
            PDP_out(I)=10*log10(self.P_los);
            PDP_out=(10.^(PDP_out/10));
            PDP_out(1:I-1)=0;
        end
        
        function [ME_H]=PLOT_H_corr(self)
            %Compares  simulated freq domain correlation with theory (theory_ME_Hf)
            f=2e9+(0:8:1024/8)*1e6;
            NofChannels=1600;
            [data]=do_n(self,NofChannels);
            
            HH=zeros(NofChannels,length(f));
            for cnt=1:length(data)
                [H]=H_freq(data(cnt).tau,data(cnt).h,f);
                HH(cnt,:)=H;
            end
            ME_H=mean(abs(HH).^2);
            close all
            plot(f,10*log10(abs(ME_H)))
            
            figure
            mesh(f,f,real((HH'*HH)/NofChannels))
            hold on
            NofTones=length(f);
            F_toeplitz=f'*ones(1,NofTones)-(ones(NofTones,1)*f); %Frequency difference
            [M]=theory_ME_Hf(self,F_toeplitz);
            mesh(f,f,real(M))
        end
        function []=MatchAR(self,P)
            delta_f=2e6;
            f=2.4e9+(0:1:P)*delta_f;
            F_toeplitz=f'*ones(1,P+1)-(ones(P+1,1)*f); %Frequency difference
            [~,~,~,R,]=theory_ME_Hf(self,F_toeplitz);
            v=R(2:P+1,1);
            a=-R(1:P,1:P)\v;
            sigma2_n=R(1)+real(v'*a);
            a=[1 a.'];
            zplane(1,a)
            noise=sqrt(sigma2_n/2*self.P_nlos)*[1 1i]*randn(2,100000);
            H_sim=filter(1,a,noise)+sqrt(self.P_los);
            figure;plot(20*log10(abs(H_sim)))
            figure;hist(sqrt(abs(H_sim).^2),50)
        end
        
        function [M,M_los,M_nlos,M_nlosAtZero,RealPartM_nlosAtZero]=theory_ME_Hf(self,delta_f)
            %Theoretical mathematical expectation of the frequency-domain channel-transfer-function
            %depends only on the frequency difference
            M_los = self.P_los.*exp(-2i*pi*delta_f*self.Delay_LOS*1e-9);
            M_nlos= self.P_nlos.*exp(-2i*pi*delta_f*self.Delay_LOS*1e-9)./(1+2i*pi*delta_f*(1e-9)/self.gamma);
            M_nlosAtZero=1./(1+2i*pi*delta_f*(1e-9)/self.gamma);
            %RealPartM_nlosAtZero_1=real(M_nlosAtZero)
            RealPartM_nlosAtZero=1./(1+(2*pi*delta_f*(1e-9)/self.gamma).^2);
            M=M_los+M_nlos;
        end
        
        function [p]=LOS_component(self,f)
            p=sqrt(self.P_los).*exp(-2i*pi*f*self.Delay_LOS*1e-9).*exp(1i*self.phase_0);
        end
        function S=eig_nlos(self,f)
            NofTones=length(f);
            F_toeplitz=f'*ones(1,NofTones)-(ones(NofTones,1)*f); %Frequency difference
            [~,~,M_nlos]=theory_ME_Hf(self,F_toeplitz);
            [~,S,~] = svd(M_nlos);
        end
        function DIV=Div_nlos(self,f)
            S=eig_nlos(self,f);
            DIV=trace(S)^2/trace(S.^2);
        end
        function H=do_n_freq(self,f,NofChannels)
            NofTones=length(f);
            F_toeplitz=f(:)*ones(1,NofTones)-(ones(NofTones,1)*f(:).'); %Frequency difference
            [~,~,~,M_nlosAtZero]=theory_ME_Hf(self,F_toeplitz);
            R_nlos2 = cholcov2(M_nlosAtZero)';
            
            WN=(randn(size(R_nlos2,2),NofChannels)+1i*randn(size(R_nlos2,2),NofChannels))/sqrt(2);
            CorreledNoise2=R_nlos2*WN;
            
            DD=LOS_component(self,f(:));
            H=DD*ones(1,NofChannels)+diag(DD).*sqrt(self.P_nlos)*CorreledNoise2;
        end
        function H=do_n_freq_norm(self,f,NofChannels)
            %same as above, but normalized with respect to self.P_los
            H=do_n_freq(self,f,NofChannels)/sqrt(self.P_los);
        end
        function pdf_out=pdf(self,x,f)
            if size(x,2)>1
                for cnt=1:size(x,2)
                    pdf_out(cnt)=pdf(self,x(:,cnt),f);
                end
            else
                K_f=length(f);
                One_vec=ones(K_f,1);
                
                F_diff=f(:)*One_vec'-One_vec*f(:).';
                [s]=LOS_component(self,f(:));%steering vector
                [~,~,X]=theory_ME_Hf(self,F_diff);
                C=self.P_nlos*X;
                [U,EV]=eig(C);
                EuclDist=0;
                for cnt2=1:K_f %K_f= equal to number of eigenvalues
                    EuclDist=EuclDist+1/(EV(cnt2,cnt2))*norm((x-s)'*diag(s/sqrt(self.P_los))*U(:,cnt2))^2;
                end
                pdf_out=exp(-EuclDist)/(prod(pi*diag(EV)));
            end
        end
        
        function Cov_AngularNoise=Cov_PhaseErr(self,f)
            RangeEngine=Phases2Range();
            RangeEngine.delta_F=mean(diff(f));
            
            %Theory part
            NofTones=length(f);
            F_toeplitz=f'*ones(1,NofTones)-(ones(NofTones,1)*f); %Frequency difference
            [~,~,~,~,RealPartM_nlosAtZero]=theory_ME_Hf(self,F_toeplitz);
            
            Cov_AngularNoise=RealPartM_nlosAtZero/(2*self.K);
        end
        
        
        function [tau,PDP_out]=PDP_sim(self,NofChannels,DeltaT)
            [data]=do_n(self,NofChannels);
            if nargin<3
                DeltaT=1e-9;
            end
            NFFT=1024;
            I_offset=20;
            tau=(-I_offset:NFFT)*DeltaT;
            hh=zeros(NofChannels,length(tau));
            for cnt=1:length(data)
                [td,h_td]=CIR2(data(cnt).tau,data(cnt).h,10e9,DeltaT,NFFT);
                offset=I_offset+round(td(1)/DeltaT);
                if(td(1)<0)
                    td(1)
                end
                hh(cnt,offset+(1:NFFT))=h_td;
            end
            PDP_out=mean(abs(hh).^2);
        end
        
        function PLOT_PDP(self)
            Tmax=20/(self.gamma);
            tau=0:0.5:Tmax;
            figure
            DeltaT_all=[0.5e-9 1e-9 2e-9 4e-9];
            plot(tau,PDP_dB(self,tau))
            hold on
            for cnt=1:length(DeltaT_all)
                DeltaT=DeltaT_all(cnt);
                [tau_sim,PDP_out]=PDP_sim(RV_chanmodel,500,DeltaT);
                plot(tau_sim/1e-9,10*log10(PDP_out*1e-9/DeltaT))
                w_nlos=(tau_sim/1e-9                    >self.Delay_LOS+2*DeltaT/1e-9);
                w_los =(abs(tau_sim/1e-9-self.Delay_LOS )<2*DeltaT/1e-9);
                plot(tau_sim/1e-9,w_nlos,'g')
                plot(tau_sim/1e-9,w_los,'r')
                P_NLOS_meas(cnt)=10*log10(trapz(tau_sim/1e-9,PDP_out.*w_nlos));
                P_LOS_meas(cnt) =10*log10(trapz(tau_sim/1e-9,PDP_out.*w_los ));
            end
            disp(P_LOS_meas)
            disp(P_NLOS_meas)
        end
    end
end
function tau=exp_RV(ExpectedValue,a,b)
tau = -log(rand(a,b))*ExpectedValue;
end
