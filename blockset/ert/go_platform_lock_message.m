function msg = go_platform_lock_message(lock, target)
%%***************************************************************************************
%% file         go_platform_lock_message.m
%% brief        Text shown when a Target is refused because the model is locked to the
%%              other platform. Used by the Target callback and go_platform_lock_check.
%%
%% param        lock    'Linux' or 'STM'
%% param        target  the refused Target
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    if strcmp(lock, 'STM')
        allowed  = 'the Moduline S1';
        template = 'GOcontroll Moduline M1 / L4 / HMI1';
    else
        allowed  = 'the Linux controllers (Moduline M1, L4, HMI1)';
        template = 'GOcontroll Moduline S1';
    end
    msg = sprintf(['This model is made for %s, so the Target ''%s'' cannot be selected. ' ...
                   'Start a new model from the template ''%s'' for that controller.'], ...
                  allowed, target, template);
end
