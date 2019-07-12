close all
figure
hold on
box on
grid minor
set(gcf,'Position',[100 100 700 600])
axis([-10 10 -10 10])

xlabel('X[m]','FontSize',16)
ylabel('Y[m]','FontSize',16)
%set(gca,'FontSize',12)


%k1=rectangle('Position',[-3 0 2 1.5],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none');
h(1)=fill([-3 -.5 -.5 -3],[0 0 1.5 1.5],[255/255 230/255 203/255],'EdgeColor','none');

rectangle('Position',[-3 -1 0.5 1],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none')
rectangle('Position',[-1 1 0.5 0.5],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none')
rectangle('Position',[0 -1.5 1.5 2.5],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none')
rectangle('Position',[-.5 0 .5 1],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none')
rectangle('Position',[-2.5 -2.5 1.5 1.5],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none')
rectangle('Position',[-1 -2.5 1 1],'FaceColor',[255/255 230/255 203/255],'EdgeColor','none')

h(2)=fill([-2.5 -1 -1 -2.5],[-1 -1 0 0],[255/255 181/255 105/255],'EdgeColor','none');

%k2=rectangle('Position',[-2.5 -1 1.5 1],'FaceColor',[255/255 181/255 105/255],'EdgeColor','none');
rectangle('Position',[-1 0 .5 1],'FaceColor',[255/255 181/255 105/255],'EdgeColor','none')
rectangle('Position',[-0.5 -1.5 .5 1.5],'FaceColor',[255/255 181/255 105/255],'EdgeColor','none')
rectangle('Position',[-1 -1.5 .5 .5],'FaceColor',[255/255 181/255 105/255],'EdgeColor','none')

%k3=rectangle('Position',[-1 -1 0.5 1],'FaceColor',[255/255 129/255 0/255],'EdgeColor','none');
h(3)=fill([-1 -0.5 -0.5 -1],[-1 -1 0 0],[255/255 129/255 0/255],'EdgeColor','none');

rectangle('Position',[-3 -1 2.5 2.5]);
rectangle('Position',[-1 -1.5 2.5 2.5])
rectangle('Position',[-2.5 -2.5 2.5 2.5])

h(4) = line(NaN,NaN,'Color',[0 0 0]);
legend(h(1:4),{'1-coverage','2-coverage','3-coverage','UAV sensing area'},'FontSize',14)


%legend(k1,'This three')