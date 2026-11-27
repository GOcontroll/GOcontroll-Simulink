function out = go_codebase_cache(action, varargin)
%%***************************************************************************************
%% file         go_codebase_cache.m
%% brief        The cache behind the option Use precompiled binary: the CodeBase is
%%              compiled once, and later builds link its objects from the cache until the
%%              CodeBase, the compiler or the build files change. Used by
%%              ert_linux_make_rtw_hook for every target.
%%
%%              cache = go_codebase_cache('open', spec)
%%                  spec.buildDir    build folder of the model (<model folder>/build)
%%                  spec.key         platform and compiler, e.g. 'Linux-GCC': each has its
%%                                   own folder build/codebase_objs/<key>, so models for
%%                                   the Moduline S1 and for a Linux controller can share
%%                                   a project folder
%%                  spec.codeRoot    the code folder of the CodeBase: every .c and .h in
%%                                   it and below counts for the fingerprint
%%                  spec.srcDirs     folders whose .c files go into the build
%%                  spec.exclude     names of .c files in srcDirs that do not
%%                  spec.buildFiles  files that set the compiler and its flags (the hook,
%%                                   the template makefile)
%%                  spec.settings    text: the compiler and the flags it gets
%%
%%                  cache.dir          folder of the cached objects
%%                  cache.sources      .c files whose object is cached (name, folder)
%%                  cache.perBuild     .c files that include SYS_config.h: compiled every
%%                                     build, see i_sources
%%                  cache.fingerprint  cellstr: what the objects are made from
%%                  cache.usable       true when the cache has an object of every source,
%%                                     made from this fingerprint
%%                  cache.reason       why it is not usable, '' when it is
%%
%%              go_codebase_cache('store', cache, objFolder)
%%                  After a build that compiled the CodeBase: copies the objects of
%%                  cache.sources from objFolder to cache.dir and records the fingerprint.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    switch action
        case 'open'
            out = i_open(varargin{:});
        case 'store'
            i_store(varargin{:});
        otherwise
            error('go_codebase_cache: unknown action ''%s''.', action);
    end
end

function cache = i_open(spec)
    i_remove_flat_cache(spec.buildDir);
    cache.dir = fullfile(spec.buildDir, 'codebase_objs', spec.key);
    [cache.sources, cache.perBuild] = i_sources(spec.srcDirs, spec.exclude);
    cache.fingerprint = i_fingerprint(spec);
    [cache.usable, cache.reason] = i_usable(cache);
end

function i_store(cache, objFolder)
    if ~isfolder(cache.dir)
        mkdir(cache.dir);
    end
    % The fingerprint goes first and comes back last: a store that stops halfway
    % leaves a cache that is not used.
    stamp = fullfile(cache.dir, 'fingerprint.mat');
    if isfile(stamp), delete(stamp); end
    old = dir(fullfile(cache.dir, '*.obj'));
    for k = 1:numel(old)
        delete(fullfile(old(k).folder, old(k).name));
    end
    for k = 1:numel(cache.sources)
        obj = i_obj(cache.sources(k).name);
        if ~isfile(fullfile(objFolder, obj))
            warning('GOcontroll:codebaseCache', ...
                '### The CodeBase is not cached, %s is missing: the next build compiles it again', obj);
            return
        end
        copyfile(fullfile(objFolder, obj), fullfile(cache.dir, obj));
    end
    fingerprint = cache.fingerprint; %#ok<NASGU>  saved below
    save(stamp, 'fingerprint');
    [~, key] = fileparts(cache.dir);
    disp(['### Cached ' num2str(numel(cache.sources)) ' CodeBase objects in build/codebase_objs/' key '/']);
end

% The .c files of the build. A file that includes SYS_config.h is compiled every build:
% that header is written again for each build (station ID, XCP port, CAN buffer size),
% and an object from the cache would report the station ID of an older build, which
% then no longer matches the a2l.
function [cached, perBuild] = i_sources(srcDirs, exclude)
    cached   = struct('name', {}, 'folder', {});
    perBuild = cached;
    for d = 1:numel(srcDirs)
        files = dir(fullfile(srcDirs{d}, '*.c'));
        for k = 1:numel(files)
            if any(strcmp(files(k).name, exclude)), continue, end
            src = struct('name', files(k).name, 'folder', files(k).folder);
            if contains(fileread(fullfile(src.folder, src.name)), 'SYS_config.h')
                perBuild(end+1) = src; %#ok<AGROW>
            else
                cached(end+1) = src; %#ok<AGROW>
            end
        end
    end
end

% One line per thing the objects depend on: the settings, each build file and every
% .c and .h of the CodeBase, each with its size and date. A git checkout or pull gives
% a changed file a new date, so the cache is made again after every change.
function lines = i_fingerprint(spec)
    lines = {['settings ' spec.settings]};
    for k = 1:numel(spec.buildFiles)
        lines{end+1} = i_stamp(dir(spec.buildFiles{k}), spec.buildFiles{k}); %#ok<AGROW>
    end
    files = [dir(fullfile(spec.codeRoot, '**', '*.c')); dir(fullfile(spec.codeRoot, '**', '*.h'))];
    code = cell(numel(files), 1);
    for k = 1:numel(files)
        code{k} = i_stamp(files(k), '');
    end
    lines = [lines(:); sort(code)];
end

function line = i_stamp(file, name)
    if isempty(file)
        line = ['missing ' name];
    else
        line = sprintf('%s %d %.8f', fullfile(file(1).folder, file(1).name), file(1).bytes, file(1).datenum);
    end
end

function [usable, reason] = i_usable(cache)
    usable = false;
    stamp = fullfile(cache.dir, 'fingerprint.mat');
    if ~isfile(stamp)
        reason = 'no cached CodeBase objects yet';
        return
    end
    try
        saved = load(stamp, 'fingerprint');
        before = saved.fingerprint;
    catch
        before = {};
    end
    if ~iscellstr(before), before = {}; end %#ok<ISCLSTR>  a damaged stamp: make the cache again
    changed = setdiff(cache.fingerprint, before, 'stable');
    gone    = setdiff(before, cache.fingerprint, 'stable');
    if ~isempty(changed)
        reason = ['changed since the cache was made: ' i_what(changed{1})];
        return
    end
    if ~isempty(gone)
        reason = ['changed since the cache was made: ' i_what(gone{1})];
        return
    end
    for k = 1:numel(cache.sources)
        if ~isfile(fullfile(cache.dir, i_obj(cache.sources(k).name)))
            reason = [i_obj(cache.sources(k).name) ' is not in the cache'];
            return
        end
    end
    usable = true;
    reason = '';
end

% The name of the file a fingerprint line is about, or 'the compiler settings'.
function what = i_what(line)
    if startsWith(line, 'settings ')
        what = 'the compiler settings';
    elseif startsWith(line, 'missing ')
        [~, name, ext] = fileparts(extractAfter(line, 'missing '));
        what = [name ext];
    else
        [~, name, ext] = fileparts(regexprep(line, ' \d+ [\d.]+$', ''));
        what = [name ext];
    end
end

function obj = i_obj(source)
    obj = regexprep(source, '\.c$', '.obj');
end

% Blockset 5.0.0 and older kept the objects of the Moduline S1 straight in
% build/codebase_objs/, without a fingerprint: they are never used again.
function i_remove_flat_cache(buildDir)
    old = dir(fullfile(buildDir, 'codebase_objs', '*.obj'));
    for k = 1:numel(old)
        delete(fullfile(old(k).folder, old(k).name));
    end
    if ~isempty(old)
        disp('### Removed the CodeBase cache of an older blockset from build/codebase_objs/');
    end
end
