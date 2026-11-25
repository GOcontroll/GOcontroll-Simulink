function [t, can] = go_can_bus_setup(blk)
%%***************************************************************************************
%% file         go_can_bus_setup.m
%% brief        Mask helper of the CAN Receive-msg and CAN Send-msg blocks.
%%
%%              - Mirrors the model's Target into the read-only ContrType field.
%%              - Fills the CAN channel list with the buses of that controller and, on
%%                the Linux controllers, the three CAN buses of a Multibus Module.
%%              - Returns what the code generation needs.
%%
%%              Whether a Multibus Module is fitted is not known when the model is
%%              built, so the block states it. A Multibus Module adds up to three
%%              CAN (FD) buses, which Linux names mb_can1..3 (go-multibus). A bus that
%%              is not there when the application starts is skipped by the generated
%%              code (see InitCanSocket in can_common.tlc).
%%
%%              Only the buses of one Multibus Module can be chosen for now. A second
%%              module is planned as mb_can4..6; until then a controller with two
%%              Multibus Modules, where go-multibus names the buses mb<slot>_canN, is
%%              not served by these blocks.
%%
%%              Called from the mask initialization and from the callback of the
%%              canBus parameter: a linked block only accepts changes to its mask
%%              parameters from there.
%%
%% param        blk     block path or handle (gcb)
%% return       t       canonical target name, or 'Unknown controller'
%% return       can     struct of char: canBus (bus number of the controller),
%%                      key (name of the bus in the generated code), ifname (Linux
%%                      interface), label (text for the block icon)
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    try
        t = go_target_name(get_param(getActiveConfigSet(bdroot(blk)), 'tlcGOcontrollTarget'));
    catch
        t = 'Unknown controller';   % library, or a model without the GOcontroll target
    end
    t = char(t);

    msk = Simulink.Mask.get(blk);
    c   = msk.getParameter('ContrType');
    cb  = msk.getParameter('canBus');

    if ~strcmp(c.Value, t)
        c.Value = t;
    end
    isS1 = strcmp(t, 'Moduline S1');

    % ----- CAN channels: the buses of the controller, then those of a Multibus Module ---
    switch t
        case 'Moduline S1'
            channels = {'CAN1 - Connector pin 16 (L) and pin 17 (H)', ...
                        'CAN2 - Connector pin 24 (L) and pin 25 (H)'};
        case 'Moduline M1'
            channels = {'CAN1 - Connector B pin 24 (L) and pin 25 (H)', ...
                        'CAN2 - Connector A pin 24 (L) and pin 25 (H)'};
        case 'Moduline HMI1'
            channels = {'CAN1 - Connector A pin 16 (L) and pin 17 (H)', ...
                        'CAN2 - Connector A pin 24 (L) and pin 25 (H)'};
        otherwise   % Moduline L4, and the library
            channels = {'CAN1 - Connector C pin 14 (L) and pin 15 (H)', ...
                        'CAN2 - Connector C pin 20 (L) and pin 21 (H)', ...
                        'CAN3 - Connector C pin 19 (L) and pin 18 (H)', ...
                        'CAN4 - Connector C pin 13 (L) and pin 12 (H)'};
    end
    if ~isS1
        channels = [channels, {'Multibus CAN 1 - connector interface 1 (mb_can1)', ...
                               'Multibus CAN 2 - connector interface 3 (mb_can2)', ...
                               'Multibus CAN 3 - connector interface 4 (mb_can3)'}];
    end
    chosen = get_param(blk, 'canBus');
    useMultibus = strncmp(chosen, 'Multibus CAN', 12);
    if isS1 && useMultibus
        error('go_can_bus_setup:multibusOnS1', ...
              ['%s: a CAN bus of a Multibus Module is selected, but this block supports the ' ...
               'Multibus Module on the Linux controllers (Moduline M1, L4, HMI1) only. ' ...
               'Choose a CAN channel of the controller.'], getfullname(blk));
    end
    if ~isequal(cb.TypeOptions(:), channels(:))
        cb.TypeOptions = channels;
    end
    bus = sscanf(chosen, 'CAN%d', 1);
    if isempty(bus)
        bus = 1;
    end

    % ----- what the code generation needs ----------------------------------------------
    can.canBus = int2str(bus);
    if useMultibus
        mbBus = sscanf(chosen, 'Multibus CAN %d', 1);
        can.ifname = sprintf('mb_can%d', mbBus);
        can.label  = sprintf('Multibus CAN %d', mbBus);
        can.key = can.ifname;
    else
        can.ifname = sprintf('can%d', bus - 1);
        can.key    = int2str(bus);
        can.label  = sprintf('CAN %d', bus);
    end
end
