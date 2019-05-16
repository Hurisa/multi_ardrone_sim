clear; clc;
d = uigetdir(pwd, 'Select a folder');
files = dir(fullfile(d, '*.bag'));

Nf=size(files,1);

msgCell=cell(1,Nf);
for ii=1:Nf
    ii
    bag=rosbag(strcat(d,'/',files(ii).name));
    msg=select(bag,'Topic',strcat('/swarmState'));
    %msgCell{ii}=readMessages(msg,'DataFormat','struct');
    msgCell=readMessages(msg,'DataFormat','struct');
    for s=1:size(msgCell,1)
        msgCell{s}=rmfield(msgCell{s},'MessageType');
    end
    filename=strcat(files(ii).name,'.mat');
    save(filename, 'msgCell')%, '-v7.3')
end


% sizes=(cellfun(@(v)size(v,1),msgCell,'UniformOutput', false))';
% totalmsgs=sum([sizes{:}]);
% 
% msgStruct=cell(totalmsgs,1);
% 
% gaps=[0,cumsum([sizes{:}])];
% 
% for jj=1:Nf
%    
%    msgStruct(gaps(jj)+1:gaps(jj+1),1)=msgCell{jj};
%     
% end