function ert_linux_select_callback_handler(varargin)

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
    slConfigUISetVal(hDlg, hSrc, 'Solver', 'FixedStepDiscrete');
    slConfigUISetEnabled(hDlg, hSrc, 'Solver', true);
    slConfigUISetVal(hDlg, hSrc, 'SolverType', 'Fixed-step');
    slConfigUISetEnabled(hDlg, hSrc, 'SolverType', false);
    slConfigUISetVal(hDlg, hSrc, 'SampleTimeConstraint', 'Unconstrained');
    slConfigUISetVal(hDlg, hSrc, 'SolverMode', 'SingleTasking');
    slConfigUISetVal(hDlg, hSrc, 'AutoInsertRateTranBlk', 'off');

    % ----- data import/export pane -------------------------------------------

    % ----- optimization pane -------------------------------------------------

    % ----- diagnostics pane --------------------------------------------------

    % ----- hardware implementation pane --------------------------------------

    % ----- model referencing pane --------------------------------------------
	slConfigUISetVal(hDlg,hSrc,'ModelReferenceCompliant','on');
	slConfigUISetEnabled(hDlg,hSrc,'ModelReferenceCompliant',false);

    % ----- real time workshop pane -------------------------------------------
    % general (default settings)
%     slConfigUISetVal(hDlg, hSrc, 'GenerateReport', 'off');
%     slConfigUISetVal(hDlg, hSrc, 'IncludeHyperlinkInReport', 'off');
%     slConfigUISetVal(hDlg, hSrc, 'LaunchReport', 'off'); %
%     slConfigUISetVal(hDlg, hSrc, 'IgnoreCustomStorageClasses', 'off');
     slConfigUISetVal(hDlg, hSrc, 'MakeCommand', 'make_rtw');
	 slConfigUISetEnabled(hDlg, hSrc, 'GenCodeOnly', true);
     slConfigUISetVal(hDlg, hSrc, 'GenCodeOnly', 'off');
    %
    % debug
%     slConfigUISetVal(hDlg, hSrc, 'RTWVerbose', 'on');
%     slConfigUISetVal(hDlg, hSrc, 'RetainRTWFile', 'on');

    % interface
     slConfigUISetVal(hDlg, hSrc, 'MatFileLogging', 'off');
	 slConfigUISetVal(hDlg, hSrc, 'GenerateASAP2', 'on');

    % templates
     slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate', 'ert_linux_file_process.tlc');
     slConfigUISetEnabled(hDlg, hSrc, 'ERTCustomFileTemplate',true);
     slConfigUISetVal(hDlg, hSrc, 'GenerateSampleERTMain', 'off');
     slConfigUISetEnabled(hDlg, hSrc, 'GenerateSampleERTMain',false);

    % ----- other -------------------------------------------------------------
