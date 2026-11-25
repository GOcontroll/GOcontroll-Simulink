function installPath = getInstallationLocation(compilerName)
%%***************************************************************************************
%% file         getInstallationLocation.m
%% brief        Returns the installation root directory for a given compiler/tool.
%%              Configure the paths below once per development machine.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************
%
% Supported compilerName values:
%   'aarch64-none-linux-gnu-gcc'  - GCC cross-compiler for Linux targets (aarch64)
%   'arm-none-eabi-gcc'           - GCC cross-compiler for bare-metal STM32 targets
%   'Zig-x86'                     - Zig compiler for x86 host machines
%   'Zig-aarch64'                 - Zig compiler for aarch64 host machines
%
% Set each value to the root directory of the installed toolchain,
% e.g. 'C:/compilers/gcc-aarch64-linux' or '/opt/gcc-aarch64-linux'.
% Leave empty ('') if the compiler is available on the system PATH.

    switch compilerName
        case 'aarch64-none-linux-gnu-gcc'
            installPath = '';
        case 'arm-none-eabi-gcc'
            installPath = '';
        case 'Zig-x86'
            installPath = '';
        case 'Zig-aarch64'
            installPath = '';
        otherwise
            error('getInstallationLocation: unknown compiler ''%s''.', compilerName);
    end
end
