%% BLACKRAWUVREADER read .flt data and .pgm data and compare
% TODO: appears to be loading the .flt data incorrectly--maybe wrong nx,
% ny?
function [ufl,vfl] = blackrawuvreader(dpath, uflt, vflt, upgm,vpgm)
arguments
  dpath (1,1) string
  uflt (1,1) string
  vflt (1,1) string
  upgm (1,1) string
  vpgm (1,1) string
end

nx = 200;
ny = 200;
%a priori from looking at original image size

%% Read .flt data
ufn = fullfile(dpath, uflt);
vfn = fullfile(dpath, vflt);

fid = fopen(ufn,'r');
ufl = fread(fid, [ny,nx], 'single',0,'l');
fclose(fid);

fid = fopen(vfn,'r');
vfl = fread(fid, [ny,nx], 'single',0,'l');
fclose(fid);

%% Read .pgm data
ufn = fullfile(dpath, upgm);
vfn = fullfile(dpath, vpgm);

ud8 = transpose(imread(ufn));
vd8 = transpose(imread(vfn));

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

if ~nargout, clear ufl uvl, end
end
