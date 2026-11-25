function device = go_target_hardware(target)
%%***************************************************************************************
%% file         go_target_hardware.m
%% brief        The Hardware Implementation device type that belongs to a Target, or sets
%%              it on a model.
%%
%%              The word sizes of the generated code have to match the compiler of the
%%              platform: the Moduline S1 is a 32-bit Cortex-M (long is 32 bits), the
%%              Linux controllers are 64-bit ARM (long and pointers are 64 bits). With
%%              the wrong device type the generated code can stop the compiler with
%%              "Code was generated for compiler with different sized ulong/long".
%%
%%              device = go_target_hardware(target)   device type for a Target name
%%              go_target_hardware(mdl)               sets it on the model, from the
%%                                                    Target of its configuration set
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    isModel = false;
    try
        isModel = bdIsLoaded(target);
    catch
    end
    if isModel
        cs = getActiveConfigSet(target);
        device = go_target_hardware(get_param(cs, 'tlcGOcontrollTarget'));
        if ~strcmp(get_param(cs, 'ProdHWDeviceType'), device)
            set_param(cs, 'ProdHWDeviceType', device);
        end
        if ~strcmp(get_param(cs, 'ProdEqTarget'), 'on')
            set_param(cs, 'ProdEqTarget', 'on');
        end
        return
    end

    if strcmp(go_target_platform(target), 'STM')
        device = 'ARM Compatible->ARM Cortex-M';
    else
        device = 'ARM Compatible->ARM 64-bit (LP64)';
    end
end
