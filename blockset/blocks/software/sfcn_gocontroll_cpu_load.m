%%***************************************************************************************
%% file         sfcn_gocontroll_cpu_load.m
%% brief        Level-2 M file S-Function that reads the current CPU load as a percentage
%%              (0–100). STM only - returns 0 in simulation and on Linux.
%%              The value is computed as 100% minus the FreeRTOS IDLE task share over
%%              the last measurement interval (~1 s).
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2025 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
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
function sfcn_gocontroll_cpu_load(block)
	setup(block);
end

function setup(block)
	block.NumInputPorts  = 0;
	block.NumOutputPorts = 1;

	cpuOut = 1;
	addSimpleOutput(block, cpuOut, DatatypeID.Uint8);

	tsamp = 1;
	block.NumDialogPrms = 1;
	block.SampleTimes = [block.DialogPrm(tsamp).Data 0];

	block.RegBlockMethod('Start',   @Start);
	block.RegBlockMethod('Outputs', @Outputs);
	block.RegBlockMethod('Update',  @Update);
end

function Start(~)
	%% No initialisation needed
end

function Outputs(~)
	%% No simulation output - handled by TLC
end

function Update(~)
	%% No state update
end

%%******************************* end of sfcn_gocontroll_cpu_load.m ******************
