clear;
clc;
%close;
[file, path] = uigetfile({'*.mat'},'Select .mat file');
data=load(strcat(path,file));

Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

figure
hold on
box on

axis([data.limits(1)-1 data.limits(2)+1 data.limits(3)-1 data.limits(4)+1])
NPoses=size(data.poses.position,2);
NRuns=size(data.coverage,2);
stamp=0;

xlabel(strcat('\fontsize{14}','X[m]'));
ylabel(strcat('\fontsize{14}','Y[m]'));
x=zeros(1,NPoses);
y=zeros(1,NPoses);
for r=1:NRuns    
    sprintf('Run %d', r)
    for t=1:finalLength
        for p=1:NPoses
            x(p)=data.poses.position{r,p}(t,1);
            y(p)=data.poses.position{r,p}(t,2);
            
            kplot(p)=rectangle('Position',[x(p)-1.25,y(p)-1.25,2.5,2.5],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none');
        end
        
        
        h=plot(x,y,'bo','Linewidth',2,'MarkerFaceColor','b');
        pause(stamp);
        if (t~=finalLength)
            delete(h);
            delete(kplot);
        end
    end
    if (r~=NRuns)
        delete(h);
        delete(kplot);
    end
end


