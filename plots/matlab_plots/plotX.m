function plotX(n,data)

%[file, path] = uigetfile({'*.*'},'Select .mat file');
%data=load(strcat(path,file));
for ii=1:size(data.poses,1)
figure
hold on
axis([0, size(data.poses{1},1), -11,11])
plot(data.poses{ii,n}(:,1))
hold off
end

end