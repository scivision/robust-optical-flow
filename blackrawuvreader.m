function [ufl,vfl] = blackrawuvreader(dpath,ufn,vfn,upgm,vpgm)
% Michael Hirsch
% tested with Matlab and Octave 3.8

nx = 200; ny= 200; %a priori from looking at original image size

%%
fid = fopen([dpath,ufn],'r');
ufl = fread(fid,[ny,nx],'single',0,'l');
fclose(fid);

fid = fopen([dpath,vfn],'r');
vfl = fread(fid,[ny,nx],'single',0,'l');
fclose(fid);

%%
ud8 = transpose(imread([dpath,upgm]));
vd8 = transpose(imread([dpath,vpgm]));

%%
figure(1), clf(1)
subplot(2,2,1)
imagesc(ufl)
colorbar()
title('u from .flt file')

subplot(2,2,3)
imagesc(ud8)
colorbar
title('u from .pgm file (after transpose(u) )')

subplot(2,2,2)
imagesc(vfl)
colorbar()
title('v from .flt file')

subplot(2,2,4)
imagesc(vd8)
colorbar
title('v from .pgm file (after transpose(v) )')

if ~nargout, clear, end
end
