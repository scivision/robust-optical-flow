% Michael Hirsch
% a simple way to slide a texture through the field of view

clear

%% user parameters
dpath = '~/phantoms/';
imgname = 'soft8gray.png';
saveplot = true;


xp = 1:512; %arbitrary choice of which x-pixels to take from phantom
yp = 1:512; %arbitrary choice of which x-pixels to take from phantom
nxp = length(xp);
nyp = length(yp);

nxc = 256; % # of x-pixel to view 
nyc = 256; % # of y-pixel to view 

nframe = 256;


if exist('OCTAVE_VERSION','builtin')
	oldway = true; % false: slower method for pre-R2013a.  true: faster method for R2013a onward
elseif verLessThan('matlab','8.1') %octave 3.8 doesn't have verLessThan yet 
	oldway = true;
else
	oldway = false;
end

dx = 0.; %amount to slide each step, positive is RIGHT
dy = 0.5; %positive is DOWN
%% main program
imgfn = [dpath,imgname];

img = imread(imgfn);
img = img(1:nyp,1:nxp); %arbitrary start/stop locations for clipping

figure(1),clf(1)
ax = axes('parent',1);
hi = imagesc(nan(nyc,nxc));  %initialize image
set(ax,'ydir','normal')
colormap(gray)
xlabel('x'), ylabel('y')
title(imgname)


for ifrm = 1:nframe
    T =   [1,0,0
           0,1,0
           -nxp/4+ifrm*dx, -nyp/4+ifrm*dy, 1];
    
    if ~oldway %new,fast
        tform = affine2d(T);
        RA = imref2d([nxp,nyp],[1,nxc],[1, nyc]);%,1,1);   %I want a clipped view in x, so I don't see the fillvalue
        [cimg,RB] = imwarp(img,tform,'outputView',RA,'interp','linear');
    else %old, slow
    
        tform = maketform('affine',T);    %#ok<MTFA1>
                
        cimg = imtransform(img,tform,'bilinear',...
                     'Udata',[1 nxp],...
                     'Vdata',[1 nyp],...
                     'Xdata',[1 nxc],...
                     'Ydata',[1 nyc],...
                     'fillvalues',0,...
                     'size',[nyp,nxp]); %#ok<DIMTRNS>
                 
    end %if
    
    set(hi,'cdata',cimg) %fast update of image display
    title(ax,[imgname,'i=',int2str(ifrm)])
    pause(0.01)

     if saveplot
		imwrite(cimg,['/tmp/imgslide_f',num2str(ifrm,'%03d'),'.pgm'])
     end
end
