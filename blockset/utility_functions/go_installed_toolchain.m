function out = go_installed_toolchain(what, name)
%%***************************************************************************************
%% file         go_installed_toolchain.m
%% brief        Where GOcontrollInstallCompilers puts the compilers, and what it installs.
%%
%%              go_installed_toolchain('root')            folder of the installed compilers
%%              go_installed_toolchain('spec', name)      package of one compiler (struct)
%%              go_installed_toolchain('exe', name)       its compiler executable
%%              go_installed_toolchain('path', compiler)  installation folder for
%%                                                        getInstallationLocation: the root
%%                                                        folder of an installed GCC, the
%%                                                        folder that holds zig; '' when it
%%                                                        is not installed here
%%
%%              name:     'arm-none-eabi', 'aarch64-gcc' or 'zig'
%%              compiler: the names getInstallationLocation knows
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    switch what
        case 'root'
            out = i_root();
        case 'spec'
            out = i_spec(name);
        case 'exe'
            spec = i_spec(name);
            out  = fullfile(i_root(), spec.folder, spec.exe);
        case 'path'
            map = struct('aarch64_none_linux_gnu_gcc', 'aarch64-gcc', ...
                         'arm_none_eabi_gcc',          'arm-none-eabi', ...
                         'Zig_x86',                    'zig', ...
                         'Zig_aarch64',                'zig');
            key = strrep(name, '-', '_');
            out = '';
            if ~isfield(map, key), return, end
            spec = i_spec(map.(key));
            if isfile(fullfile(i_root(), spec.folder, spec.exe))
                out = fullfile(i_root(), spec.folder);
            end
        otherwise
            error('go_installed_toolchain: unknown request ''%s''.', what);
    end
end

function r = i_root()
    if ispc
        r = fullfile(getenv('LOCALAPPDATA'), 'GOcontroll', 'tc');
    else
        r = fullfile(getenv('HOME'), '.gocontroll', 'tc');
    end
end

function s = i_spec(name)
    if ispc, x = '.exe'; else, x = ''; end
    gh = 'https://github.com/GOcontroll/GOcontroll-Simulink/releases/download/';
    arm = 'https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/';
    s = struct('name', name, 'url', '', 'mb', 0, 'versionArg', '--version');
    switch name
        case 'arm-none-eabi'
            s.title  = 'Arm GNU Toolchain arm-none-eabi 14.2.rel1 (Moduline S1)';
            s.folder = 'arm-none-eabi-14.2';
            s.exe    = fullfile('bin', ['arm-none-eabi-gcc' x]);
            if ispc
                s.url = [arm 'arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi.zip']; s.mb = 300;
            elseif strcmp(computer, 'GLNXA64')
                s.url = [arm 'arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz'];        s.mb = 145;
            end
        case 'aarch64-gcc'
            s.title  = 'GCC 9.2-2019.12 aarch64-none-linux-gnu (Moduline M1, L4, HMI1)';
            s.folder = 'aarch64-gcc-9.2';
            s.exe    = fullfile('bin', ['aarch64-none-linux-gnu-gcc' x]);
            if ispc
                s.url = [gh 'V4.0.0/gcc-arm-9.2-2019.12-mingw-w64-i686-aarch64-none-linux-gnu.zip']; s.mb = 650;
            elseif strcmp(computer, 'GLNXA64')
                s.url = [gh 'V4.0.0/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu.zip'];         s.mb = 560;
            end
        case 'zig'
            s.title  = 'Zig 0.14.1 (Moduline M1, L4, HMI1)';
            s.folder = 'zig-0.14.1';
            s.exe    = ['zig' x];
            s.versionArg = 'version';
            if ispc
                s.url = [gh 'V4.4.3/zig-x86_64-windows-0.14.1.zip']; s.mb = 80;
            elseif strcmp(computer, 'GLNXA64')
                s.url = [gh 'V4.4.3/zig-x86_64-linux-0.14.1.zip'];   s.mb = 80;
            elseif strcmp(computer, 'MACA64')
                s.url = [gh 'V4.4.3/zig-aarch64-macos-0.14.1.zip'];  s.mb = 80;
            elseif strcmp(computer, 'MACI64')
                s.url = [gh 'V4.4.3/zig-x86_64-macos-0.14.1.zip'];   s.mb = 80;
            end
        otherwise
            error('go_installed_toolchain: unknown compiler ''%s''.', name);
    end
end
