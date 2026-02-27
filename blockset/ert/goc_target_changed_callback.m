function goc_target_changed_callback(hDlg, hSrc)
% GOC_TARGET_CHANGED_CALLBACK  Show/hide target-specific options.
%   Called when the Target popup changes, and at panel open time.
%   Uses SetEnabled (always works) + SetVisible in try-catch (works after
%   MATLAB restart in 2023b).

    target = slConfigUIGetVal(hDlg, hSrc, 'tlcGOcontrollTarget');
    isIoT  = strcmp(target, 'GOcontroll Moduline IoT');

    % Update template makefile and custom file template
    if isIoT
        slConfigUISetVal(hDlg, hSrc, 'TemplateMakefile',     'ert_stm32_target_32.tmf');
        slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate','ert_iot_file_process.tlc');
    else
        slConfigUISetVal(hDlg, hSrc, 'TemplateMakefile',     'ert_linux_target_64.tmf');
        slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate','ert_linux_file_process.tlc');
    end

    % Gray out fields that do not apply to the selected target
    slConfigUISetEnabled(hDlg, hSrc, 'tlcIoTUpload',     isIoT);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcLinuxUpload',   ~isIoT);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcLinuxPriority', ~isIoT);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcXcpTcpAddress', ~isIoT);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcUploadPort',    ~isIoT);
    slConfigUISetEnabled(hDlg, hSrc, 'tlcXcpTcpPort',    ~isIoT);

    % Also try to hide them (requires MATLAB restart in 2023b to take effect)
    try
        slConfigUISetVisible(hDlg, hSrc, 'tlcIoTUpload',     isIoT);
        slConfigUISetVisible(hDlg, hSrc, 'tlcLinuxUpload',   ~isIoT);
        slConfigUISetVisible(hDlg, hSrc, 'tlcLinuxPriority', ~isIoT);
        slConfigUISetVisible(hDlg, hSrc, 'tlcXcpTcpAddress', ~isIoT);
        slConfigUISetVisible(hDlg, hSrc, 'tlcUploadPort',    ~isIoT);
        slConfigUISetVisible(hDlg, hSrc, 'tlcXcpTcpPort',    ~isIoT);
    catch
    end
end
