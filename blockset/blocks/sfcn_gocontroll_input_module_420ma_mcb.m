%%***************************************************************************************
%% file         sfcn_gocontroll_input_module_420ma_mcb.m
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
function [moduleSlotInfoStr, connectorId, pin1Id, pin2Id, pin3Id, pin4Id, pin5Id, pin6Id, pin7Id, pin8Id, pin9Id, pin10Id] = sfcn_gocontroll_input_module_420ma_mcb(moduleSlot,ContrType,...
																		Sup16va)

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
		pin1Id = 'Pin 7';
		pin2Id = 'Pin 13';
		pin3Id = 'Pin 19';
		pin4Id = 'Pin 6';
		pin5Id = 'Pin 12';
		pin6Id = 'Pin 18';
		pin7Id = 'Pin 5';
		pin8Id = 'Pin 11';
		pin9Id = 'Pin 17';
		pin10Id = 'Pin 4';
	else
		pin1Id = 'Pin 1';
		pin2Id = 'Pin 8';
		pin3Id = 'Pin 14';
		pin4Id = 'Pin 2';
		pin5Id = 'Pin 9';
		pin6Id = 'Pin 15';
		pin7Id = 'Pin 3';
		pin8Id = 'Pin 10';
		pin9Id = 'Pin 16';
		pin10Id = 'Pin 23';
	end
elseif(ContrType == 2 || ContrType == 3) % Moduline mini / Moduline display
	if(moduleSlot == 1 || moduleSlot == 3)
		pin1Id = 'Pin 7';
		pin2Id = 'Pin 15';
		pin3Id = 'Pin 23';
		pin4Id = 'Pin 6';
		pin5Id = 'Pin 14';
		pin6Id = 'Pin 22';
		pin7Id = 'Pin 5';
		pin8Id = 'Pin 13';
		pin9Id = 'Pin 21';
		pin10Id = 'Pin 4';
	else
		pin1Id = 'Pin 1';
		pin2Id = 'Pin 10';
		pin3Id = 'Pin 18';
		pin4Id = 'Pin 2';
		pin5Id = 'Pin 11';
		pin6Id = 'Pin 19';
		pin7Id = 'Pin 3';
		pin8Id = 'Pin 12';
		pin9Id = 'Pin 20';
		pin10Id = 'Pin 29';
	end
end

% Create resource keywords to be reserved in resource database
modelRTWFields = struct('moduleSlot', int2str(moduleSlot),...
						'Sup16va', int2str(Sup16va));

% Insert modelRTWFields in the I/O block S-Function containing the Tag starting with 'HANcoder_TARGET_'
HANcoder_TARGET_DataBlock = find_system(gcb, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'BlockType', 'M-S-Function');
set_param(HANcoder_TARGET_DataBlock{1}, 'RTWdata', modelRTWFields);


%%******************************* end of sfcn_gocontroll_input_module_420ma_mcb.m ******************

