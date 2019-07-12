function [MeanCellCount, sd]=CountLevelsCumulative(levels,Area)

meanArea=mean(Area,3);

MeanCellCount=zeros(size(levels));
ICellCount=zeros([size(Area,3),size(levels,2)]);

for zz=1:size(Area,3)    
    for ww=1:size(ICellCount,2)
        if ww==1
            ICellCount(zz,ww)=sum(sum((Area(:,:,zz)<levels(ww))));
        else
            ICellCount(zz,ww)=sum(sum((Area(:,:,zz)<levels(ww))));
        end
    end
end

for ii=1:size(MeanCellCount,2)
    if ii==1
        MeanCellCount(ii)=sum(sum((meanArea<levels(ii))));
    else
        MeanCellCount(ii)=sum(sum((meanArea<levels(ii))));
    end
end
sd=std(ICellCount,0,1);


end



