function name = go_target_name(target)
%%***************************************************************************************
%% file         go_target_name.m
%% brief        Normalizes a GOcontroll target name to the current product naming.
%%
%%              'GOcontroll Moduline IoT'     -> 'Moduline S1'
%%              'GOcontroll Moduline Mini'    -> 'Moduline M1'
%%              'GOcontroll Moduline IV'      -> 'Moduline L4'
%%              'GOcontroll Moduline Display' -> 'Moduline HMI1'
%%
%%              Anything else is returned unchanged, so block mask initialization
%%              never fails on an unexpected value (unlike go_target_platform, which
%%              deliberately errors during a build).
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    switch target
        case 'GOcontroll Moduline IoT',     name = 'Moduline S1';
        case 'GOcontroll Moduline Mini',    name = 'Moduline M1';
        case 'GOcontroll Moduline IV',      name = 'Moduline L4';
        case 'GOcontroll Moduline Display', name = 'Moduline HMI1';
        otherwise,                          name = target;
    end
end
