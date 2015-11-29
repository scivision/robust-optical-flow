function RunGNC0(dmcFN,pgmstem)
addpath('~/code/histutils/Matlab') %for rawDMCreader.m
gncpath = '';
%% load raw data
data=rawDMCreader(dmcFN,'framereq',1:2);
%% convert raw data to PGM
[nx,ny,FrameInd] = makeIMG(data,pgmstem);
%% run gnc black robust flow code
runBlack(pgmstem,FrameInd,gncpath,nx,ny)
end 
