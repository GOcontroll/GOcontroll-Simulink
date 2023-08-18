%%***************************************************************************************
%% file         librarySetup.m
%% brief        Initialization that is automatically executed when a Simulink model in 
%%              the same directory is opened.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2019 (c) by HAN Automotive     http://www.han.nl          All rights reserved
%%  Copyrigth 2023 (c) by GOcontroll B.V.    http://www.gocontroll.com  All rights reserved
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
%%
%% This file has been altered by GOcontroll to also support Linux based host machines and 
%% to support adding new blockset folders.
%%
%%***************************************************************************************

% First restore the path to factory default
restoredefaultpath;
clear RESTOREDEFAULTPATH_EXECUTED

% add Linux Target blockset related directories to the MATLAB path
OS = computer();
if OS=="GLNXA64" % if the host is Linux
    addpath(sprintf('%s/blockset', pwd));
    addpath(sprintf('%s/blockset/blocks',pwd));
    addpath(sprintf('%s/blockset/code',pwd)); 
    path1 = getenv('LD_LIBRARY_PATH');
    path = ['/lib/x86_64-linux-gnu' ':' path1];
    setenv('LD_LIBRARY_PATH',path);
else % if the host is Windows
    addpath(sprintf('%s\\blockset', pwd));
    addpath(sprintf('%s\\blockset\\blocks',pwd));
    addpath(sprintf('%s\\blockset\\code',pwd)); 
end

% find every folder that matches the blockset_* format and execute the
% librarySetup.m script located in this folder
% If a user wishes to add their own blockset elements see the GOcontroll
% Wiki on how to get started (link to be added).
d = dir("blockset_*");
folders = {d.name};
for i = 1:length(folders)
    name=folders(1,i);
    if OS=="GLNXA64"
        setupScript = sprintf("%s/%s/librarySetup.m",pwd,char(name));
    else
        setupScript = sprintf("%s\\%s\\librarySetup.m",pwd,char(name));
    end
    if isfile(setupScript)
    	run(setupScript);
    else
    	warndlg(sprintf('No library setup script found for %s', char(name)),'Warning');
    end
end

clear setupScript
clear i
clear d
clear folders
clear name
clear path
clear path1

warning off Simulink:SL_LoadMdlParameterizedLink;
warning off Simulink:Commands:LoadMdlParameterizedLink;

%%******************************* end of librarySetup.m **************************************

