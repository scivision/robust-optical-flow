function runBlack(pgmstem,FrameInd,gncpath,nx,ny)

isoctave = logical(exist('OCTAVE_VERSION', 'builtin'));

for ii = FrameInd(1):FrameInd(end)-1
        fnOut = ['results/rawvid',int2str(ii+1),'-'];
        
        [~,msg]=unix([gncpath,'gnc ',...
        int2str(ii),' ', int2str(ii+1),...
        ' 4 1 ',...
        pgmstem,' ',fnOut,...
        ' -l1 10.0 -l2 1.0 -S1 10.0 -S2 1.0 -s1 10.0 -s2 0.05 ',...
        ' -stages 5',...
        ' -nx ',int2str(nx),...
        ' -ny ',int2str(ny),...
        ' -f 0.5 -F 0 -w 1.995 -iters 20 ',...
        ' -end .pgm -skip 15']);  %15 is # of header bytes for PGM
    if ~isoctave
        display(msg)
    end
    
end


bzero = 128;  % TODO FIXME this is a total guess!
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
