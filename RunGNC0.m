function RunGNC0(dmcFN,pgmstem)
addpath('~/HST/code/cv-hst') %for rawDMCreader on my PC

if nargin<1, dmcFN = '~/DMC/2012-12-20T22-48-34_frames_317600-1-339300.DMCdata'; end
if nargin<2, pgmstem='data/rawvid'; end

gncpath = '~/RobustFlow/src/';
%% load raw data
data=rawDMCreader(dmcFN,512,512,4,4,1:2); %'all');
%% convert raw data to PGM
[nx,ny,FrameInd] = makeIMG(data,pgmstem);
%% run gnc black robust flow code
runBlack(pgmstem,FrameInd,gncpath,nx,ny)
end 
