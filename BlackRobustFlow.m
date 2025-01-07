%% BLACKROBUSTFLOW
% Runs Michael Black GNC dense robust optical flow estimation
%
% Inputs
% ------
% pgmstem: stem (path/filename) to .pgm image date
% frameind: which frame numbers to analyze (.pgm files to load, part of stem filename)
%
% Outputs
% -------
% u, v: estimated optical flow U V
function [u,v] = BlackRobustFlow(pgmstem,frameind)
arguments
  pgmstem (1,1) string
  frameind (1,:) double = 0:1
end

cmd = "build/gnc";
if ispc
  cmd = cmd + ".exe";
end
assert(isfile(cmd), "compile the GNC program")

rowcol= size(imread(pgmstem + string(frameind(1)) + ".pgm"));
[~,stem] = fileparts(pgmstem);


for ii = frameind(1):frameind(end)-1
        fnOut = "results/" + stem + string(ii+1) + "-";

        fullcmd = join([cmd, string(ii), string(ii+1), "4 1", ...
        pgmstem,fnOut, ...
        "-l1 10.0 -l2 1.0 -S1 10.0 -S2 1.0 -s1 10.0 -s2 0.05", ...
        "-stages 5",...
        "-nx", string(rowcol(2)), "-ny", string(rowcol(1)), ...
        "-f 0.5 -F 0 -w 1.995 -iters 20", ...
        "-end .pgm -skip 15"]);

        disp(fullcmd)
        [err, msg] = system(fullcmd);  %15 is # of header bytes for PGM
    switch err
      case {127,1}
        disp(fullcmd)
        disp(msg)
        disp(err)
        error('compile "gnc" executable')
       case 0 %normal
         disp("frame " + string(ii))
       otherwise
        disp(fullcmd)
        disp(msg)
        disp(err)
        break
    end

end


bzero = 128;  % NOTE: it appears that for the unsigned 8-bit output, 128 is zero, and values less than 128 are negative
ufn = fnOut + "u-4.pgm";
vfn = fnOut + "v-4.pgm";
disp("loading  " + ufn + " " + vfn)
% upcast by one size (int16) to account for initial uint8
% values that would overflow int8. RAM is cheap.
u = int16(imread(ufn)) - bzero;
v = int16(imread(vfn)) - bzero;

if all(u(:)==u(1,1)), warning("all elements of U are identical: " + num2str(u(1,1)) + " likely bad result"), end
if all(v(:)==v(1,1)), warning("all elements of V are identical: " + num2str(v(1,1)) + " likely bad result"), end

%% plot
downsamp = 10; %arbitrary

figure(1)
imagesc(imread(pgmstem + string(frameind(1)) + ".pgm"))
colormap gray
hold on
[x,y] = meshgrid(1:downsamp:rowcol(2),1:downsamp:rowcol(1));
quiver(x, y, u(1:downsamp:end,1:downsamp:end), v(1:downsamp:end,1:downsamp:end))
title('Robust Optical flow vectors, Black method')
axis('off')


if ~nargout, clear u v, end
end
