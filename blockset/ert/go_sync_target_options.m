function go_sync_target_options(mdl)
%%***************************************************************************************
%% file         go_sync_target_options.m
%% brief        Makes the GOcontroll options of a model match its Target, for a Target
%%              that was set from a script.
%%
%%              Choosing the Target in the Configuration Parameters dialog runs
%%              ert_gocontroll_target_changed_callback_handler, which greys out the
%%              Linux-only options for the Moduline S1 and selects the template
%%              makefile and file template of the platform. set_param does not run
%%              that callback, so a model whose Target was set from a script keeps
%%              the state of the previous Target. This function applies the same
%%              rules without the dialog.
%%
%% param        mdl     model name (default: bdroot)
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    if nargin < 1 || isempty(mdl)
        mdl = bdroot;
    end
    cs = getActiveConfigSet(mdl);
    isStm = strcmp(go_target_platform(get_param(cs, 'tlcGOcontrollTarget')), 'STM');

    % Linux-specific options (greyed out when the Moduline S1 is selected)
    linuxOnly = {'tlcLinuxPriority', 'tlcXcpTcpAddress', 'tlcXcpTcpPort'};
    for k = 1:numel(linuxOnly)
        cs.setPropEnabled(linuxOnly{k}, ~isStm);
    end

    % Word sizes of the platform (Hardware Implementation pane)
    go_target_hardware(mdl);

    if isStm
        set_param(cs, 'ERTCustomFileTemplate', 'ert_stm_file_process.tlc', ...
                      'TemplateMakefile',      'ert_stm_target_32.tmf');
    else
        set_param(cs, 'ERTCustomFileTemplate', 'ert_linux_file_process.tlc', ...
                      'TemplateMakefile',      'ert_linux_target_64.tmf');
    end
end
