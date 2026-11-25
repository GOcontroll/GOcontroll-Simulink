function ert_gocontroll_select_callback_handler(varargin)
%%***************************************************************************************
%% file         ert_gocontroll_select_callback_handler.m
%% brief        SelectCallback invoked when the Code Generation panel is opened.
%%              Applies mandatory solver and code generation settings for all
%%              GOcontroll targets (Linux and STM).
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    hDlg = varargin{1};
    hSrc = varargin{2};

    % ----- solver pane -------------------------------------------------------
    slConfigUISetVal    (hDlg, hSrc, 'Solver',                  'FixedStepDiscrete');
    slConfigUISetEnabled(hDlg, hSrc, 'Solver',                  true);
    slConfigUISetVal    (hDlg, hSrc, 'SolverType',              'Fixed-step');
    slConfigUISetEnabled(hDlg, hSrc, 'SolverType',              false);
    slConfigUISetVal    (hDlg, hSrc, 'SampleTimeConstraint',    'Unconstrained');
    slConfigUISetVal    (hDlg, hSrc, 'SolverMode',              'SingleTasking');
    slConfigUISetVal    (hDlg, hSrc, 'AutoInsertRateTranBlk',   'off');

    % ----- model referencing pane --------------------------------------------
    slConfigUISetVal    (hDlg, hSrc, 'ModelReferenceCompliant', 'on');
    slConfigUISetEnabled(hDlg, hSrc, 'ModelReferenceCompliant', false);

    % ----- code generation pane ----------------------------------------------
    slConfigUISetVal    (hDlg, hSrc, 'MakeCommand',             'make_rtw');
    slConfigUISetEnabled(hDlg, hSrc, 'GenCodeOnly',             true);
    slConfigUISetVal    (hDlg, hSrc, 'GenCodeOnly',             'off');
    slConfigUISetVal    (hDlg, hSrc, 'RetainRTWFile',           'on');

    % ----- interface ---------------------------------------------------------
    slConfigUISetVal    (hDlg, hSrc, 'MatFileLogging',          'off');
    slConfigUISetVal    (hDlg, hSrc, 'GenerateASAP2',           'off');

    % ----- templates ---------------------------------------------------------
    slConfigUISetEnabled(hDlg, hSrc, 'ERTCustomFileTemplate',   true);
    slConfigUISetVal    (hDlg, hSrc, 'GenerateSampleERTMain',   'off');
    slConfigUISetEnabled(hDlg, hSrc, 'GenerateSampleERTMain',   false);

    % ----- other -------------------------------------------------------------
    slConfigUISetVal    (hDlg, hSrc, 'ParMdlRefBuildCompliant', 'on');
    slConfigUISetEnabled(hDlg, hSrc, 'ParMdlRefBuildCompliant', false);
