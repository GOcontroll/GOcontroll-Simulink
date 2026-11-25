%%***************************************************************************************
%% file         goc_deleteBuildFiles.m
%% brief        Timer callback that removes generated build artefacts after a build.
%%              Called via a one-shot timer from ert_linux_make_rtw_hook so that
%%              Simulink's own onCleanup can cd out of the build folder first.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************
function goc_deleteBuildFiles(t, slprjPath, genCodePath, slxcFile)
    stop(t);
    delete(t);
    % The slprj folder is shared by every model in the project folder. While another
    % build is running (two models built right after each other) removing it would
    % break that build, so try again a little later. A build that started more than
    % half an hour ago is taken to be one that was aborted without telling.
    if isappdata(0, 'GOcontrollBuildRunning')
        started = getappdata(0, 'GOcontrollBuildRunning');
        if (now - started) * 24 * 60 < 30 %#ok<TNOW1>
            again = timer('ExecutionMode', 'singleShot', 'StartDelay', 3, ...
                'TimerFcn', @(src, ~) goc_deleteBuildFiles(src, slprjPath, genCodePath, slxcFile));
            start(again);
            return
        end
    end
    try
        if exist(slprjPath,   'dir'),  rmdir(slprjPath,   's'); end
    catch err
        warning('### Failed to delete slprj: %s', err.message);
    end
    try
        if exist(genCodePath, 'dir'),  rmdir(genCodePath, 's'); end
    catch err
        warning('### Failed to delete generated_code: %s', err.message);
    end
    try
        if exist(slxcFile,    'file'), delete(slxcFile);        end
    catch err
        warning('### Failed to delete .slxc: %s', err.message);
    end
    disp('### Build files deleted');
end
