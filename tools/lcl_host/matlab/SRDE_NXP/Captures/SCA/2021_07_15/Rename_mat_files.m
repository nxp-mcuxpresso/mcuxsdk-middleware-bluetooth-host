[raw,txt] = xlsread('measurements.xlsx');
num = raw(:,1);
pos = raw(:,2);
foldname = txt;
for m = 1:length(num)
    cd(foldname{m});
    movefile(sprintf('20210715T%d.mat',num(m)),sprintf('Pos%d.mat',pos(m)));
    cd('..');
end