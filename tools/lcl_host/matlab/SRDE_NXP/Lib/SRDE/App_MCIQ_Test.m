load ../../Data/IqExamples/Iq_1way_LOS;
Iq = Iq/1000;
freq_rast = (0:1e6:79e6).'; dist_grid = -10:0.05:100;
[d, y, d_all,y_all,Param] = SRDE(Iq*100,freq_rast,1);
[~, ~, d_all_complex,y_all_complex,~] = SRDE(Iq*100,freq_rast,2);
y_all_2 = Music(Iq,freq_rast,dist_grid);
% figure; plot(d_all,y_all/30); hold on; plot(d_all_complex,y_all_complex/30,'--'); plot(dist_grid,1./y_all_2);
plot(d_all,y_all); hold on; plot(d_all_complex,y_all_complex,'r--');