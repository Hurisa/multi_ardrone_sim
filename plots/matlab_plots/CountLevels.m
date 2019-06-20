function [MeanCellCount, sd, MeanCellCountCumulative, sdCumulative]=CountLevels(varargin)

GridSize=0.5;
sradius=0.5; %how many times gridsize?

if (size(varargin,2)==0)   
    [file, path] = uigetfile({'*.*'},'Select .mat file');
    data=load(strcat(path,file));
    levels=10:20:220;
else
    data=load(varargin{1});
    levels=varargin{2};
end

limits=data.limits;

xGrid=limits(1):GridSize:limits(2);
yGrid=limits(3):GridSize:limits(4);

%colorbarLimits=[0 35];
colorbarLimits=[15 220];
thresh=0.5;

NCells=size(xGrid,2);
    
Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);
nRuns = size(data.poses.position,1);
nUAVs = size(data.poses.position,2);
nIter = size(data.poses.position{1},1);   

timeStep=(data.Time/finalLength);

Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1,nRuns);
Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

for rr=1:nRuns
    rr
    for uu=1:nUAVs
        for ggX=1:NCells-1
            for ggY=1:NCells-1
                                
                a=find(data.poses.position{rr,uu}(:,1)<xGrid(ggX+1) & data.poses.position{rr,uu}(:,1)>xGrid(ggX));
                b=find(data.poses.position{rr,uu}(:,2)<xGrid(ggY+1) & data.poses.position{rr,uu}(:,2)>xGrid(ggY));
                nHits=size(intersect(a,b),1);                
                %Area(ggX,ggY,rr)=Area(ggX,ggY,rr)+timeStep*nHits;
                
                xSense=[max(1,ggX-(sradius/GridSize)):min(size(Area,1),ggX+(sradius/GridSize))];
                ySense=[max(1,ggY-(sradius/GridSize)):min(size(Area,1),ggY+(sradius/GridSize))];
                Area(xSense,ySense,rr)=Area(xSense,ySense,rr)+timeStep*nHits;
               
            end
        end
    end
end


meanArea=mean(Area,3);

MeanCellCount=zeros(size(levels));


ICellCount=zeros([size(Area,3),size(levels,2)]);

for zz=1:size(Area,3)    
    for ww=1:size(ICellCount,2)
        if ww==1
            ICellCount(zz,ww)=sum(sum((Area(:,:,zz)<levels(ww) & Area(:,:,zz)>0)));
        else
            ICellCount(zz,ww)=sum(sum((Area(:,:,zz)<levels(ww) & Area(:,:,zz)>levels(ww-1))));
        end
    end
    
end

for ii=1:size(MeanCellCount,2)

    if ii==1
        MeanCellCount(ii)=sum(sum((meanArea<levels(ii) & meanArea>0)));
    else
        MeanCellCount(ii)=sum(sum((meanArea<levels(ii) & meanArea>levels(ii-1))));
    end
end
%plot(levels,MeanCellCount)
sd=std(ICellCount,0,1);
[MeanCellCountCumulative, sdCumulative]=CountLevelsCumulative(levels,Area);


end



