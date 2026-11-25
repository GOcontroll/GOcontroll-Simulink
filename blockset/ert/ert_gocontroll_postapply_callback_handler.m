function ert_gocontroll_postapply_callback_handler(varargin)

% *************************************************************************
% Authors:    Lukáš Hamáček, Michal Sojka
% Date:      2007/01/18
%
%  Copyright:
%  (c) 2009 Dept. of Control Engineering, FEE, CTU Prague
%
% *************************************************************************

    hDlg = varargin{1};
    hSrc = varargin{2};

    % ----- solver pane -------------------------------------------------------
    % fixed-step required by the rtw
%    slConfigUISetVal(hDlg, hSrc, 'Solver', 'FixedStepDiscrete');
%    slConfigUISetEnabled(hDlg, hSrc, 'Solver', true);
%    slConfigUISetVal(hDlg, hSrc, 'SolverType', 'Fixed-step');
%    slConfigUISetEnabled(hDlg, hSrc, 'SolverType', false);
%    slConfigUISetVal(hDlg, hSrc, 'SampleTimeConstraint', 'Unconstrained');
%    slConfigUISetVal(hDlg, hSrc, 'SolverMode', 'Auto');
%    slConfigUISetVal(hDlg, hSrc, 'AutoInsertRateTranBlk', 'off');

    % ----- data import/export pane -------------------------------------------

    % ----- optimization pane -------------------------------------------------

    % ----- diagnostics pane --------------------------------------------------

    % ----- hardware implementation pane --------------------------------------

    % ----- model referencing pane --------------------------------------------

    % ----- real time workshop pane -------------------------------------------
    % general (default settings)
%     slConfigUISetVal(hDlg, hSrc, 'GenerateReport', 'off');
%     slConfigUISetVal(hDlg, hSrc, 'IncludeHyperlinkInReport', 'off');
%     slConfigUISetVal(hDlg, hSrc, 'LaunchReport', 'off'); %
%     slConfigUISetVal(hDlg, hSrc, 'IgnoreCustomStorageClasses', 'off');
%     slConfigUISetVal(hDlg, hSrc, 'MakeCommand', 'make_rtw CC=powerpc-603e-linux-gnu-gcc.exe');
%     slConfigUISetVal(hDlg, hSrc, 'GenCodeOnly', 'on');
%     slConfigUISetEnabled(hDlg, hSrc, 'GenCodeOnly', false);
    %
    % debug
%     slConfigUISetVal(hDlg, hSrc, 'RTWVerbose', 'on');
%     slConfigUISetVal(hDlg, hSrc, 'RetainRTWFile', 'on');

    % interface
