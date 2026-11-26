function ert_gocontroll_target_changed_callback_handler(varargin)
%%***************************************************************************************
%% file         ert_gocontroll_target_changed_callback_handler.m
%% brief        Callback invoked when the Target popup changes in the Code Generation
%%              panel. Shows/hides target-specific options and sets the correct
%%              ERTCustomFileTemplate for the selected GOcontroll target.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    hDlg = varargin{1};
    hSrc = varargin{2};

    target = slConfigUIGetVal(hDlg, hSrc, 'tlcGOcontrollTarget');

    % A model made from a template belongs to one platform (tlcGOcontrollPlatformLock).
    % Refuse a Target of the other platform and put the previous one back.
    lock = '';
    try
        lock = get_param(hSrc, 'tlcGOcontrollPlatformLock');
    catch
    end
    if ~isempty(lock) && ~strcmp(go_target_platform(target), lock)
        refused  = target;
        target   = get_param(hSrc, 'tlcGOcontrollTarget');    % the applied value
        if ~strcmp(go_target_platform(target), lock)
            if strcmp(lock, 'STM'), target = 'Moduline S1'; else, target = 'Moduline L4'; end
        end
        slConfigUISetVal(hDlg, hSrc, 'tlcGOcontrollTarget', target);
        errordlg(go_platform_lock_message(lock, refused), 'GOcontroll target', 'modal');
    end
    isStm  = strcmp(go_target_platform(target), 'STM');

    % Linux-specific options (greyed out when STM is selected)
    slConfigUISetEnabled(hDlg, hSrc, 'tlcLinuxPriority', ~isStm);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcXcpTcpAddress', ~isStm);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcXcpTcpPort',    ~isStm);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcUploadPort',    ~isStm);
    % Moduline S1 only: which controller a Bluetooth upload goes to
    slConfigUISetEnabled(hDlg, hSrc, 'tlcUploadS1Controller', isStm);

    % Word sizes of the platform (Hardware Implementation pane)
    slConfigUISetVal(hDlg, hSrc, 'ProdHWDeviceType', go_target_hardware(target));

    % Set the correct file template and makefile for the selected target
    if isStm
        slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate', 'ert_stm_file_process.tlc');
        slConfigUISetVal(hDlg, hSrc, 'TemplateMakefile',      'ert_stm_target_32.tmf');
    else
        slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate', 'ert_linux_file_process.tlc');
        slConfigUISetVal(hDlg, hSrc, 'TemplateMakefile',      'ert_linux_target_64.tmf');
    end
