function ert_gocontroll_select_callback_handler(varargin)
% ERT_GOCONTROLL_SELECT_CALLBACK_HANDLER
%   Fired when the Code Generation panel is opened.
%   Applies required solver settings and updates option visibility.

    hDlg = varargin{1};
    hSrc = varargin{2};

    % ----- solver pane -------------------------------------------------------
    slConfigUISetVal(hDlg, hSrc, 'Solver', 'FixedStepDiscrete');
    slConfigUISetEnabled(hDlg, hSrc, 'Solver', true);
    slConfigUISetVal(hDlg, hSrc, 'SolverType', 'Fixed-step');
    slConfigUISetEnabled(hDlg, hSrc, 'SolverType', false);
    slConfigUISetVal(hDlg, hSrc, 'SampleTimeConstraint', 'Unconstrained');
    slConfigUISetVal(hDlg, hSrc, 'SolverMode', 'SingleTasking');
    slConfigUISetVal(hDlg, hSrc, 'AutoInsertRateTranBlk', 'off');

    % ----- model referencing pane --------------------------------------------
    slConfigUISetVal(hDlg, hSrc, 'ModelReferenceCompliant', 'on');
    slConfigUISetEnabled(hDlg, hSrc, 'ModelReferenceCompliant', false);

    % ----- code generation pane ----------------------------------------------
    slConfigUISetVal(hDlg, hSrc, 'MakeCommand', 'make_rtw');
    slConfigUISetEnabled(hDlg, hSrc, 'GenCodeOnly', true);
    slConfigUISetVal(hDlg, hSrc, 'GenCodeOnly', 'off');
    slConfigUISetVal(hDlg, hSrc, 'RetainRTWFile', 'on');
    slConfigUISetVal(hDlg, hSrc, 'MatFileLogging', 'off');
    slConfigUISetVal(hDlg, hSrc, 'GenerateASAP2', 'off');
    slConfigUISetVal(hDlg, hSrc, 'GenerateSampleERTMain', 'off');
    slConfigUISetEnabled(hDlg, hSrc, 'GenerateSampleERTMain', false);

    % ----- parallel build ----------------------------------------------------
    slConfigUISetVal(hDlg, hSrc, 'ParMdlRefBuildCompliant', 'on');
    slConfigUISetEnabled(hDlg, hSrc, 'ParMdlRefBuildCompliant', false);

    % ----- update option visibility based on current target ------------------
    goc_target_changed_callback(hDlg, hSrc);
end
