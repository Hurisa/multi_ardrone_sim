function plotPersistCoverage(varargin)

GridSize=0.5;


[file, path] = uigetfile({'*.*'},'Select .mat file');
if (size(varargin,2)==0)   
    data=load(strcat(path,file));
else
    data=varargin{1};
end

limits=data.limits;

xGrid=limits(1):GridSize:limits(2);
yGrid=limits(3):GridSize:limits(4);

colorbarLimits=[0 35];
thresh=0.5;

NCells=size(xGrid,2);
    
Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);
nRuns = size(data.poses,1);
nUAVs = size(data.poses,2);
nIter = size(data.poses{1},1);   

timeStep=(data.Time/finalLength);

Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1,nRuns);
Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

for rr=1:nRuns
    for uu=1:nUAVs
        for ggX=1:NCells-1
            for ggY=1:NCells-1
                                
                a=find(data.poses{rr,uu}(:,1)<xGrid(ggX+1) & data.poses{rr,uu}(:,1)>xGrid(ggX));
                b=find(data.poses{rr,uu}(:,2)<xGrid(ggY+1) & data.poses{rr,uu}(:,2)>xGrid(ggY));
                nHits=size(intersect(a,b),1);                
                Area(ggX,ggY,rr)=Area(ggX,ggY,rr)+timeStep*nHits;
               
            end
        end
    end
end



figure
colormap('parula');
meanArea=mean(Area,3);

CellsAboveThres=size(find(meanArea>=colorbarLimits(2)*thresh),1)/(size(meanArea,1)*size(meanArea,2));


imagesc(xGrid,yGrid,meanArea);
colorbar;
caxis(colorbarLimits)
xlabel('X [m]')
ylabel('Y [m]')
title({strcat('\mu = ',file(length(file)-6:length(file)-4)) })
set(gca,'YDir','normal')
saveas(gcf,strcat('PCov_',file(1:length(file)-4),'.png'))
% while size(x,2)>finalLength
%     x(:,size(x,2))=[];
% end

end



