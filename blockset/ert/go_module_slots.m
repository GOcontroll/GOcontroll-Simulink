function n = go_module_slots(target)
%%***************************************************************************************
%% file         go_module_slots.m
%% brief        Number of module slots on a GOcontroll controller.
%%
%%              Moduline L4             -> 8
%%              Moduline M1             -> 4
%%              Moduline HMI1 / S1      -> 2  (the S1 shares the HMI1 slot pinout)
%%
%%              Legacy names are accepted via go_target_name. An unknown target (e.g.
%%              inside the library, which has no GOcontroll configuration set) returns
%%              8, so the full slot list stays available and mask initialization never
%%              fails on it.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    switch go_target_name(target)
        case 'Moduline L4',                   n = 8;
        case 'Moduline M1',                   n = 4;
        case {'Moduline HMI1', 'Moduline S1'}, n = 2;
        otherwise,                            n = 8;
    end
end
