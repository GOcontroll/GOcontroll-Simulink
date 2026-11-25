function repaired = go_repair_target_options(mdl, defer)
%%***************************************************************************************
%% file         go_repair_target_options.m
%% brief        Repairs the GOcontroll options pane when another copy of
%%              ert_gocontroll.tlc shadowed this repository's copy at model load.
%%
%%              Simulink resolves a model's system target file BEFORE that model's
%%              PreLoadFcn (librarySetup) has put this repository on the MATLAB path.
%%              When an older GOcontroll blockset is installed as an Add-On toolbox or
%%              sits in a second clone, its ert_gocontroll.tlc wins that race and
%%              Configuration Parameters > Code Generation > GOcontroll options is built
%%              from the wrong file: Target, Use precompiled binary and Delete build
%%              files are missing, and obsolete options take their place.
%%
%%              By the time the model has loaded, librarySetup has corrected the path,
%%              so simply loading the model again resolves the right target file and
%%              reads every stored option value back from the model. That is what this
%%              does. Patching the options in place does not work: Simulink discards
%%              option values written while the model is still settling, so the pane
%%              would come back complete but with Target silently reset to its default.
%%
%%              go_repair_target_options(mdl)        repair now
%%              go_repair_target_options(mdl, true)  repair once the load has finished
%%
%%              Pass true from a PostLoadFcn - a model cannot be closed from inside its
%%              own load callback. It is a no-op when the pane is already correct, which
%%              is the normal case, and it never runs more than once per model per
%%              MATLAB session.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    repaired = false;

    if nargin < 2 || isempty(defer)
        defer = false;
    end
    if nargin < 1 || isempty(mdl)
        mdl = bdroot;
    end
    if ~ischar(mdl) && ~isstring(mdl)
        return
    end
    mdl = char(mdl);
    if isempty(mdl) || ~bdIsLoaded(mdl)
        return
    end

    if ~i_isStale(mdl)
        return                      % pane is complete - nothing to do
    end

    stf = which('ert_gocontroll.tlc');
    if isempty(stf)
        warning('GOcontroll:targetFileNotOnPath', ...
                ['The GOcontroll options of ''%s'' are incomplete and ' ...
                 '''ert_gocontroll.tlc'' is not on the MATLAB path. Run librarySetup ' ...
                 'from this project folder and reopen the model.'], mdl);
        return
    end

    % One attempt per model per session. Without this, a target file that stays
    % wrong after the reload would send the model round in circles.
    flag = ['GOcontrollOptionsReload_' matlab.lang.makeValidName(mdl)];
    if isappdata(0, flag)
        return
    end

    if defer
        % A model cannot be closed from inside its own load callback, so hand the
        % work to the event queue. Remember now whether the model had unsaved
        % changes - anything written after this point comes from the rest of the
        % load callbacks and is reproduced by the reload.
        wasDirty = get_param(mdl, 'Dirty');
        wasOpen  = get_param(mdl, 'Open');
        i_schedule(mdl, stf, wasDirty, wasOpen, 120);
        return
    end

    repaired = i_reload(mdl, stf, get_param(mdl, 'Dirty'), get_param(mdl, 'Open'), 0);
end

%%***************************************************************************************
%% Local functions
%%***************************************************************************************

function stale = i_isStale(mdl)
% True when the model builds with this blockset but its option pane is missing the
% options that only this repository's target file defines.

    stale = false;
    try
        cs = getActiveConfigSet(mdl);
    catch
        return                      % library or model without a configuration set
    end
    if isempty(cs) || ~strcmp(get_param(cs, 'SystemTargetFile'), 'ert_gocontroll.tlc')
        return                      % not a GOcontroll model
    end
    stale = ~isfield(get_param(cs, 'ObjectParameters'), 'tlcGOcontrollTarget');
end

function i_schedule(mdl, stf, wasDirty, wasOpen, attemptsLeft)
% Puts the reload on the event queue. Simulink pumps that queue while it is still
% loading the model, so the first attempts can land too early to close it; each
% failed attempt comes back here until the load has finished.

    t = timer('StartDelay',    0.1, ...
              'ExecutionMode', 'singleShot', ...
              'Name',          'GOcontrollTargetOptionsRepair', ...
              'TimerFcn',      @(~,~) i_reload(mdl, stf, wasDirty, wasOpen, attemptsLeft), ...
              'StopFcn',       @(src,~) delete(src));
    start(t);
end

function repaired = i_reload(mdl, stf, wasDirty, wasOpen, attemptsLeft)
% Loads the model again, now that librarySetup has put this repository in front on
% the MATLAB path.

    repaired = false;
    if ~bdIsLoaded(mdl) || ~i_isStale(mdl)
        return
    end

    setappdata(0, ['GOcontrollOptionsReload_' matlab.lang.makeValidName(mdl)], true);

    if strcmp(wasDirty, 'on')
        warning('GOcontroll:targetOptionsStaleAndDirty', ...
                ['The GOcontroll options of ''%s'' were built from another blockset ' ...
                 '(%s), but the model has unsaved changes so it was not reloaded. ' ...
                 'Save your work, then reopen the model.'], mdl, stf);
        return
    end

    file = get_param(mdl, 'FileName');
    if isempty(file)
        return
    end

    try
        close_system(mdl, 0);
        if strcmp(wasOpen, 'on')
            open_system(file);
        else
            load_system(file);
        end
    catch ME
        if attemptsLeft > 0 && bdIsLoaded(mdl)
            % Almost always 'because it is being opened or loaded' - come back
            % once Simulink has finished with it.
            i_schedule(mdl, stf, wasDirty, wasOpen, attemptsLeft - 1);
            return
        end
        warning('GOcontroll:targetOptionsRepairFailed', ...
                'Could not reload ''%s'' to rebuild its GOcontroll options: %s', ...
                mdl, ME.message);
        return
    end

    if i_isStale(mdl)
        warning('GOcontroll:staleBlocksetOnPath', ...
                ['The GOcontroll options of ''%s'' are still incomplete after reloading. ' ...
                 '''%s'' does not define Target - an older GOcontroll blockset is ' ...
                 'shadowing this project. Remove it from the MATLAB path.'], mdl, stf);
        return
    end

    repaired = true;
    fprintf(['### GOcontroll options of ''%s'' were built from a shadowing blockset ' ...
             'and have been\n###   reloaded from %s\n### Target is %s. Uninstall or ' ...
             'disable the other GOcontroll blockset to avoid this.\n'], ...
            mdl, stf, get_param(getActiveConfigSet(mdl), 'tlcGOcontrollTarget'));
end
