%%***************************************************************************************
%% file         ert_gocontroll_make_rtw_hook.m
%% brief        Hook file entry point for the ert_gocontroll system target file.
%%              MATLAB resolves the hook by convention: <stf_stem>_make_rtw_hook.m.
%%              All build logic lives in ert_linux_make_rtw_hook.m.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************
function ert_gocontroll_make_rtw_hook(hookMethod, modelName, rtwroot, templateMakefile, buildOpts, buildArgs, buildInfo)
    ert_linux_make_rtw_hook(hookMethod, modelName, rtwroot, templateMakefile, buildOpts, buildArgs, buildInfo);
end
