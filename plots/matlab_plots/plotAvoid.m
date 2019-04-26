clear;
clc;
close;

[file, path] = uigetfile({'*.mat'},'Select .mat file');
data=load(strcat(path,file));

limits=data.limits;

Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

timeStep=(data.Time/finalLength);
nPoses=size(data.poses,2);
nRuns=size(data.coverage,2);



x=0:timeStep:(data.Time);
bottom=zeros(1,finalLength);
top=zeros(1,finalLength);
average=zeros(1,finalLength);
AvoidTimes=zeros(finalLength,nPoses,nRuns);
for rr=1:nRuns
     for pp=1:nPoses        
        counter=1;
        vectData=[data.avoidState{1,rr,pp}(:).Data];
        vectData(1)=0;
        
        times=find(vectData);
        prev=vectData(times-1);       
        
        transition=times(find(prev==0));
        instances=size(find(prev==0),2);
        for ii=1:instances-1
            AvoidTimes(transition(ii):transition(ii+1),pp,rr)=counter;%*timeStep;
            counter=counter+1;
        end
        AvoidTimes(transition(instances):finalLength,pp,rr)=counter-1;%*timeStep;
        
        
     end
 end    

while size(x,2)>finalLength
    x(:,size(x,2))=[];
end


close
hold on
box on
%axis([0 x(1,size(x,2)) 0 200])

meanAvoidTimes=mean(AvoidTimes,3);
average=mean(meanAvoidTimes,2)';
top=(max(meanAvoidTimes,[],2))';
bottom=(min(meanAvoidTimes,[],2))';
% % 
colour='red';
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




