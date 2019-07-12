clear;
clc;
close;

subplot(1,2,1)
axis([-21 21 -21 21])
box on
xlabel(strcat('\fontsize{14}','X[m]'));
ylabel(strcat('\fontsize{14}','Y[m]'));

subplot(1,2,2)
axis([-21 21 -21 21])
box on
xlabel(strcat('\fontsize{14}','X[m]'));
ylabel(strcat('\fontsize{14}','Y[m]'));




for ii=1:2
[file{ii}, path{ii}] = uigetfile({'*.mat'},'Select .mat file');
data{ii}=load(strcat(path{ii},file{ii}));
end

Len = cellfun(@length, data{1}.coverage, 'UniformOutput', false);
finalLength(1)=min([Len{:}]);

Len=[];
Len = cellfun(@length, data{2}.coverage, 'UniformOutput', false);
finalLength(2)=min([Len{:}]);

NPoses=15;
stamp=0;

% 
% x=zeros(1,NPoses);
% y=zeros(1,NPoses);
r=1;
    %sprintf('Run %d', r)
    
    for t=1:min(finalLength)
        for ii=1:2
            for p=1:NPoses
                x(p)=data{ii}.poses.position{r,p}(t,1);
                y(p)=data{ii}.poses.position{r,p}(t,2);
            end
            if t>1
               delete(h(ii)); 
               delete(s(ii)); 
            end
            subplot(1,2,ii)
            hold on
            axis([-21 21 -21 21])
            s(ii)=scatter(x,y,1200,'filled','MarkerFaceAlpha',0.3,'MarkerFaceColor',[255/255 181/255 105/255]);
            h(ii)=plot(x,y,'bo','Linewidth',2,'MarkerFaceColor','b');
            pause(stamp);
%             if (t~=finalLength)
%                 delete(h);
%             end
        end
%         if (r~=NRuns)
%             delete(h);
%         end
    end
    
    
    
    
    
    
    
    
    
    
    
    
