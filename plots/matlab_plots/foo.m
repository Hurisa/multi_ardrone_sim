%close;
function  CumulativeTimes=foo(EvalHits,timeStep,x)
%     [file, path] = uigetfile({'*.mat'},'Select .mat file');
%     data=load(strcat(path,file));
% 
%     EvalHits=data.EvalHits;
%     AvoidHits=data.AvoidHits;
%     rawHits=data.rawHits;

    finalLength=size(EvalHits,1);
    nPoses=size(EvalHits,2);
    nRuns=size(EvalHits,3);

    CumulativeTimes=zeros(size(EvalHits));
    for rr=1:nRuns
        for pp=1:nPoses

           CumulativeTimes(:,pp,rr)=cumsum(EvalHits(:,pp,rr)).*timeStep;

        end
    end
   
close
hold on
box on
%axis([0 x(1,size(x,2)) 0 200])

meanAvoidTimes=mean(CumulativeTimes,3);
average=mean(meanAvoidTimes,2)';
top=(max(meanAvoidTimes,[],2))';
bottom=(min(meanAvoidTimes,[],2))';
% % 
colour='blue';
if strcmp(colour,'red')
    fillColour = [255,99,71] / 255; 
    MeanColour = [128,0,0] / 255;  
    a=0.2;
elseif strcmp(colour,'blue')
    fillColour = [135,206,235] / 255; 
    MeanColour = [25,25,112] / 255; 
    a=0.5;
elseif strcmp(colour,'green')
    fillColour = [34,139,34] / 255; 
    MeanColour = [85,107,47] / 255; 
    a=0.2;
end
% 
x2 = [x, fliplr(x)];
inBetween = [bottom, fliplr(top)];
s=fill(x2, inBetween,fillColour,'LineStyle','none');
alpha(s,a);
plot(x,average,'color',MeanColour,'Linewidth',2)
xlabel('Time [s]')
ylabel('A(t)')
set(gca,'FontSize',20)
hold off

% filename=strcat('grid',num2str(GridSize),'_',file(1:length(file)-4),'_average','.mat');
% save(filename, 'average');


    
    
end
