d = uigetdir(pwd, 'Select a folder');
files = dir(fullfile(strcat(d,'/','*.mat')));

numFiles=size(files,1);

for s=1:numFiles
    s
    data=load(strcat(d,'/',files(s).name));
    filename=files(s).name;
    
    plotPersistCoverage(data,filename);
    
end