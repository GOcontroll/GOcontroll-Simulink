%%***************************************************************************************
%% file         sfcn_gocontroll_input_module_10_mcb.m
%% brief        Level-2 M file S-Function for GOcontroll input module.
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
function [moduleSlotInfoStr, connectorId, pin1Id, pin2Id, pin3Id, pin4Id, pin5Id, pin6Id, pin7Id, pin8Id, pin9Id, pin10Id] = sfcn_gocontroll_input_module_10_mcb(moduleSlot,ContrType,...
																		C1func,C1pu,C1pd,...
																		C2func,C2pu,C2pd,...
																		C3func,C3pu,C3pd,...
																		C4func,C4pu,C4pd,...
																		C5func,C5pu,C5pd,...
																		C6func,C6pu,C6pd,...
																		C7func,C7pu,C7pd,...
																		C8func,C8pu,C8pd,...
																		C9func,C9pu,C9pd,...
																		C10func,C10pu,C10pd,...
																		SensSup1)

  % array with module slot infos
  moduleSlotInfoStrings = {  	'Module slot 1',  ...\
								'Module slot 2',  ...\
								'Module slot 3',  ...\
								'Module slot 4',  ...\
								'Module slot 5',  ...\
								'Module slot 6',  ...\
								'Module slot 7',  ...\
								'Module slot 8'};

  % construct the module slot info string
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
elseif(ContrType == 2 || ContrType == 3) % Moduline mini / Moduline display
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
end

% Create resource keywords to be reserved in resource database
modelRTWFields = struct('moduleSlot', int2str(moduleSlot),	'C1func',int2str(C1func),'C1pu', int2str(C1pu),'C1pd', int2str(C1pd),...
															'C2func', int2str(C2func),'C2pu', int2str(C2pu),'C2pd', int2str(C2pd),...
															'C3func', int2str(C3func),'C3pu', int2str(C3pu),'C3pd', int2str(C3pd),...
															'C4func', int2str(C4func),'C4pu', int2str(C4pu),'C4pd', int2str(C4pd),...
															'C5func', int2str(C5func),'C5pu', int2str(C5pu),'C5pd', int2str(C5pd),...
															'C6func', int2str(C6func),'C6pu', int2str(C6pu),'C6pd', int2str(C6pd),...
															'C7func', int2str(C7func),'C7pu', int2str(C7pu),'C7pd', int2str(C7pd),...
															'C8func', int2str(C8func),'C8pu', int2str(C8pu),'C8pd', int2str(C8pd),...
															'C9func', int2str(C9func),'C9pu', int2str(C9pu),'C9pd', int2str(C9pd),...
															'C10func', int2str(C10func),'C10pu', int2str(C10pu),'C10pd', int2str(C10pd),...
															'SensSup1', int2str(SensSup1));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_input_module_10_mcb.m ******************


