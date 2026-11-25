%%***************************************************************************************
%% file         startup.m
%% brief        Run by MATLAB at start-up when this folder is the start-up folder.
%%
%%              Simulink resolves a model's system target file BEFORE that model's
%%              PreLoadFcn runs, so librarySetup is too late to stop another installed
%%              GOcontroll blockset (an Add-On toolbox, a second clone) from shadowing
%%              this repository's ert_gocontroll.tlc. The Code Generation pane is then
%%              built from the wrong file and Target, Use precompiled binary and Delete
%%              build files are missing from GOcontroll options.
%%
%%              Running librarySetup here puts this repository on the MATLAB path before
%%              any model is opened, so this repository's target file always wins.
%%
%%              This is the belt; go_repair_target_options, called from the model's
%%              PostLoadFcn, is the braces for when MATLAB was started somewhere else.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

try
    run(fullfile(fileparts(mfilename('fullpath')), 'librarySetup.m'));
catch startupError
    % Never let a failure here block MATLAB from starting - the models repair
    % themselves on load.
    warning('GOcontroll:startupFailed', ...
            'librarySetup could not be run from startup.m: %s', startupError.message);
end
clear startupError

%%******************************** end of startup.m *************************************
