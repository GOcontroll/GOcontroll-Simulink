%%***************************************************************************************
%% file         librarySetup.m
%% brief        Initialization that is automatically executed when a Simulink model in 
%%              the same directory is opened.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2019 (c) by HAN Automotive     http://www.han.nl     All rights reserved
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
%%
%%***************************************************************************************

% add GOcontroll blockset related directories to the MATLAB path
repoRoot = fileparts(mfilename('fullpath'));

% Add the blockset to the MATLAB path. startup.m already does this at MATLAB
% start-up, before Simulink can resolve a system target file - in that case there is
% nothing left to add here, but the libraries below must still be loaded.
needPath = ~contains(path, fullfile(repoRoot, 'blockset', 'ert'));

OS = computer();
if needPath && OS=="GLNXA64"
    addpath(sprintf('%s/blockset/assets', repoRoot));
    addpath(sprintf('%s/blockset/blocks', repoRoot));
    addpath(sprintf('%s/blockset/blocks/software', repoRoot));
    addpath(sprintf('%s/blockset/blocks/software/tlc_c', repoRoot));
    addpath(sprintf('%s/blockset/blocks/board', repoRoot));
    addpath(sprintf('%s/blockset/blocks/board/tlc_c', repoRoot));
    addpath(sprintf('%s/blockset/blocks/can', repoRoot));
    addpath(sprintf('%s/blockset/blocks/can/tlc_c', repoRoot));
    addpath(sprintf('%s/blockset/blocks/memory', repoRoot));
    addpath(sprintf('%s/blockset/blocks/memory/tlc_c', repoRoot));
    addpath(sprintf('%s/blockset/blocks/modules', repoRoot));
    addpath(sprintf('%s/blockset/blocks/modules/tlc_c', repoRoot));
    addpath(sprintf('%s/blockset/blocks/network', repoRoot));
    addpath(sprintf('%s/blockset/blocks/network/tlc_c', repoRoot));
    addpath(sprintf('%s/blockset/GOcontroll-CodeBase/code', repoRoot));
    addpath(sprintf('%s/blockset/ert', repoRoot));
    addpath(sprintf('%s/blockset/GOcontroll-CodeBase/lib', repoRoot));
    addpath(sprintf('%s/blockset/utility_functions', repoRoot));
    %path1 = getenv('LD_LIBRARY_PATH');
    %path = ['/lib/x86_64-linux-gnu' ':' path1];
    %setenv('LD_LIBRARY_PATH',path);
elseif needPath
    addpath(genpath(fullfile(repoRoot,'blockset/')),'-begin');
end

% The folder of this file goes on the path too, at the end: a model calls librarySetup
% when it loads, and it has to find it when the model is not in this folder - a project
% that carries the blockset as a git submodule keeps its models one folder up.
if needPath
    addpath(repoRoot, '-end');
end

% Warn when MATLAB also reaches another copy of the blockset - an Add-On, another clone on
% the path, a library loaded from it: Simulink could take the target file and the blocks
% from that copy. Before the libraries are loaded, so a library from that copy is seen.
% Only a warning: the check never stops a model from opening.
try
    go_other_blocksets(repoRoot);
catch
end

% Explicitly load libraries so Simulink can resolve all library links
if ~bdIsLoaded('GOcontroll_Moduline_Target')
    load_system('GOcontroll_Moduline_Target');
end
if ~bdIsLoaded('GOcontroll_Generic')
    load_system('GOcontroll_Generic');
end

%% load .mat file for this model
%load(strcat(bdroot,'.mat'));

%checkGnuArmToolchain('suppressOutput');

clear path repoRoot OS needPath

warning off Simulink:SL_LoadMdlParameterizedLink;
warning off Simulink:Commands:LoadMdlParameterizedLink;

%%******************************* end of librarySetup.m **************************************
