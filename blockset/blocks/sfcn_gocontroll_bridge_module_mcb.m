%%***************************************************************************************
%% file         sfcn_gocontroll_bridge_module_mcb.m
%% brief        Level-2 M file S-Function for GOcontroll bridge module.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2023 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
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

function [moduleSlotInfoStr, connectorId, pin1Id, pin2Id] = sfcn_gocontroll_bridge_module_mcb(moduleSlot,ContrType,C1func,C1freq,C2func,C2freq)



	% array with pinID infos
	moduleSlotInfoStrings = {  	'Module Slot 1',  ...\
								'Module Slot 2',  ...\
								'Module Slot 3',  ...\
								'Module Slot 4',  ...\
								'Module Slot 5',  ...\
								'Module Slot 6',  ...\
								'Module Slot 7',  ...\
								'Module Slot 8'};
	% construct the pinID info string
	moduleSlotInfoStr = moduleSlotInfoStrings{moduleSlot};


	if(moduleSlot == 1 || moduleSlot == 2)
		connectorId = 'A';
	elseif(moduleSlot == 3 || moduleSlot == 4)
		connectorId = 'B';
	elseif(moduleSlot == 5 || moduleSlot == 6)
		connectorId = 'D';
	else
		connectorId = 'E';
	end

	if (ContrType == 1) % Moduline IV
		if(moduleSlot == 1 || moduleSlot == 3 || moduleSlot == 5 || moduleSlot == 7)
			pin1Id = 'Pin 11,12,13';
			pin2Id = 'Pin 17,18,19';
		else
			pin1Id = 'Pin 8,9,10';
			pin2Id = 'Pin 14,15,16';
		end
	elseif(ContrType == 2 || ContrType == 3) % Moduline mini / Moduline display
		if(moduleSlot == 1 || moduleSlot == 3)
			pin1Id = 'Pin 13,14,15';
			pin2Id = 'Pin 21,22,23';
		else
			pin1Id = 'Pin 10,11,12';
			pin2Id = 'Pin 18,19,20';
		end
	end

	% Create resource keywords to be reserved in resource database
	modelRTWFields = 	struct('moduleSlot', int2str(moduleSlot),	'C1func', int2str(C1func),'C1freq', int2str(C1freq),...
																			'C2func', int2str(C2func),'C2freq', int2str(C2freq));

	% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
	HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
	set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_bridge_module_mcb.m ******************


