function go_platform_lock_check(mdl)
%%***************************************************************************************
%% file         go_platform_lock_check.m
%% brief        Errors when the Target of a model does not belong to the platform the
%%              model is locked to (tlcGOcontrollPlatformLock: '', 'Linux' or 'STM').
%%
%%              The model templates set the lock, so a model made for the Linux
%%              controllers cannot be built for the Moduline S1 by accident and the
%%              other way round. A model without the lock is not checked.
%%
%% param        mdl     model name
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    cs = getActiveConfigSet(mdl);
    try
        lock = get_param(cs, 'tlcGOcontrollPlatformLock');
    catch
        return          % option not defined: model from an older blockset
    end
    if isempty(lock)
        return
    end
    target = get_param(cs, 'tlcGOcontrollTarget');
    if ~strcmp(go_target_platform(target), lock)
        error('go_platform_lock_check:wrongPlatform', '%s', go_platform_lock_message(lock, target));
    end
end
