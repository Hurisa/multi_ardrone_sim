clc
clear
Alignd = uigetdir(pwd, 'Select a folder with alignment .mat files');
nAlignd = uigetdir(pwd, 'Select a folder with no_alignment .mat files');

Afiles = dir(fullfile(strcat(Alignd,'/', '*.mat')));
NAfiles = dir(fullfile(strcat(nAlignd,'/', '*.mat')));

global realVar
realVar=input('Are these real experiments? (true / false): ');
if size(Afiles,1)==size(NAfiles,1)
    ProbAreaCell=cell(2,size(Afiles,1));
    for f=1:size(Afiles,1)        
        ProbAreaCell{1,f}=ProbD(strcat(Alignd,'/',Afiles(f).name));
        ProbAreaCell{2,f}=ProbD(strcat(nAlignd,'/',NAfiles(f).name));
    end
else
    disp('Number of parameter files do not match');
end

T=120;
%% Plot 1-coverage
%plotProb(1,ProbAreaCell,T,Afiles,NAfiles,3200)


%% Plot 2-coverage
plotProb(2,ProbAreaCell,T,Afiles,NAfiles,3200)


%% Plot 3-coverage
%plotK(3,ProbAreaCell,T,Afiles,NAfiles,40)


%%

function plotProb(k,KCovCell,T,Afiles,NAfiles,limitX)
global realVar
figure
hold on
box on

x=[0:limitX];

% lengths=[];
% for f=1:size(Afiles,1)
%    for j=1:2
%        lengths(length(lengths)+1)=size(KCovCell{j,f},1);
%    end  
% end
% 
% timeStep=(T/min(lengths));
% x=0:timeStep:T;
% 
% while size(x,2)>min(lengths)
%     x(:,size(x,2))=[];
% end
%%%%%%
for m1=1:size(Afiles,1)
    meanKcov=sum(sum(KCovCell{1,m1}(:,:,:,k)));
    meanKcovVect=[meanKcov(:)];

    averages{:,m1}=mean(meanKcovVect);
        %averages{:,m1}=averages{:,m1}(1:length(x));
    stds{:,m1}=std(meanKcovVect,0,1);
        %stds{:,m1}=stds{:,m1}(1:length(x));
        
    %top=averages{:,m1}+stds{:,m1};
    %bottom=averages{:,m1}-stds{:,m1};
    
    
    %fillColour = [135,206,235] / 255;
    MeanColour = [25,25,112] / 255; 
    %a=0.3;
    %x2 = [x, fliplr(x)];
    %inBetween = [bottom', fliplr(top')];
    %s=fill(x2, inBetween,fillColour,'LineStyle','none');
    %alpha(s,a);
   
    if (realVar)
        params{m1}=strcat('\mu','=',Afiles(m1).name(1:3),'   ');
    else
        params{m1}=strcat('\mu','=',Afiles(m1).name(31:33),'   ');
    end
end


vector=[160 200 240];
for aa=1:size(averages,2)
    sigma=stds{:,aa};
    mu=averages{:,aa};
    y = exp(- 0.5 * ((x - mu) / sigma) .^ 2) / (sigma * sqrt(2 * pi));
    h(aa)=plot(x,y,'color',vector./255,'LineWidth',2);
    vector(1)=vector(1)-30;
    vector(2)=vector(2)-30;
end

%%%%%%
for m1=1:size(NAfiles,1)
    meanKcov=sum(sum(KCovCell{2,m1}(:,:,:,k)));
    meanKcovVect=[meanKcov(:)]; 
    
    averages{:,m1}=mean(meanKcovVect);
        %averages{:,m1}=averages{:,m1}(1:length(x));
    stds{:,m1}=std(meanKcovVect,0,1);
        %stds{:,m1}=stds{:,m1}(1:length(x));
        
    %top=averages{:,m1}+stds{:,m1};
    %bottom=averages{:,m1}-stds{:,m1};
    
    %fillColour = [255,99,71] / 255; 
    MeanColour = [128,0,0] / 255;  
    %a=0.2;

    %x2 = [x, fliplr(x)];
    %inBetween = [bottom', fliplr(top')];
    %s=fill(x2, inBetween,fillColour,'LineStyle','none');
    %alpha(s,a);

end

vector=[246 150 150];
for aa=1:size(averages,2)
    sigma=stds{:,aa};
    mu=averages{:,aa};
    y = exp(- 0.5 * ((x - mu) / sigma) .^ 2) / (sigma * sqrt(2 * pi));
    g(aa)=plot(x,y,'color',vector./255,'Linewidth',2);
    vector(2)=vector(2)-30;
    vector(3)=vector(3)-30;
end
[~, hobj, ~, ~]=legend([h,g],horzcat(params,params));
ht = findobj(hobj,'type','text');
set(ht,'FontSize',25);
hl = findobj(hobj,'type','line');
set(hl,'LineWidth',4);


xlabel('N cells')
ylabel('P(Xk)')
set(gca,'FontSize',25)
%title(strcat(num2str(k),'-Coverage over time'))
set(gcf,'Position',[100 100 1200 700])
legend('boxoff')
xlim([0, limitX])
hold off



end













