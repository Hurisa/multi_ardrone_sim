d = uigetdir(pwd, 'Select a folder');
files = dir(fullfile(strcat(d,'/','*.mat')));

numFiles=size(files,1);

for s=1:numFiles
    
    current=load(files(s).name);
    data=current.data;
    filename=files(s).name;
    
    plotPersistCoverage(data,filename);
    
end