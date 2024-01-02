%%***************************************************************************************
%% file         sfcn_gocontroll_input_module.m
%% brief        Block mask initialization function.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
%%
%%---------------------------------------------------------------------------------------
%%                            L I C E N S E
%%---------------------------------------------------------------------------------------
%% Permission is hereby granted, free of charge, to any person obtaining a copy of this
%% software and associated documentation files (the "Software"), to deal in the Software
%% without restriction, including without limitation the rights to use, copy, modify, merge,
%% publish, distribute, sublicense, and/or sell copies of the Software, and to permit
%% persons to whom the Software is furnished to do so, subject to the following conditions:
%%
%% The above copyright notice and this permission notice shall be included in all copies or
%% substantial portions of the Software.
%%
%% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
%% INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
%% PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
%% FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
%% OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
%% DEALINGS IN THE SOFTWARE.
%%
%%***************************************************************************************
function [moduleSlotInfoStr, inputChannelInfoStr] = sfcn_gocontroll_input_module_reset_mcb(moduleSlot,contrType,inputChannel)



  % array with module slot infos
moduleSlotInfoStrings = {'Module Slot 1',  ...\
						 'Module Slot 2',  ...\
						 'Module Slot 3',  ...\
						 'Module Slot 4',  ...\
						 'Module Slot 5',  ...\
						 'Module Slot 6',  ...\
						 'Module Slot 7',  ...\
						 'Module Slot 8'};

channelIDInfoStrings = {	'Input Channel 1',  ...\
							'Input Channel 2',  ...\
							'Input Channel 3',  ...\
							'Input Channel 4',  ...\
							'Input Channel 5',  ...\
							'Input Channel 6',  ...\
							'Input Channel 7',  ...\
							'Input Channel 8',  ...\
							'Input Channel 9',  ...\
							'Input Channel 10'};



  % construct the pinID info string
 moduleSlotInfoStr = moduleSlotInfoStrings{moduleSlot};
 inputChannelInfoStr  = channelIDInfoStrings{inputChannel};



% Create resource keywords to be reserved in resource database
modelRTWFields = struct('moduleSlot', int2str(moduleSlot),'contrType', int2str(contrType),	'inputChannel', int2str(inputChannel));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_input_module.m ******************