%     slConfigUISetVal(hDlg, hSrc, 'GenFloatMathFcnCalls', 'ANSI_C');
%     slConfigUISetEnabled(hDlg, hSrc, 'GenFloatMathFcnCalls', false);
%     slConfigUISetVal(hDlg, hSrc, 'MultiInstanceERTCode', 'off');
%     slConfigUISetEnabled(hDlg, hSrc, 'MultiInstanceERTCode', false);
%     slConfigUISetVal(hDlg, hSrc, 'GRTInterface', 'off');
%     slConfigUISetEnabled(hDlg, hSrc, 'GRTInterface', false);
%     slConfigUISetVal(hDlg, hSrc, 'ExtModeMexArgs', '''192.168.123.199'' 1 17725');

%     slConfigUISetVal(hDlg, hSrc, 'CombineOutputUpdateFcns', 'on');
%     slConfigUISetEnabled(hDlg, hSrc, 'CombineOutputUpdateFcns', false);
%     slConfigUISetVal(hDlg, hSrc, 'GRTInterface', 'off');
%     slConfigUISetEnabled(hDlg, hSrc, 'GRTInterface', false);

%     slConfigUISetVal(hDlg, hSrc, 'GenerateErtSFunction', 'off');
%     slConfigUISetEnabled(hDlg, hSrc, 'GenerateErtSFunction', false);
%     slConfigUISetVal(hDlg, hSrc, 'MatFileLogging', 'off');

    % templates
%     slConfigUISetVal(hDlg, hSrc, 'ERTSrcFileBannerTemplate', 'linux_ert_target_code_template.cgt');
%     slConfigUISetEnabled(hDlg, hSrc, 'ERTSrcFileBannerTemplate',false);
%     slConfigUISetVal(hDlg, hSrc, 'ERTHdrFileBannerTemplate', 'linux_ert_target_code_template.cgt');
%     slConfigUISetEnabled(hDlg, hSrc, 'ERTHdrFileBannerTemplate',false);
%     slConfigUISetVal(hDlg, hSrc, 'ERTDataSrcFileTemplate', 'linux_ert_target_code_template.cgt');
%     slConfigUISetEnabled(hDlg, hSrc, 'ERTDataSrcFileTemplate',false);
%     slConfigUISetVal(hDlg, hSrc, 'ERTDataHdrFileTemplate', 'linux_ert_target_code_template.cgt');
%     slConfigUISetEnabled(hDlg, hSrc, 'ERTDataHdrFileTemplate',false);
%    slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate', 'ert_linux_file_process.tlc');
%    slConfigUISetEnabled(hDlg, hSrc, 'ERTCustomFileTemplate',true);
%    slConfigUISetVal(hDlg, hSrc, 'GenerateSampleERTMain', 'off');
%    slConfigUISetEnabled(hDlg, hSrc, 'GenerateSampleERTMain',false);

    % ----- other -------------------------------------------------------------
	disp('User Changed settings, this might prevent the code generation from working properly');

    % ----- refresh the model when the Target changed --------------------------
    % The block masks mirror tlcGOcontrollTarget into their read-only ContrType
    % field and derive their CAN channel descriptions from it. Those only
    % re-evaluate when the model is refreshed, so without this the canvas keeps
    % showing the previous controller's pinout after switching Target.
    i_refreshOnTargetChange(hSrc);
end

%%***************************************************************************************
%% Local functions
%%***************************************************************************************

function i_refreshOnTargetChange(hSrc)
% Refreshes the model that owns this configuration set, but only when its block
% masks are actually out of date with the selected Target.

    mdl = i_modelOfConfigSet(hSrc);
    if isempty(mdl)
        return
    end

    try
        target = go_target_name(get_param(getActiveConfigSet(mdl), 'tlcGOcontrollTarget'));
    catch
        return  % not a GOcontroll target file - nothing to refresh
    end

    stale = i_staleMirrorBlocks(mdl, target);
    if isempty(stale)
        return  % Target unchanged, or no blocks that care
    end

    % Defer the refresh until the Configuration Parameters dialog has finished
    % applying - modifying the model from inside the callback itself is refused.
    t = timer('StartDelay',     0.01, ...
              'ExecutionMode',  'singleShot', ...
              'Name',           'GOcontrollTargetRefresh', ...
              'TimerFcn',       @(~,~) i_doRefresh(mdl, target, numel(stale)), ...
              'StopFcn',        @(src,~) delete(src));
    start(t);
end

function i_doRefresh(mdl, target, nStale)
    if ~bdIsLoaded(mdl)
        return  % model closed in the meantime
    end
    try
        % A model update is what actually re-evaluates the mask initialization
        % code. Simulink.BlockDiagram.refreshBlocks only refreshes library
        % links and leaves the mask parameters at their previous values.
        set_param(mdl, 'SimulationCommand', 'update');
        fprintf('### Target set to %s - refreshed %d block(s) in %s\n', target, nStale, mdl);
    catch ME
        warning('GOcontroll:targetRefreshFailed', ...
                ['Could not refresh ''%s'' after changing Target to %s (%s). ' ...
                 'Press Ctrl+D to update the model manually.'], mdl, target, ME.message);
    end
end

function mdl = i_modelOfConfigSet(hSrc)
% Finds the loaded model whose active configuration set is hSrc. The callback
% only gets the configuration set, and bdroot/gcs are not reliable here when
% more than one model is open.

    mdl = '';
    try
        candidates = find_system('type', 'block_diagram');
    catch
        return
    end
    for k = 1:numel(candidates)
        try
            if getActiveConfigSet(candidates{k}) == hSrc
                mdl = candidates{k};
                return
            end
        catch
            % libraries and models without a config set - skip
        end
    end
end

function stale = i_staleMirrorBlocks(mdl, target)
% Blocks whose read-only ContrType mirror no longer matches the selected Target.
% Only the 'edit' flavour mirrors the configuration set; the 'popup' flavour is
% a separate, user-chosen controller type and must be left alone.

    stale = {};
    try
        blks = find_system(mdl, 'LookUnderMasks', 'all', 'FollowLinks', 'on', ...
                                'RegExp', 'on', 'MaskType', '.');
    catch
        return
    end
    for k = 1:numel(blks)
        try
            p = Simulink.Mask.get(blks{k}).getParameter('ContrType');
        catch
            continue
        end
        if isempty(p) || ~strcmp(p.Type, 'edit')
            continue
        end
        if ~strcmp(p.Value, target)
            stale{end+1} = blks{k}; %#ok<AGROW>
        end
    end
end
