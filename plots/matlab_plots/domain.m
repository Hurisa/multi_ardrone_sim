close all
hold on
axis([0 10 0 10])
box on


x=0:0.5:9.5;
for i=1:size(x,2)
    for j=1:size(x,2)
        rectangle('Position',[x(i),x(j),0.5,0.5],'EdgeColor',[192,192,192]./255, 'Linewidth',0.1);
    end
end
rectangle('Position',[0,0,10,10],'Linewidth',1.5)
xlabel('X [m]');
ylabel('Y [m]');
% X = [0:10];
% Y = zeros(size(X))-1;
% U = zeros(size(X));
% V = ones(size(Y));
% quiver(X,Y,U,V,1/2,'b')
% 
% X = [0:10];
% Y = 10*ones(size(X))+1;
% U = zeros(size(X));
% V = -ones(size(Y));
% quiver(X,Y,U,V,1/2,'b')
% 
% 
% Y = [0:10];
% X = (10+zeros(size(Y)))+1;
% U = -ones(size(X));
% V = zeros(size(Y));
% quiver(X,Y,U,V,1/2,'b')
% 
% Y = [0:10];
% X = zeros(size(X))-1;
% U = ones(size(X));
% V = zeros(size(Y));
% quiver(X,Y,U,V,1/2,'b')