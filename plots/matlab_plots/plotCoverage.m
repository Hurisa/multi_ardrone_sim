clear;
clc;
close;

GridSize=0.5;


[file, path] = uigetfile({'*.*'},'Select .mat file');
data=load(strcat(path,file));

limits=data.limits;

xGrid=limits(1):GridSize:limits(2);
yGrid=limits(3):GridSize:limits(4);


Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

x=0:(data.Time/finalLength):(data.Time);
bottom=zeros(1,finalLength);
top=zeros(1,finalLength);
average=zeros(1,finalLength);

if (GridSize ~= 0.5)
    nRuns = size(data.poses,1);
    nUAVs = size(data.poses,2);
    nIter = size(data.poses{1},1);    
    coverage=cell(1,nRuns);
    
    for r=1:nRuns        
        Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1);        
        for it=1:nIter
            for nD=1:nUAVs
                
                tmpX=xGrid;
                tmpX=[xGrid,data.poses{r,nD}(it,1)];
                tmpX=sort(tmpX);
                Xplace = find(tmpX==data.poses{r,nD}(it,1))-1;
                
                tmpY=yGrid;
                tmpY=[yGrid,data.poses{r,nD}(it,2)];
                tmpY=sort(tmpY);
                Yplace = find(tmpY==data.poses{r,nD}(it,2))-1;
                
                if (limits(1)<data.poses{r,nD}(it,1) && data.poses{r,nD}(it,1)<limits(2) && limits(3)<data.poses{r,nD}(it,2) && data.poses{r,nD}(it,2)<limits(4))
                    if (Area(Xplace,Yplace)==0)
                        Area(Xplace,Yplace)=1;
                    end  
                end           
            end
            coverage{1,r}(it,1)=sum(sum(Area))/(size(Area,1)*size(Area,2));
        end
    end
    for ii=1:finalLength
        bottom(ii)=min(cellfun(@(v)v(ii),coverage));
        average(ii)=mean(cellfun(@(v)v(ii),coverage));
        top(ii)=max(cellfun(@(v)v(ii),coverage));
    end
    
else      
    for ii=1:finalLength
        bottom(ii)=min(cellfun(@(v)v(ii),data.coverage));
        average(ii)=mean(cellfun(@(v)v(ii),data.coverage));
        top(ii)=max(cellfun(@(v)v(ii),data.coverage));
    end    
end

while size(x,2)>finalLength
    x(:,size(x,2))=[];
end


close
hold on
box on
axis([0 x(1,size(x,2)) 0 1])

colour=input('colour? (red, blue, green) ','s');
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

x2 = [x, fliplr(x)];
inBetween = [bottom, fliplr(top)];
s=fill(x2, inBetween,fillColour,'LineStyle','none');
alpha(s,a);
plot(x,average,'color',MeanColour,'Linewidth',2)
xlabel('Time [s]')
ylabel('E(t)')
set(gca,'FontSize',20)
hold off

% filename=strcat('grid',num2str(GridSize),'_',file(1:length(file)-4),'_average','.mat');
% save(filename, 'average');
% 
% 


