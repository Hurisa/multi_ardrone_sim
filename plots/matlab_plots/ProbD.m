function ProbArea=ProbD(varargin)

    if (size(varargin,2)==0)   
        [file, path] = uigetfile({'*.*'},'Select .mat file');
        data=load(strcat(path,file));
        cellSize=input('Cell size(m) is: ');
    else
        data=load(varargin{1});
        cellSize=varargin{2};
        TPerCent=varargin{3};
%         data=varargin{1};
%         file=varargin{2};
    end
    


    limits=data.limits; 

    xGrid=limits(1):cellSize:limits(2);
    yGrid=limits(3):cellSize:limits(4);

    D = size(data.poses.position,2);                        % Number of Drones   
    T = round(size(data.poses.position{1},1)*TPerCent);     % Total time
    R = size(data.poses.position,1);                        % Number of Runs  

    Ks=[1 2 3];
    
    kCVal=zeros(T,size(Ks,2),R);    
    sRad=1/GridSize;                          % in grid tiles
    
    
    ProbArea=zeros(size(xGrid,2)-1,size(yGrid,2)-1,R,size(Ks,2)); 
    for rr=1:R
      
        for tt=1:T
            Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1);
            for dd=1:D

                [xG, yG]=findInGrid(data.poses.position{rr,dd}(tt,:),xGrid,yGrid);
                xSense=max(1,xG-sRad):min(size(xGrid,2)-1,xG+sRad);
                ySense=max(1,yG-sRad):min(size(yGrid,2)-1,yG+sRad);
                Area(xSense,ySense)=Area(xSense,ySense)+1;              
         
            end
            
            for kk=1:size(Ks,2)
                 [x,y]=find(Area==Ks(kk));
               %kCVal(tt,kk,rr)=sum(sum(Area==Ks(kk)));
                ProbArea(x,y,rr,kk)=1;
            end
        end
    end
end

function [xG,yG]=findInGrid(pos,xGrid,yGrid)
        
        tmpX=[xGrid,pos(1,1)];
        tmpX=sort(tmpX);
        xG = find(tmpX==pos(1,1))-1;
        
        tmpY=[yGrid,pos(1,2)];
        tmpY=sort(tmpY);
        yG = find(tmpY==pos(1,2))-1;   

end
