function GOcontrollInstallCompilers(choice)
%%***************************************************************************************
%% file         GOcontrollInstallCompilers.m
%% brief        Downloads and installs the compilers the GOcontroll blockset builds with,
%%              in the versions it was tested with, for the current user.
%%
%%              GOcontrollInstallCompilers S1       Moduline S1:  Arm GNU Toolchain
%%                                                  arm-none-eabi 14.2.rel1
%%              GOcontrollInstallCompilers Linux    Moduline M1, L4, HMI1 (Compiler = GCC):
%%                                                  aarch64-none-linux-gnu gcc 9.2-2019.12
%%              GOcontrollInstallCompilers Zig      Moduline M1, L4, HMI1 (Compiler = Zig):
%%                                                  Zig 0.14.1
%%              GOcontrollInstallCompilers all      all three
%%              GOcontrollInstallCompilers          shows what is installed
%%
%%              The compilers go to a folder of the user, outside the project and outside
%%              the blockset, so they stay when the blockset is updated:
%%                  Windows:        %LOCALAPPDATA%\GOcontroll\tc\<compiler>
%%                  Linux, macOS:   ~/.gocontroll/tc/<compiler>
%%              The build finds them there by itself (getInstallationLocation); nothing has
%%              to be added to PATH. A compiler that was installed here is used before one
%%              on PATH. Run the command again to repair an installation.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    if nargin < 1 || isempty(choice)
        i_status();
        return
    end

    choice = lower(char(choice));
    switch choice
        case 's1',    names = {'arm-none-eabi'};
        case 'linux', names = {'aarch64-gcc'};
        case 'zig',   names = {'zig'};
        case 'all',   names = {'arm-none-eabi', 'aarch64-gcc', 'zig'};
        otherwise
            error('GOcontroll:installCompilers', ...
                  'Unknown choice ''%s''. Use S1, Linux, Zig or all.', choice);
    end

    for k = 1:numel(names)
        i_install(go_installed_toolchain('spec', names{k}));
    end
    fprintf('\nDone. Restart is not needed: the next build uses the installed compiler.\n');
end

%% ------------------------------------------------------------------------------------
function i_install(spec)
    if isempty(spec.url)
        fprintf('\n%s: no package for this computer (%s). Install it yourself and put it on PATH.\n', ...
                spec.title, computer);
        return
    end
    root   = go_installed_toolchain('root');
    target = fullfile(root, spec.folder);
    fprintf('\n%s\n  from    %s\n  size    about %d MB\n  into    %s\n', spec.title, spec.url, spec.mb, target);

    if ~isfolder(root), mkdir(root); end
    stage = fullfile(root, '_download');
    if isfolder(stage), rmdir(stage, 's'); end
    mkdir(stage);
    cleanStage = onCleanup(@() i_remove(stage));

    [~, file, ext] = fileparts(spec.url);
    archive = fullfile(stage, [file ext]);
    fprintf('  downloading ... (this can take several minutes)\n');
    websave(archive, spec.url, weboptions('Timeout', 120));

    fprintf('  unpacking ...\n');
    unpacked = fullfile(stage, 'x');
    mkdir(unpacked);
    i_unpack(archive, unpacked);
    delete(archive);

    % the package holds one top folder; that becomes <root>/<spec.folder>
    d = dir(unpacked); d = d(~ismember({d.name}, {'.', '..'}));
    if numel(d) == 1 && d(1).isdir
        src = fullfile(unpacked, d(1).name);
    else
        src = unpacked;
    end
    if isfolder(target), rmdir(target, 's'); end
    movefile(src, target);

    exe = go_installed_toolchain('exe', spec.name);
    if ~isfile(exe)
        error('GOcontroll:installCompilers', '%s: %s was not found after unpacking.', spec.title, exe);
    end
    [status, out] = system(['"' exe '" ' spec.versionArg]);
    lines = strsplit(strtrim(out), newline);
    if status ~= 0
        error('GOcontroll:installCompilers', '%s does not run: %s', exe, strtrim(out));
    end
    fprintf('  installed: %s\n', strtrim(lines{1}));
end

function i_unpack(archive, into)
    if endsWith(archive, '.zip') && ~ispc
        unzip(archive, into);
        return
    end
    % Windows 10 and later have tar (bsdtar), which also unpacks zip files and copes with
    % the long file names inside a compiler package better than unzip.
    [status, out] = system(sprintf('tar -xf "%s" -C "%s"', archive, into));
    if status ~= 0
        if endsWith(archive, '.zip')
            unzip(archive, into);
        else
            error('GOcontroll:installCompilers', 'Unpacking %s failed: %s', archive, strtrim(out));
        end
    end
end

function i_remove(folder)
    if isfolder(folder)
        try, rmdir(folder, 's'); catch, end
    end
end

function i_status()
    fprintf(['GOcontroll compilers\n\n' ...
             '  GOcontrollInstallCompilers S1      Moduline S1\n' ...
             '  GOcontrollInstallCompilers Linux   Moduline M1, L4, HMI1 (Compiler = GCC)\n' ...
             '  GOcontrollInstallCompilers Zig     Moduline M1, L4, HMI1 (Compiler = Zig)\n' ...
             '  GOcontrollInstallCompilers all     all three\n\n' ...
             'Installed for this user (%s):\n'], go_installed_toolchain('root'));
    names = {'arm-none-eabi', 'aarch64-gcc', 'zig'};
    for k = 1:numel(names)
        spec = go_installed_toolchain('spec', names{k});
        exe  = go_installed_toolchain('exe', names{k});
        if isfile(exe)
            state = 'installed';
        else
            state = 'not installed';
        end
        fprintf('  %-48s %s\n', spec.title, state);
    end
end
