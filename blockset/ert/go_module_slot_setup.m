function t = go_module_slot_setup(blk, slotParams)
%%***************************************************************************************
%% file         go_module_slot_setup.m
%% brief        Mask-initialization helper for the module blocks.
%%
%%              - Mirrors the model's Target (tlcGOcontrollTarget) into the read-only
%%                ContrType field, so the block follows Configuration Parameters.
%%              - Limits the slot popup(s) to the slots that controller has
%%                (go_module_slots).
%%              - Errors when the selected slot does not exist on that controller,
%%                instead of letting Simulink silently fall back to slot 1.
%%
%%              Must be called from the block's mask initialization: a linked block
%%              only accepts TypeOptions changes from its own mask callbacks, not
%%              from a script.
%%
%% param        blk         block path or handle (gcb)
%% param        slotParams  cellstr of slot popup parameters (default {'moduleSlot'})
%% return       t           canonical target name, or 'Unknown controller'
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    if nargin < 2
        slotParams = {'moduleSlot'};
    end

    try
        t = go_target_name(get_param(getActiveConfigSet(bdroot(blk)), 'tlcGOcontrollTarget'));
    catch
        t = 'Unknown controller';   % library, or a model without the GOcontroll target
    end

    msk = Simulink.Mask.get(blk);
    c = msk.getParameter('ContrType');
    if isempty(c)
        c = msk.getParameter('contrType');   % Reset Pulse Counter spells it this way
    end
    if ~isempty(c) && ~strcmp(c.Value, t)
        c.Value = t;
    end

    n = go_module_slots(t);
    options = arrayfun(@(k) sprintf('Moduleslot %d', k), 1:n, 'UniformOutput', false);

    for k = 1:numel(slotParams)
        p = msk.getParameter(slotParams{k});
        slot = sscanf(get_param(blk, slotParams{k}), 'Moduleslot %d');
        if ~isempty(slot) && slot > n
            error('go_module_slot_setup:slotOutOfRange', ...
                  ['%s: ''Moduleslot %d'' is selected, but the %s has only %d module ' ...
                   'slots. Select a slot between 1 and %d.'], ...
                  getfullname(blk), slot, t, n, n);
        end
        if ~isequal(p.TypeOptions(:), options(:))
            p.TypeOptions = options;
        end
    end
end
