%%***************************************************************************************
%% file         sfcn_can_receive_mcb.m
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
function [] = sfcn_can_receive_mcb(canBus, outputNumber, dataType, byteOrder)

%Our data type parameters:
% int8
% uint8
% int16
% uint16
% int32
% uint32
% boolean

% calculate the data length (targe specific)
dataLength = 0;
switch(dataType)
    case {1,2,7}
        % data length of 1 byte
        dataLength = outputNumber * 1;
    case {3,4}
        % data length of 2 bytes
        dataLength = outputNumber * 2;
    case {5,6}
        % data length of 4 bytes
        dataLength = outputNumber * 4;
end

% check if number of outputs, combined with the data type, is exceeding the
% maximum data length
if (dataLength < 1)
    error ('The CAN message has a minimum of 1 byte');
elseif (dataLength > 8)
    error ('The CAN message supports only up to 8 data bytes');
end


% The number of inputs needs to match the amount of outputs the user did set

% Before we are able to change the block, we have to de-activate the link
% of this block to the library.
% So check if the link status is active:
if(~strcmp(get_param(gcb,'LinkStatus'),'inactive'))
    % De-activate the library link
	set_param(gcb,'LinkStatus','inactive')
end

% Get current number of outputs & inputs ('gcb' is a system defined var that points
% to the tree structure of the current block)
currentOutputNumber = length (find_system (gcb,'SearchDepth', 1, 'LookUnderMasks', 'on', 'BlockType', 'Outport'));

% Now we update the number of outputs to match the given amount

% If we have to many outputs (-1 for the 'new data' port):
if currentOutputNumber - 1 > outputNumber
    % Open the default library (invisible) for use
    load_system('built-in')
    % Loop trough all outputs that are to much
    for counterRemove = outputNumber+2:currentOutputNumber
        % Get the position of the output block
        thisPosition = get_param ([gcb, '/Out', num2str(counterRemove)], 'Position');
        % Remove the output block
        delete_block ([gcb, '/Out', num2str(counterRemove)])
        % Add a terminator block
        add_block ('built-in/Terminator', [gcb, '/Term', num2str(counterRemove)])
        % Align the new terminator block to be placed at the position of the
        % removed output
        set_param ([gcb, '/Term', num2str(counterRemove)], 'Position', thisPosition)
    end
% Or if we have to few inputs (-1 for the 'new data' port):
elseif outputNumber > currentOutputNumber - 1
    % Open the default library (invisible) for use
    load_system('built-in')
    % Loop trough all terminators that needs to be outputs
    for counterAdd = currentOutputNumber+1:outputNumber+1
        % Get the position of the terminator block
        thisPosition = get_param ([gcb, '/Term', num2str(counterAdd)], 'Position');
        % Remove the terminator block
        delete_block ([gcb, '/Term', num2str(counterAdd)])
        % Add a output block
        add_block ('built-in/Outport', [gcb, '/Out', num2str(counterAdd)])
        % Align the new output block to be placed at the position of the
        % removed terminator
        set_param ([gcb, '/Out', num2str(counterAdd)], 'Position', thisPosition)
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


% The outputs are in line with the user input now.

% Create resource keywords to be reserved in resource database
modelRTWFields = struct('canBus', int2str(canBus-1),...
    'outputNumber', int2str(outputNumber), 'dataType', int2str(dataType), 'byteOrder', int2str(byteOrder),...
    'dataLength', int2str(dataLength));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_can_receive_mcb.m *************************


