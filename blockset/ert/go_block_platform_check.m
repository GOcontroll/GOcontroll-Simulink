function go_block_platform_check(blk, platform, mode)
%%***************************************************************************************
%% file         go_block_platform_check.m
%% brief        Errors when a block is used in a model for a platform it does not
%%              support. Called first in the mask initialization of every block that
%%              exists for one platform only, so the block reports itself when it is
%%              placed and when the model is updated or built.
%%
%%              The same rule is checked during code generation by RequirePlatform in
%%              go_platform.tlc, for a model whose masks were not run again.
%%
%% param        blk         block path (gcb)
%% param        platform    'Linux' (Moduline M1, L4, HMI1) or 'STM' (Moduline S1)
%% param        mode        optional. 'notify': show the message in a dialog instead of
%%                          raising an error. Used from the CopyFcn of the block, which
%%                          runs when the block is dragged into a model: an error in
%%                          the mask initialization alone is not shown at that moment.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    mdl = bdroot(blk);
    if ~strcmp(get_param(mdl, 'BlockDiagramType'), 'model')
        return                      % in the library itself
    end
    try
        target = get_param(getActiveConfigSet(mdl), 'tlcGOcontrollTarget');
        actual = go_target_platform(target);
    catch
        return                      % not a model for a GOcontroll target
    end
    if strcmp(actual, platform)
        return
    end

    names = struct('Linux', 'the Linux controllers (Moduline M1, L4, HMI1)', ...
                   'STM',   'the Moduline S1');
    name = get_param(blk, 'MaskType');
    if isempty(name)
        name = strrep(get_param(blk, 'Name'), newline, ' ');
    end
    msg = sprintf(['The block "%s" is for %s only. This model is set to the %s ' ...
         '(Configuration Parameters > Code Generation > GOcontroll options). ' ...
         'Remove the block or use a model for the other platform.'], ...
        name, names.(platform), go_target_name(target));
    if nargin > 2 && strcmp(mode, 'notify')
        if usejava('desktop')
            errordlg(msg, 'GOcontroll blockset', 'modal');
        else
            disp(['GOcontroll blockset: ' msg]);
        end
        return
    end
    error('go_block_platform_check:wrongPlatform', '%s', msg);
end
