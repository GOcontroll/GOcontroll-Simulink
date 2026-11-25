%%***************************************************************************************
%% file         sfcn_gocontroll_bridge_module_monitor.m
%% brief        Level-2 M file S-Function for GOcontroll bridge module.
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
function sfcn_gocontroll_bridge_module_monitor(block)
	setup(block);
end

%% Function: setup ===================================================
%% Abstract:
%%   Set up the S-function block's basic characteristics such as:
%%   - Input ports
%%   - Output ports
%%   - Dialog parameters
%%   - Options
%%
%%   Required         : Yes
%%   C-Mex counterpart: mdlInitializeSizes

function setup(block)
	%% Register number of input and output ports
	block.NumInputPorts = 0;
	block.NumOutputPorts = 4;

	moduleTemp = 1;
	addSimpleOutput(block, moduleTemp, DatatypeID.Int16);

	moduleGnd = 2;
	addSimpleOutput(block, moduleGnd, DatatypeID.Uint16);

	channel1 = 3;
	addSimpleOutput(block, channel1, DatatypeID.Int16);

	channel2 = 4;
	addSimpleOutput(block, channel2, DatatypeID.Int16);

	% Number of S-Function parameters expected
	tsamp = 1;

	block.NumDialogPrms     = 1;
	block.SampleTimes = [block.DialogPrm(tsamp).Data 0];

	%% -----------------------------------------------------------------
	%% Register methods called at run-time
	%% -----------------------------------------------------------------
	
	block.RegBlockMethod('Start', @Start);

	block.RegBlockMethod('Outputs', @Outputs);

	block.RegBlockMethod('Update', @Update);
end

function Start(~)
end

function Outputs(~)
end

function Update(~)
end

%%******************************* end of sfcn_gocontroll_bridge_module_monitor.m **********************