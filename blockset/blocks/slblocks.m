%%***************************************************************************************
%% file         slblocks.m
%% brief        sets up the simulink library.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
%%
%%---------------------------------------------------------------------------------------
%%                            L I C E N S E
%%---------------------------------------------------------------------------------------
%% Permission is hereby granted, free of charge, to any person obtaining a copy of this
%% software and associated documentation files (the "Software"), to deal in the Software
%% without restriction, including without limitation the rights to use, copy, modify, merge,
%% publish, distribute, sublicense, and/or sell copies of the Software, and to permit
%% persons to whom the Software is furnished to do so, subject to the following conditions:
%%
%% The above copyright notice and this permission notice shall be included in all copies or
%% substantial portions of the Software.
%%
%% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
%% INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
%% PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
%% FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
%% OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
%% DEALINGS IN THE SOFTWARE.
%% endinternal
%%
%%***************************************************************************************

function blkStruct = slblocks
%SLBLOCKS Defines the block library for a specific Toolbox or Blockset.

% Name of the subsystem which will show up in the SIMULINK Blocksets
% and Toolboxes subsystem.
% Example:  blkStruct.Name = 'DSP Blockset';
blkStruct.Name = 'GOcontroll Linux Target';

% The function that will be called when the user double-clicks on
% this icon.
% Example:  blkStruct.OpenFcn = 'dsplib';
blkStruct.OpenFcn = 'GOcontroll_Moduline_Target';

% The argument to be set as the Mask Display for the subsystem.  You
% may comment this line out if no specific mask is desired.
% Example:  blkStruct.MaskDisplay = 'plot([0:2*pi],sin([0:2*pi]));';
% blkStruct.MaskDisplay = 'disp(''......'')';

% Define the library list for the Simulink Library browser.
% Return the name of the library model and the name for it
Browser(1).Library = 'GOcontroll_Moduline_Target';
Browser(1).Name    = 'GOcontroll Moduline';
Browser(1).IsFlat  = 0;

Browser(2).Library = 'GOcontroll_Generic';
Browser(2).Name    = 'GOcontroll Generic';
Browser(2).IsFlat  = 0;

% find every folder that matches the blockset_* format and execute the
% slblocks.m script located in this folder
% If a user wishes to add their own blockset elements see the GOcontroll
% Wiki on how to get started (link to be added).
d = dir("blockset_*");
folders = {d.name};
BrowserIndex = 3;
for i = 1:length(folders)
    name=char(folders(1,i));
	blocksetScript = [pwd filesep name filesep 'setupBlocks.m'];
    if isfile(blocksetScript)
        run(blocksetScript);
    else
    	warndlg(sprintf('No setupBlocks script found for %s', name),'Warning');
    end

end

blkStruct.Browser = Browser;
