%%***************************************************************************************
%% file         sfcn_gocontroll_input_module_mcb.m
%% brief        Level-2 M file S-Function for GOcontroll input module.
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
function [moduleSlotInfoStr, connectorId, pin1Id, pin2Id, pin3Id, pin4Id, pin5Id, pin6Id] = sfcn_gocontroll_input_module_mcb(moduleSlot,ContrType,...
																		C1func,C1volt,C1afil,C1dpul,C1pu,C1pd,...
																		C2func,C2volt,C2afil,C2dpul,C2pu,C2pd,...
																		C3func,C3volt,C3afil,C3dpul,C3pu,C3pd,...
																		C4func,C4volt,C4afil,C4dpul,C4pu,C4pd,...
																		C5func,C5volt,C5afil,C5dpul,C5pu,C5pd,...
																		C6func,C6volt,C6afil,C6dpul,C6pu,C6pd,...
																		SensSup1,SensSup2,SensSup3)

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
	else
		pin1Id = 'Pin 8';
		pin2Id = 'Pin 14';
		pin3Id = 'Pin 9';
		pin4Id = 'Pin 15';
		pin5Id = 'Pin 10';
		pin6Id = 'Pin 16';
	end
elseif(ContrType == 2 || ContrType == 3) % Moduline mini / Moduline display
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
modelRTWFields = struct('moduleSlot', int2str(moduleSlot),	'C1func', int2str(C1func),'C1volt', int2str(C1volt), 'C1afil', int2str(C1afil),'C1dpul', int2str(C1dpul),'C1pu', int2str(C1pu),'C1pd', int2str(C1pd),...
															'C2func', int2str(C2func),'C2volt', int2str(C2volt), 'C2afil', int2str(C2afil),'C2dpul', int2str(C2dpul),'C2pu', int2str(C2pu),'C2pd', int2str(C2pd),...
															'C3func', int2str(C3func),'C3volt', int2str(C3volt), 'C3afil', int2str(C3afil),'C3dpul', int2str(C3dpul),'C3pu', int2str(C3pu),'C3pd', int2str(C3pd),...
															'C4func', int2str(C4func),'C4volt', int2str(C4volt), 'C4afil', int2str(C4afil),'C4dpul', int2str(C4dpul),'C4pu', int2str(C4pu),'C4pd', int2str(C4pd),...
															'C5func', int2str(C5func),'C5volt', int2str(C5volt), 'C5afil', int2str(C5afil),'C5dpul', int2str(C5dpul),'C5pu', int2str(C5pu),'C5pd', int2str(C5pd),...
															'C6func', int2str(C6func),'C6volt', int2str(C6volt), 'C6afil', int2str(C6afil),'C6dpul', int2str(C6dpul),'C6pu', int2str(C6pu),'C6pd', int2str(C6pd),...
															'SensSup1', int2str(SensSup1),'SensSup2', int2str(SensSup2), 'SensSup3', int2str(SensSup3));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_input_module.m ******************


