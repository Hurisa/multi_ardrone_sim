
Alignd = uigetdir(pwd, 'Select a folder with alignment .mat files');
nAlignd = uigetdir(pwd, 'Select a folder with no_alignment .mat files');

Afiles  = dir(fullfile(strcat(Alignd,'/', '*.mat')));
NAfiles = dir(fullfile(strcat(nAlignd,'/', '*.mat')));

MeanCellCount=[];
sd=[];
levels=10:15:220;

axSimpleA=[10, 220,-10, 600];
axSimpleNA=[10, 220,-10, 1000];

axCumul=[10,220,-10,1650];

for ff=1:size(Afiles,1)        
    ff
    [MeanCellCount(ff,:), sd(ff,:), MeanCellCountCumulative(ff,:), sdCumulative(ff,:)]=CountLevels(strcat(Alignd,'/',Afiles(ff).name),levels);
end

plotLevels(MeanCellCount, sd, 'true', Afiles, levels, axSimpleA)
plotLevels(MeanCellCountCumulative, sdCumulative, 'true', Afiles, levels,axCumul)

MeanCellCount=[];
sd=[];
for ff=1:size(NAfiles,1)   
    ff
    [MeanCellCount(ff,:), sd(ff,:), MeanCellCountCumulative(ff,:), sdCumulative(ff,:)]=CountLevels(strcat(nAlignd,'/',NAfiles(ff).name),levels);
end


plotLevels(MeanCellCount, sd, 'false', NAfiles, levels,axSimpleNA)
plotLevels(MeanCellCountCumulative, sdCumulative, 'false', NAfiles, levels,axCumul)

%%
function plotLevels(MeanCellCount, sd, al, files, x, axs)


if al
    a=.2;
    fillColour = [160,200,255] / 255;
    MeanColour = [25,25,112] / 255;
    vector=[200 240 255];
else
    a=0.3;
    fillColour = [255,99,71] / 255;
    MeanColour = [128,0,0] / 255;
    vector=[255 200 200];
end

figure
hold on
box on

for ff=1:size(files,1) 
    params{ff}=strcat('\mu','=',files(ff).name(31:33),'   ');
    top=MeanCellCount(ff,:)+sd(ff,:);
    bottom=MeanCellCount(ff,:)-sd(ff,:);
    
    x2 = [x, fliplr(x)];
    inBetween = [bottom, fliplr(top)];
    s=fill(x2, inBetween,fillColour,'LineStyle','none');
    alpha(s,a);
end

for ff=1:size(files,1)
    g(ff)=plot(x,MeanCellCount(ff,:),'color',vector./255,'Linewidth',2.5);
    if al
        vector(1)=vector(1)-65;
        vector(2)=vector(2)-65;
    else
        vector(2)=vector(2)-65;
        vector(3)=vector(3)-65;
    end
end

[~, hobj, ~, ~]=legend(g,params);
ht = findobj(hobj,'type','text');
set(ht,'FontSize',25);
hl = findobj(hobj,'type','line');
set(hl,'LineWidth',4);


xlabel('levels[s]')
ylabel('#Cells')
set(gca,'FontSize',25)
%title(strcat(num2str(k),'-Coverage over time'))
set(gcf,'Position',[100 100 1200 700])
legend('boxoff')
axis(axs)
hold off

end

