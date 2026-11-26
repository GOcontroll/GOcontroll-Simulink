function installPath = getInstallationLocation(compilerName)
%%***************************************************************************************
%% file         getInstallationLocation.m
%% brief        Returns the installation root directory for a given compiler/tool.
%%
%%              In this order:
%%              1. a folder filled in below (normally left empty);
%%              2. the compiler installed with GOcontrollInstallCompilers for this user;
%%              3. '' - the compiler is called by name and must be on the system PATH.
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
% A value below is the root directory of the toolchain (the folder that holds bin), or
% for Zig the folder that holds zig, e.g. 'C:/compilers/gcc-aarch64-linux'. Leave it
% empty to use GOcontrollInstallCompilers or the system PATH. This file is part of the
% blockset: a value filled in here is lost when the blockset is updated.

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

    if isempty(installPath)
        installPath = go_installed_toolchain('path', compilerName);
    end
end
