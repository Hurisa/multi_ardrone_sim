% Alignd = uigetdir(pwd, 'Select a folder with alignment .mat files');
% nAlignd = uigetdir(pwd, 'Select a folder with no_alignment .mat files');
% 
% Afiles = dir(fullfile(strcat(Alignd,'/', '*.mat')));
% NAfiles = dir(fullfile(strcat(nAlignd,'/', '*.mat')));
% 
% if size(Afiles,1)==size(NAfiles,1)
%     KCovCell=cell(2,size(Afiles,1));
%     for f=1:size(Afiles,1)        
%         KCovCell{1,f}=kcoverage(strcat(Alignd,'/',Afiles(f).name));
%         KCovCell{2,f}=kcoverage(strcat(nAlignd,'/',NAfiles(f).name));
%     end
% else
%     disp('Number of parameter files do not match');
% end

T=1800;
%% Plot 1-coverage
plotK(1,KCovCell,T,Afiles,NAfiles,350)


%% Plot 2-coverage
plotK(2,KCovCell,T,Afiles,NAfiles,150)


%% Plot 3-coverage
plotK(3,KCovCell,T,Afiles,NAfiles,40)


%%

function plotK(k,KCovCell,T,Afiles,NAfiles,limitY)

figure
hold on
box on


lengths=[];
for f=1:size(Afiles,1)
   for j=1:2
       lengths(length(lengths)+1)=size(KCovCell{j,f},1);
   end  
end

timeStep=(T/min(lengths));
x=0:timeStep:T;

while size(x,2)>min(lengths)
    x(:,size(x,2))=[];
end
%%%%%%
for m1=1:size(Afiles,1)
    meanKcov=KCovCell{1,m1}(:,k,:);
    averages{:,m1}=mean(meanKcov,3);
        averages{:,m1}=averages{:,m1}(1:length(x));
    stds{:,m1}=std(meanKcov,0,3);
        stds{:,m1}=stds{:,m1}(1:length(x));
        
    top=averages{:,m1}+stds{:,m1};
    bottom=averages{:,m1}-stds{:,m1};
    
    
    fillColour = [135,206,235] / 255;
    MeanColour = [25,25,112] / 255; 
    a=0.3;
    x2 = [x, fliplr(x)];
    inBetween = [bottom', fliplr(top')];
    s=fill(x2, inBetween,fillColour,'LineStyle','none');
    alpha(s,a);
   
    params{m1}=strcat('\mu','=',Afiles(m1).name(31:33),'   ');
end


vector=[160 200 240];
for aa=1:size(averages,2)
    h(aa)=plot(x,averages{aa}(1:length(x)),'color',vector./255,'LineWidth',2);
    vector(1)=vector(1)-30;
    vector(2)=vector(2)-30;
end

%%%%%%
for m1=1:size(NAfiles,1)
    meanKcov=KCovCell{2,m1}(:,k,:);
    averages{:,m1}=mean(meanKcov,3);
        averages{:,m1}=averages{:,m1}(1:length(x));
    stds{:,m1}=std(meanKcov,0,3);
        stds{:,m1}=stds{:,m1}(1:length(x));
        
    top=averages{:,m1}+stds{:,m1};
    bottom=averages{:,m1}-stds{:,m1};
    
    fillColour = [255,99,71] / 255; 
    MeanColour = [128,0,0] / 255;  
    a=0.2;

    x2 = [x, fliplr(x)];
    inBetween = [bottom', fliplr(top')];
    s=fill(x2, inBetween,fillColour,'LineStyle','none');
    alpha(s,a);

end

vector=[246 150 150];
for aa=1:size(averages,2)
    g(aa)=plot(x,averages{aa}(1:length(x)),'color',vector./255,'Linewidth',2);
    vector(2)=vector(2)-30;
    vector(3)=vector(3)-30;
end
[~, hobj, ~, ~]=legend([h,g],horzcat(params,params));
ht = findobj(hobj,'type','text');
set(ht,'FontSize',25);
hl = findobj(hobj,'type','line');
set(hl,'LineWidth',4);


xlabel('time [s]')
ylabel('K(t)')
set(gca,'FontSize',25)
title(strcat(num2str(k),'-Coverage over time'))
set(gcf,'Position',[100 100 1200 700])
legend('boxoff')
axis([0, T,-10, limitY])
hold off



end













