%%***************************************************************************************
%% file         sfcn_can_receive_mcb.m
%% brief        Block mask initialization function.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2013 (c) by HAN Automotive     http://www.han.nl     All rights reserved
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
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
    error('The CAN message supports only up to 8 data bytes');
end

% Create resource keywords to be reserved in resource database
modelRTWFields = struct('canBus', int2str(canBus-1),...
    'outputNumber', int2str(outputNumber), 'dataType', int2str(dataType), 'byteOrder', int2str(byteOrder),...
    'dataLength', int2str(dataLength));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_can_receive_mcb.m *************************


