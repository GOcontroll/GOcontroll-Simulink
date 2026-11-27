function others = go_other_blocksets(repoRoot, options)
%%***************************************************************************************
%% file         go_other_blocksets.m
%% brief        Finds other copies of the GOcontroll blockset that MATLAB can reach next to
%%              the one in repoRoot, and warns about them (GOcontroll:otherBlockset).
%%              librarySetup calls it before it loads the libraries.
%%
%%              A second copy - an Add-On, another clone on the MATLAB path - lets
%%              Simulink take the system target file, the libraries or the blocks from
%%              that copy: options and fixes of this blockset are then missing.
%%
%%              others = go_other_blocksets(repoRoot)
%%                  repoRoot  root folder of this blockset (the folder of librarySetup.m)
%%                  others    struct array, one element per other copy:
%%                            root     its root folder
%%                            version  its version, 'V5.0.0', or 'unknown version'
%%                            addOn    true when it is installed as a MATLAB Add-On
%%
%%              For the tests, targetFiles= and libraryFiles= replace what MATLAB reports:
%%              every ert_gocontroll.tlc on the path, and the file of every GOcontroll
%%              library that is loaded.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    arguments
        repoRoot (1,:) char
        options.targetFiles  cell = i_target_files()
        options.libraryFiles cell = i_library_files()
    end

    files  = [options.targetFiles(:); options.libraryFiles(:)];
    others = struct('root', {}, 'version', {}, 'addOn', {});
    for k = 1:numel(files)
        root = i_root(files{k});
        if isempty(root) || i_same(root, repoRoot) || any(cellfun(@(r) i_same(r, root), {others.root}))
            continue
        end
        others(end+1) = struct('root', root, 'version', i_version(root), 'addOn', i_is_addon(root)); %#ok<AGROW>
    end

    if ~isempty(others)
        i_warn(repoRoot, others);
    end
    if nargout == 0
        clear others    % a call as a statement does not leave ans behind in librarySetup
    end
end

function files = i_target_files()
    files = which('ert_gocontroll.tlc', '-all');
end

function files = i_library_files()
    files = {};
    for lib = {'GOcontroll_Moduline_Target', 'GOcontroll_Generic'}
        if bdIsLoaded(lib{1})
            files{end+1} = get_param(lib{1}, 'FileName'); %#ok<AGROW>
        end
    end
end

% The root of the copy a file belongs to: the folder that holds its blockset folder.
function root = i_root(file)
    root = '';
    if isempty(file), return, end
    k = regexp(file, '[\\/]blockset[\\/]', 'start');
    if isempty(k)
        root = fileparts(file);
    else
        root = file(1:k(end) - 1);
    end
end

function same = i_same(a, b)
    a = i_normal(a);
    b = i_normal(b);
    if ispc
        same = strcmpi(a, b);
    else
        same = strcmp(a, b);
    end
end

function p = i_normal(p)
    if ispc, p = strrep(p, '/', '\'); end
    p = regexprep(p, '[\\/]+$', '');
end

function v = i_version(root)
    v = 'unknown version';
    file = fullfile(root, 'blockset', 'GOcontroll_Simulink_version.m');
    if ~isfile(file), return, end
    t = regexp(fileread(file), 'version\s*=\s*''([^'']*)''', 'tokens', 'once');
    if ~isempty(t)
        v = regexprep(t{1}, '^GOcontroll\s+', '');
    end
end

% MATLAB installs Add-Ons in a folder called "MATLAB Add-Ons" or "MATLABadd_ons_<release>".
function addOn = i_is_addon(root)
    addOn = contains(lower(root), {'add-ons', 'add_ons'});
end

function i_warn(repoRoot, others)
    lines = cell(1, numel(others));
    for k = 1:numel(others)
        tag = '';
        if others(k).addOn, tag = '   (Add-On)'; end
        lines{k} = sprintf('    %-16s %s%s', others(k).version, others(k).root, tag);
    end
    msg = sprintf([ ...
        'MATLAB also reaches another copy of the GOcontroll blockset than the one in\n' ...
        '%s (%s):\n\n%s\n\n' ...
        'Simulink can take the system target file, the libraries and the blocks from that copy:\n' ...
        'options and fixes of this blockset are then missing. Remove the other copy:\n' ...
        '  - an Add-On: Home > Add-Ons > Manage Add-Ons, then Uninstall at GOcontroll-Simulink\n' ...
        '  - another folder: Home > Set Path, Remove its folders and Save, and do not start\n' ...
        '    MATLAB in that folder\n' ...
        'Then restart MATLAB in your project folder.'], ...
        repoRoot, i_version(repoRoot), strjoin(lines, newline));
    backtrace = warning('off', 'backtrace');
    restore = onCleanup(@() warning(backtrace)); %#ok<NASGU>  the backtrace comes back on return
    warning('GOcontroll:otherBlockset', '%s', msg);
end
