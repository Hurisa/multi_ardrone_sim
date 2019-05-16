clear; clc;

NRuns=1; %% IMPORTANT: THIS SHOULD BE GOTTEN AUTOMATICALLY %%

[file, path] = uigetfile({'*.*'},'Select limits yaml file');
limitsTxt = fileread(strcat(path,file)); % get the yaml file for limits
limitsStr=split(limitsTxt);
limits=[str2double(limitsStr{2}),str2double(limitsStr{4}),str2double(limitsStr{6}),str2double(limitsStr{8})];
global xGrid yGrid

d = uigetdir(pwd, 'Select a folder');
files = dir(fullfile(d, '*.bag'));

for ff=1:size(files,1)
ff
%[file, path] = uigetfile({'*.*'},'Select bag file');
bag=rosbag(strcat(d,'/',files(ff).name));

msg=select(bag,'Topic',strcat('/swarmState'));

nInter=1000;
chunck=round(size(bag.MessageList,1)/nInter);

msgStruct=readMessages(msg,'DataFormat','struct');


% msgStruct=cell(size(bag.MessageList,1),1);
% msgStruct(1:chunck,1)=readMessages(msg,1:chunck,'DataFormat','struct');
% for nn=2:nInter-1
%     nn
%     msgStruct((nn-1)*chunck+1:nn*chunck,1)=readMessages(msg,(nn-1)*chunck+1:nn*chunck,'DataFormat','struct');    
% end
% msgStruct((nInter-1)*chunck+1:size(bag.MessageList,1),1)=readMessages(msg,(nInter-1)*chunck+1:size(bag.MessageList,1),'DataFormat','struct');

%msgStruct=readMessages(msg,'DataFormat','struct');
%%Avoidmsg=select(bag,'Topic',strcat('/swarmAvoid'));
%%AvoidmsgStruct=readMessages(Avoidmsg,'DataFormat','struct');

Time=(msg.EndTime-msg.StartTime)/NRuns;
grid=[0.5, 0.5];


xGrid=limits(1):grid(1):limits(2);
yGrid=limits(3):grid(2):limits(4);
Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1);

preProc=false;
while (~preProc)
    if (isempty(msgStruct{1}.Poses))
      msgStruct(1)=[];
    else
      preProc=true;  
    end
end
Nposes=size(msgStruct{1,1}.Poses,2);
coverage=cell(1,NRuns);
%avoidState=cell(1,NRuns,Nposes);
run=1; 
iter=1;
poses.position=cell(NRuns,Nposes);
poses.orientation=cell(NRuns,Nposes);

for t=1:size(msgStruct)
    
    %if size((AvoidmsgStruct{t,1}.State),2)>0
        if (t>1 && double(msgStruct{t,1}.Run)<double(msgStruct{t-1,1}.Run) )
            iter=1;
            run=run+1;
            Area=zeros(size(xGrid,2)-1,size(yGrid,2)-1);
        else
            if(t>1)
                iter=iter+1;
            end
        end
        
        for u=1:Nposes
            [Xplace, Yplace, xpos, ypos, qx, qy, qz, qw]=getXYQ(msgStruct{t,1},u);
            poses.position{run,u}(iter,:)=[xpos, ypos];
            poses.orientation{run,u}(iter,:)=[qx, qy, qz, qw];
            if (limits(1)<xpos && xpos<limits(2) && limits(3)<ypos && ypos<limits(4))
                if (Area(Xplace,Yplace)==0)
                    Area(Xplace,Yplace)=1;
                end
            end
            
            %avoidState{1,run,u}(iter,1)=AvoidmsgStruct{t,1}.State(u);
        end
        coverage{1,run}(iter,1)=sum(sum(Area))/(size(Area,1)*size(Area,2));
    %end
end

Len = cellfun(@length, coverage, 'UniformOutput', false);
finalLength=min([Len{:}]);

for rr=1:NRuns
    coverage{rr}=coverage{rr}(1:finalLength);
    coverage{rr}(1:5)=[];
    for pp=1:Nposes
        poses.position{rr,pp}=poses.position{rr,pp}(1:finalLength,:); % clean up the end
        poses.position{rr,pp}(1:5,:)=[];                     % clean up the begining
        
        poses.orientation{rr,pp}=poses.orientation{rr,pp}(1:finalLength,:); % clean up the end
        poses.orientation{rr,pp}(1:5,:)=[];                     % clean up the begining
        
        %avoidState{1,rr,pp}=avoidState{1,rr,pp}(1:finalLength,:);
        %avoidState{1,rr,pp}(1:5,:)=[];
    end
end
file=files(ff).name;
filename=strcat(file(1:length(file)-4),'.mat');
save(filename,'coverage','poses','Time','limits');

data.coverage=coverage;
%data.avoidState=avoidState;
data.poses=poses;
data.Time=Time;
data.limits=limits;

plotPersistCoverage(data);
end


function [Xplace, Yplace, xpos, ypos, qx, qy, qz, qw]=getXYQ(msg,n)
global xGrid yGrid

        tmpX=xGrid;
        tmpX=[xGrid,msg.Poses(n).Position.X];
        tmpX=sort(tmpX);
        Xplace = find(tmpX==msg.Poses(n).Position.X)-1;
        
        tmpY=yGrid;
        tmpY=[yGrid,msg.Poses(n).Position.Y];
        tmpY=sort(tmpY);
        Yplace = find(tmpY==msg.Poses(n).Position.Y)-1;
        
        xpos=msg.Poses(n).Position.X;
        ypos=msg.Poses(n).Position.Y;
        
        qx=msg.Poses(n).Orientation.X;
        qy=msg.Poses(n).Orientation.Y;
        qz=msg.Poses(n).Orientation.Z;
        qw=msg.Poses(n).Orientation.W;
        
        
end
