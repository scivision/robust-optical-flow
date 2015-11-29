function RunBlack(pgmstem,varargin)
%%
% Runs Michael Black GNC dense robust optical flow estimation
% from a convenient Matlab interface.
%
% Inputs
% ------
% pgmstem: stem (path/filename) to .pgm image date
% frameind: which frame numbers to analyze (.pgm files to load, part of stem filename)
% gncpath: path to gnc executable

p = inputParser;
addParamValue(p,'frameind',1:2) % arbitrary
addParamValue(p,'gncpath','src/') %#ok<*NVREPL>
parse(p,varargin{:})
U = p.Results;

rowcol= size(imread([pgmstem,int2str(U.frameind(1)),'.pgm']));
[~,stem] = fileparts(pgmstem);

for ii = U.frameind(1):U.frameind(end)-1
        fnOut = ['results/',stem,int2str(ii+1),'-'];
        
        [err,msg]=unix([U.gncpath,'gnc ',...
        int2str(ii),' ', int2str(ii+1),...
        ' 4 1 ',...
        pgmstem,' ',fnOut,...
        ' -l1 10.0 -l2 1.0 -S1 10.0 -S2 1.0 -s1 10.0 -s2 0.05 ',...
        ' -stages 5',...
        ' -nx ',int2str(rowcol(2)),...
        ' -ny ',int2str(rowcol(1)),...
        ' -f 0.5 -F 0 -w 1.995 -iters 20 ',...
        ' -end .pgm -skip 15']);  %15 is # of header bytes for PGM
    if err==127
        disp(msg)
        error('you must one-time compile gnc by typing in Terminal:   cd src; make')
    elseif err==5 %normal?
    else
        disp(msg)
        break
    end
    if ~isoctave
        disp(msg)
    end
    
end


bzero = 128;  % NOTE: it appears that for the unsigned 8-bit output, 128 is zero, and values less than 128 are negative
ufn = [fnOut,'u-4.pgm'];
vfn = [fnOut,'v-4.pgm'];
display(['loading ',ufn,' for u flow'])
u = int32(imread(ufn)) - bzero;
v = int32(imread(vfm)) - bzero;


%% plot
downsamp = 4; %arbitrary

[x,y] = meshgrid(1:downsamp:nx,1:downsamp:ny);

figure(10),clf(10)

imagesc(imread([pgmstem,'1.pgm']))
colormap gray
hold on
quiver(x,y,...
       u(1:downsamp:end,1:downsamp:end),v(1:downsamp:end,1:downsamp:end))


end %function
