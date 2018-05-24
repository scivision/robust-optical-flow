function [nx,ny,FrameInd] = makeIMG(MAT,pgmstem)
[ny, nx, nframes] = size(MAT);

clim = [1000 1500]; %arbitrary guess
FrameInd = 1:nframes;

disp(['converting ',int2str(length(FrameInd)),' frames to PGM'])

for i = FrameInd
    frame = normFrame(MAT(:,:,i),16,clim); 
    imwrite(frame,[pgmstem,int2str(i),'.pgm'],'pgm')
end
end
