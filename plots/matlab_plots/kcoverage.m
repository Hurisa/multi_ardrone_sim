function kCVal=kcoverage(varargin)

    if (size(varargin,2)==0)   
        [file, path] = uigetfile({'*.*'},'Select .mat file');
        data=load(strcat(path,file));
    else
        data=varargin{1};
        file=varargin{2};
    end
    
    sRad=2;                          % in grid tiles

    limits=data.limits; 
    GridSize=0.5;
    xGrid=limits(1):GridSize:limits(2);
    yGrid=limits(3):GridSize:limits(4);

    D = size(data.poses.position,2);    % Number of Drones   
    T = size(data.poses.position{1},1); % Total time
    R = size(data.poses.position,1);    % Number of Runs  

    Ks=[1 2 3];
    
    kCVal=zeros(T,size(Ks,2),R);
    
    for rr=1:R
        rr
        for tt=1:T
            Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1);
            for dd=1:D

                [xG, yG]=findInGrid(data.poses.position{rr,dd}(tt,:),xGrid,yGrid);
                xSense=max(1,xG-sRad):min(size(xGrid,2)-1,xG+sRad);
                ySense=max(1,yG-sRad):min(size(yGrid,2)-1,yG+sRad);
                Area(xSense,ySense)=Area(xSense,ySense)+1;              
         
            end
            
            for kk=1:size(Ks,2)
                
               kCVal(tt,kk,rr)=sum(sum(Area==Ks(kk)));
                
            end
        end
    end
    
    %% Plotting
    figure(1)
    hold on
    set(gcf,'Position',[100 100 1000 500])
    xlabel('time [s]')
    ylabel('#Cells')
    set(gca,'FontSize',20)
    axis([0 data.Time -10 300])
    
    timeStep=(data.Time/T);
    x=0:timeStep:(data.Time);
    x(length(x))=[];
    
    %Select colors etc
    %build another function for comparisson between params for a single
    %k-coverage
end

function [xG,yG]=findInGrid(pos,xGrid,yGrid)
        
        tmpX=[xGrid,pos(1,1)];
        tmpX=sort(tmpX);
        xG = find(tmpX==pos(1,1))-1;
        
        tmpY=[yGrid,pos(1,2)];
        tmpY=sort(tmpY);
        yG = find(tmpY==pos(1,2))-1;   

end
