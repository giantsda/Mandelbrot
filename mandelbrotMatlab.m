% this program will zoom into a mandelbrot at a random(not preset) point
% if the zoom point is detected as a boring point, it will find the nearest interesting point based on the gradient field
% x is honrizontal; y is vetical;
% Chen Shen 2/21/2019
% chenshen@rams.colostate.edu

close all;
figure;
set(gcf,'outerposition',get(0,'screensize')*1);
xPixel=720;
yPixel=600;
maxIter=200;
iterMap=zeros(yPixel,xPixel);
zm=1;
xmin=  -2.1175;
xmax= 1.6874;
ymin= -0.7469;
ymax= 2.1068;

PICK=0;
FIRST=1;
frame=1;
screenExamLength=floor(yPixel/100);
dx=1;

x0=xmin+rand(1)*(xmax-xmin);
y0=ymin+rand(1)*(ymax-ymin);

% x0=-0.658266142810369
% y0=0.437622059571268
clear F;
while dx>1e-14   % when dx is larger than the machine error
    dx
    if PICK
        [maxvalues, ind] = maxk(gradientMag(:), 100); % find n largest
        [I,J]=ind2sub(size(gradientMag),ind); % get row and column number
        pickS=[I J];
        
        for i=1:length (pickS)
            xp=x1+dx*(J(i)-1);
            yp=y1+dy*(I(i)-1);
            pickS(i,3)=sqrt((xp-x0)^2+(yp-y0)^2);% distance
        end
        [~, pickI]=min(pickS(:,3)); % find the one has the smallest distance ;
        x0=x1+dx*(J(pickI)-1);
        y0=y1+dy*(I(pickI)-1);
        
        if FIRST
            hold on;
            plot(x0,y0,'R*','MarkerSize',100);
            hold off;
        end
        title("New zoom point");
        %         pause();
        PICK=0;
    end
    
    x1=x0-2*exp(-zm/20);
    x2=x0+2*exp(-zm/20);
    y1=y0-1.5*exp(-zm/20);
    y2=y0+1.5*exp(-zm/20);
    x=linspace(x1,x2,xPixel);
    y=linspace(y1,y2,yPixel);
    dx = (x2 - x1) / (xPixel-1);
    dy = (y2 - y1) / (yPixel-1);
    for j  =1:yPixel
        for i  =1:xPixel
            cR = (i-1) * dx + x1;%c=X+i*Y;
            cI = (j-1) * dy + y1;
            R=0;
            I=0;
            iteration=0;
            while R*R+I*I<=4 && iteration < maxIter     %z=z.^2+c;
                Rtemp=R*R-I*I+cR;
                I=2*R*I+cI;
                R=Rtemp;
                iteration=iteration+1;
            end
            iterMap(j,i)=iteration;
        end
    end
    
    %% the point is interesting or boring based on the gradient filed
    [FX,FY] = gradient(iterMap);
    gradientMag=abs(log(FX.*FX+FY.*FY));
    gradientMag(gradientMag==Inf)=0;
    maxGradientMag=max(max(gradientMag));
    meanGradientMag=mean(mean(gradientMag));
    
    examedWindowMeangradientMag=mean(mean(gradientMag(yPixel/2-screenExamLength:yPixel/2+screenExamLength,xPixel/2-screenExamLength:xPixel/2+screenExamLength)));
    if examedWindowMeangradientMag<=meanGradientMag*0.5 
        PICK=1;
    end
    
    imagesc(x,y,(iterMap));
    %     title([num2str(examedWindowMeangradientMag) ...
    %         '  ' num2str(meanGradientMag*0.1) ' dx=' num2str(dx)] );
    
    if FIRST
        dx0=dx;
        FIRST=0;
    end
    
    title(['zoom: X ' num2str(dx0/dx)]);
%     if dx<=1e-12;   % my little interesting point selection code for MCU systems
%         fprintf("x0=%2.15f\ny0=%2.15f\n",x0,y0);
%         close all;
%         error('haha');
%         pause();
%     end
    
    %     hold on;          % plot zoom point
    %     plot(x0,y0,'w*','MarkerSize',30);
    %     hold off;
    
    %     colormap summer;
    %     colormap parula
    colormap jet;
    
    colorbar;
    axis equal;
    axis tight
    F(frame)=getframe(gcf); %% store frame 
    frame=frame+1;
    zm=zm+5;
    pause(0);
end

%% write image to a video
Frame=[Frame(1:end) Frame(end:-1:1)];
outputVideo = VideoWriter('haha.avi');
outputVideo.FrameRate =8;
open(outputVideo);
for i=1:length(Frame)
    i/length(Frame)*100
    writeVideo(outputVideo,Frame(i).cdata);
end
close(outputVideo)

 