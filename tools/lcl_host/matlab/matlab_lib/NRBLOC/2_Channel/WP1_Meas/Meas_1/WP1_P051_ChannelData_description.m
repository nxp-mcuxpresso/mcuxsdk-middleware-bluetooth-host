%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Jac Romme (jac.romme@imec-nl.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%
% This file only describes the content of the WP1_P051_ChannelData.mat
% and relates the 2.4GHz radio channel measurements conducted in P054 at
% Imec-nl/Holst-center in 2016. A description of the measurements can be
% found in Technical Note TN-16-WATS-TP2-196, "“Accurate localization & 
%               distance measurement capabilities of NB ISM band radios”
%
% The file WP1_P051_ChannelData.mat has the following content:
%   BSID_all              2200x1                 17600  double              
%   Date_all              1x2200                 17600  double              
%   Number_all            1x2200                 17600  double              
%   PosID_all             1x2200                 17600  double              
%   S_Data                1x2200            1153680064  struct              
%   SegmentID_all         1x2200                255200  cell                
%   freq_meas             1x4096                 32768  double              
%   labels                1x8                       64  double   
%
% The file contains 2200 measurements 4-port VNA measurements
% The data of each measurement can be found in in the structure S_Data,
% i.e. S_Data(MeasID).H is a matrix of 8x4096. 8 is the number of relevant
% VNA-port/antenna combinations. The VNA port ID can be found in 
% labels=[13 14 23 24 31 32 41 42], which can be translated to orientation
% with label2orientation.m. The 4096 refers to the number of meas-frequency, 
% see variable freq_meas.
% The basestation/anchor of each measurement is labeled in BSID and can be 
% translated to a 3D position using GetBSPos.m
% The SegmentID_all denotes the segment of the mobile-side and can be 
% translated to a 3D position together with the PosID_all using GetPosition.m
% Note that the 2200 are done at 440 unique BS, position combination and
% there repeated 5 times, shortly after each other, each labeled with an
% unique number between 1 and 5, see Number_all.
%
% Description end