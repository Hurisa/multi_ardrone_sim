clear;
clc;
close all;
[file, path] = uigetfile({'*.mat'},'Select .mat file');
data=load(strcat(path,file));

Len = cellfun(@length, data.coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

figure
hold on
box on

window=round(finalLength/4);
step=30;

nplots=1:step:window;



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
    for t=1:size(nplots,2)
        x=[];
        y=[];
        u=[];
        v=[];
        for p=1:NPoses
            x(p)=data.poses.position{r,p}(nplots(t),1);
            y(p)=data.poses.position{r,p}(nplots(t),2);
            
            qWXYZ=[data.poses.orientation{r,p}(nplots(t),4),data.poses.orientation{r,p}(nplots(t),1:3)];
            rpy=quat2eul(qWXYZ,'XYZ');
            u(p)=cos(rpy(3));
            v(p)=sin(rpy(3));
            
            th(t,p)=rpy(3);
        end
        h(t)=scatter(x,y,100,'MarkerEdgeColor','w','MarkerFaceColor','b');
        quiver(x,y,u,v,0.18,'r')
        alpha(h(t),t/size(nplots,2)-0.01)
        pause(stamp);
        if (t~=finalLength)
            %delete(h);
        end
    end

end
        
O=EvalOrder(th,nplots);


function O=EvalOrder(angles,nplots)
      figure
      hold on  
      box on
      axis([-20 710 0 1.1])
        for t=1:size(angles,1)
            O(t)=norm(sum(exp(angles(t,:).*1i)))/(size(angles,2)); 

            b(t)=bar(nplots(1,t),O(t),20,'b');
            alpha(b(t),t/size(angles,1))
           
        end
     ylabel(strcat('\fontsize{14}','O(t)'))
     xlabel(strcat('\fontsize{14}','t[s]'))
end
