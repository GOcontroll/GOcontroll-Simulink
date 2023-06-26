%%***************************************************************************************
%% file         sfcn_gocontroll_output_module_10_mcb.m
%% brief        Level-2 M file S-Function for GOcontroll 10ch output module.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2023 (c) by GOcontroll     http://www.gocontroll.com    All rights reserved
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

function [moduleSlotInfoStr, connectorId, pin1Id, pin2Id, pin3Id, pin4Id, pin5Id, pin6Id,pin7Id, pin8Id, pin9Id, pin10Id] =...
									sfcn_gocontroll_output_module_10_mcb(moduleSlot,ContrType,...
C12freq,C1func,C2func,C34freq,C3func,C4func,C56freq,C5func,C6func,C78freq,C7func,C8func,C910freq,C9func,C10func,...
C1peakDuty,C1peakTime,C2peakDuty,C2peakTime,C3peakDuty,C3peakTime,C4peakDuty,C4peakTime,C5peakDuty,C5peakTime,...
C6peakDuty,C6peakTime,C7peakDuty,C7peakTime,C8peakDuty,C8peakTime,C9peakDuty,C9peakTime,C10peakDuty,C10peakTime)


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

if(ContrType == 1) % Moduline IV
	if(moduleSlot == 1 || moduleSlot == 3 || moduleSlot == 5 || moduleSlot == 7)
	pin1Id = 'Pin 13';
	pin2Id = 'Pin 19';
	pin3Id = 'Pin 12';
	pin4Id = 'Pin 18';
	pin5Id = 'Pin 11';
	pin6Id = 'Pin 17';
	pin7Id = 'Pin 24';
	pin8Id = 'Pin 6';
	pin9Id = 'Pin 5';
	pin10Id = 'Pin 4';
	else
	pin1Id = 'Pin 8';
	pin2Id = 'Pin 14';
	pin3Id = 'Pin 9';
	pin4Id = 'Pin 15';
	pin5Id = 'Pin 10';
	pin6Id = 'Pin 16';
	pin7Id = 'Pin 22';
	pin8Id = 'Pin 2';
	pin9Id = 'Pin 3';
	pin10Id = 'Pin 23';
	end
elseif(ContrType == 2) % Moduline mini
	if(moduleSlot == 1 || moduleSlot == 3)
	pin1Id = 'Pin 15';
	pin2Id = 'Pin 23';
	pin3Id = 'Pin 14';
	pin4Id = 'Pin 22';
	pin5Id = 'Pin 13';
	pin6Id = 'Pin 17';
	pin7Id = 'Pin 21';
	pin8Id = 'Pin 30';
	pin9Id = 'Pin 5';
	pin10Id = 'Pin 4';
	else
	pin1Id = 'Pin 10';
	pin2Id = 'Pin 18';
	pin3Id = 'Pin 11';
	pin4Id = 'Pin 19';
	pin5Id = 'Pin 12';
	pin6Id = 'Pin 20';
	pin7Id = 'Pin 28';
	pin8Id = 'Pin 2';
	pin9Id = 'Pin 3';
	pin10Id = 'Pin 29';
	end
elseif(ContrType == 3) % Moduline screen
	if(moduleSlot == 1)
	pin1Id = 'Pin 15';
	pin2Id = 'Pin 23';
	pin3Id = 'Pin 14';
	pin4Id = 'Pin 22';
	pin5Id = 'Pin 13';
	pin6Id = 'Pin 17';
	pin7Id = 'Pin 21';
	pin8Id = 'Pin 30';
	pin9Id = 'Pin 5';
	pin10Id = 'Pin 4';
	else
	pin1Id = 'Pin 10';
	pin2Id = 'Pin 18';
	pin3Id = 'Pin 11';
	pin4Id = 'Pin 19';
	pin5Id = 'Pin 12';
	pin6Id = 'Pin 20';
	pin7Id = 'Pin 28';
	pin8Id = 'Pin 2';
	pin9Id = 'Pin 3';
	pin10Id = 'Pin 29';
	end
end


% Create resource keywords to be reserved in resource database
modelRTWFields = 	struct('moduleSlot', int2str(moduleSlot),	'C12freq', int2str(C12freq),'C1func', int2str(C1func),'C2func', int2str(C2func),...
																'C34freq', int2str(C34freq),'C3func', int2str(C3func),'C4func', int2str(C4func),...
																'C56freq', int2str(C56freq),'C5func', int2str(C5func),'C6func', int2str(C6func),...
																'C78freq', int2str(C78freq),'C7func', int2str(C7func),'C8func', int2str(C8func),...
																'C910freq', int2str(C910freq),'C9func', int2str(C9func),'C10func', int2str(C10func),...
																'C1peakDuty', int2str(C1peakDuty),'C1peakTime', int2str(C1peakTime),...
																'C2peakDuty', int2str(C2peakDuty),'C2peakTime', int2str(C2peakTime),...
																'C3peakDuty', int2str(C3peakDuty),'C3peakTime', int2str(C3peakTime),...
																'C4peakDuty', int2str(C4peakDuty),'C4peakTime', int2str(C4peakTime),...
																'C5peakDuty', int2str(C5peakDuty),'C5peakTime', int2str(C5peakTime),...
																'C6peakDuty', int2str(C6peakDuty),'C6peakTime', int2str(C6peakTime),...
																'C7peakDuty', int2str(C7peakDuty),'C7peakTime', int2str(C7peakTime),...
																'C8peakDuty', int2str(C8peakDuty),'C8peakTime', int2str(C8peakTime),...
																'C9peakDuty', int2str(C9peakDuty),'C9peakTime', int2str(C9peakTime),...
																'C10peakDuty', int2str(C10peakDuty),'C10peakTime', int2str(C10peakTime));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_output_module_10_mcb.m ******************


