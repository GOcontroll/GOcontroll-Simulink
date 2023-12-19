%%***************************************************************************************
%% file         sfcn_gocontroll_output_module_mcb.m
%% brief        Level-2 M file S-Function for GOcontroll output module.
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

function [moduleSlotInfoStr, connectorId, pin1Id, pin2Id, pin3Id, pin4Id, pin5Id, pin6Id] = sfcn_gocontroll_output_module_mcb(moduleSlot,ContrType,...
C12freq,C1func,C2func,C34freq,C3func,C4func,C56freq,C5func,C6func,C1cur,C2cur,C3cur,C4cur,C5cur,C6cur,...
C1peakCurrent,C1peakTime,C2peakCurrent,C2peakTime,C3peakCurrent,C3peakTime,C4peakCurrent,C4peakTime,C5peakCurrent,C5peakTime,C6peakCurrent,C6peakTime)


  % array with pinID infos
  moduleSlotInfoStrings = {  	'Module slot 1',  ...\
								'Module slot 2',  ...\
								'Module slot 3',  ...\
								'Module slot 4',  ...\
								'Module slot 5',  ...\
								'Module slot 6',  ...\
								'Module slot 7',  ...\
								'Module slot 8'};
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


if(ContrType == 1) % Moduline IV
	if(moduleSlot == 1 || moduleSlot == 3 || moduleSlot == 5 || moduleSlot == 7)
	pin1Id = 'Pin 13';
	pin2Id = 'Pin 19';
	pin3Id = 'Pin 12';
	pin4Id = 'Pin 18';
	pin5Id = 'Pin 11';
	pin6Id = 'Pin 17';
	else
	pin1Id = 'Pin 8';
	pin2Id = 'Pin 14';
	pin3Id = 'Pin 9';
	pin4Id = 'Pin 15';
	pin5Id = 'Pin 10';
	pin6Id = 'Pin 16';
	end
elseif(ContrType == 2 || ContrType == 3) % Moduline mini / Moduline Display
	if(moduleSlot == 1 || moduleSlot == 3)
	pin1Id = 'Pin 15';
	pin2Id = 'Pin 23';
	pin3Id = 'Pin 14';
	pin4Id = 'Pin 22';
	pin5Id = 'Pin 13';
	pin6Id = 'Pin 21';
	else
	pin1Id = 'Pin 10';
	pin2Id = 'Pin 18';
	pin3Id = 'Pin 11';
	pin4Id = 'Pin 19';
	pin5Id = 'Pin 12';
	pin6Id = 'Pin 20';
	end
end


% Create resource keywords to be reserved in resource database
modelRTWFields = 	struct('moduleSlot', int2str(moduleSlot),	'C12freq', int2str(C12freq),...
																		'C1func', int2str(C1func),'C2func', int2str(C2func),...
																		'C34freq', int2str(C34freq),...
																		'C3func', int2str(C3func),'C4func', int2str(C4func),...
																		'C56freq', int2str(C56freq),...
																		'C5func', int2str(C5func),'C6func', int2str(C6func),...
																		'C1cur', int2str(C1cur),'C2cur', int2str(C2cur),...
																		'C3cur', int2str(C3cur),'C4cur', int2str(C4cur),...
																		'C5cur', int2str(C5cur),'C6cur', int2str(C6cur),...
																		'C1peakCurrent', int2str(C1peakCurrent),'C1peakTime', int2str(C1peakTime),...
																		'C2peakCurrent', int2str(C2peakCurrent),'C2peakTime', int2str(C2peakTime),...
																		'C3peakCurrent', int2str(C3peakCurrent),'C3peakTime', int2str(C3peakTime),...
																		'C4peakCurrent', int2str(C4peakCurrent),'C4peakTime', int2str(C4peakTime),...
																		'C5peakCurrent', int2str(C5peakCurrent),'C5peakTime', int2str(C5peakTime),...
																		'C6peakCurrent', int2str(C6peakCurrent),'C6peakTime', int2str(C6peakTime));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_output_module_mcb.m ******************


