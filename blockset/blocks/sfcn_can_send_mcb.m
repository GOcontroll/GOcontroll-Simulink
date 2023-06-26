%%***************************************************************************************
%% file         sfcn_can_send_mcb.m
%% brief        Block mask initialization function.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2013 (c) by HAN Automotive     http://www.han.nl     All rights reserved
%%  Copyright 2023 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
%%
%%---------------------------------------------------------------------------------------
%%                            L I C E N S E
%%---------------------------------------------------------------------------------------
%% This file is part of the HANcoder Target Matlab/Simulink Blockset environment. For the
%% licensing terms, please contact HAN Automotive.
%%
%% This software has been carefully tested, but is not guaranteed for any particular
%% purpose. HAN Automotive does not offer any warranties and does not guarantee the
%% accuracy, adequacy, or completeness of the software and is not responsible for any
%% errors or omissions or the results obtained from use of the software.
%%
%%***************************************************************************************
function [] = sfcn_can_send_mcb(canBus, frameType, inputNumber, dataType, byteOrder, RTR)

%Our data type parameters:
% int8
% uint8
% int16
% uint16
% int32
% uint32
% boolean

% calculate the data length (target specific)
dataLength = 0;
switch(dataType)
    case {1,2,7}
        % data length of 1 byte
        dataLength = inputNumber * 1;
    case {3,4}
        % data length of 2 bytes
        dataLength = inputNumber * 2;
    case {5,6}
        % data length of 4 bytes
        dataLength = inputNumber * 4;
end

% check if number of inputs, combined with the data type, is exceeding the
% maximum data length
if (dataLength > 8)
    error ('The CAN message supports only up to 8 data bytes');
end


% The number of inputs needs to match the amount of inputs the user did set

% Before we are able to change the block, we have to de-activate the link
% of this block to the library.
% So check if the link status is active:
if(~strcmp(get_param(gcb,'LinkStatus'),'inactive'))
    % De-activate the library link
	set_param(gcb,'LinkStatus','inactive')
end

% Get current number of inputs ('gcb' is a system defined var that points
% to the tree structure of the current block)
currentInputNumber = length (find_system (gcb, 'LookUnderMasks', 'on', 'BlockType', 'Inport'));

% Now we update the number of inputs to match the given amount

% If we have to many inputs:
if inputNumber < currentInputNumber - 1
    % Open the default library (invisible) for use
    load_system('built-in')
    % Loop trough all inputs that are to much
    for counterRemove = inputNumber+2:currentInputNumber
        % Get the position of the input block
        thisPosition = get_param ([gcb, '/In', num2str(counterRemove)], 'Position');
        % Remove the input block
        delete_block ([gcb, '/In', num2str(counterRemove)])
        % Add a ground block
        add_block ('built-in/Ground', [gcb, '/Gnd', num2str(counterRemove)])
        % Alligh the new ground block to be placed at the position of the
        % removed input
        set_param ([gcb, '/Gnd', num2str(counterRemove)], 'Position', thisPosition)
    end
% Or if we have to few inputs:
elseif inputNumber > currentInputNumber - 1
    % Open the default library (invisible) for use
    load_system('built-in')
    % Loop trough all Grounds that needs to be inputs
    for counterAdd = currentInputNumber+1:inputNumber+1
        % Get the position of the ground block
        thisPosition = get_param ([gcb, '/Gnd', num2str(counterAdd)], 'Position');
        % Remove the ground block
        delete_block ([gcb, '/Gnd', num2str(counterAdd)])
        % Add a input block
        add_block ('built-in/Inport', [gcb, '/In', num2str(counterAdd)])
        % Alligh the new input block to be placed at the position of the
        % removed ground
        set_param ([gcb, '/In', num2str(counterAdd)], 'Position', thisPosition)
    end
end

% NOTE: I had some problems with this dynamic input system, because the
% S-Function updates at a later time. The problem with that, is that you
% cannot draw a line to the S-Function if you just added a new port. And on
% top of that, the line is not drawn, resulting in an error when you attemp
% to remove the line when you lower the amounts of ports.
% The solution to this problem is to have the S-Function set his amount of
% input ports static to 8, and add Grounds in the model to close those
% inputs. That way you do not have the S-Function to update its amount of
% ports.


% The inputs are in line with the user input now.

% Create resource keywords to be reserved in resource database
modelRTWFields = struct('canBus', int2str(canBus-1), 'frameType', int2str(frameType),...
    'inputNumber', int2str(inputNumber), 'dataType', int2str(dataType), 'byteOrder', int2str(byteOrder),...
    'dataLength', int2str(dataLength), 'RTR', int2str(RTR));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_can_send_mcb.m ****************************