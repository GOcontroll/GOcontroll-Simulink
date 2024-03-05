%%***************************************************************************************
%% file         sfcn_lin_master.m
%% brief        Level-2 M file S-Function for LIN message transmission
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
%% endinternal
%%
%%***************************************************************************************
function sfcn_lin_master(block)
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
	tsamp = 1;
	id = 2;
	direction = 3;
	dataLength = 4;
	check = 5;

	switch block.DialogPrm(dataLength).Data
		case 1
			ports = 2;
		case 2
			ports = 4;
		case 3
			ports = 8;
		otherwise
			error('Incorrect data length setting');
	end

	if block.DialogPrm(direction).Data == 1
		block.NumOutputPorts = ports;
		block.NumInputPorts = 0;
		for outputCounter = 1:ports
			block.OutputPort(outputCounter).Dimensions = 1;
			block.OutputPort(outputCounter).DatatypeID = 3; %% 3 = uint8
			block.OutputPort(outputCounter).Complexity = 'Real';
			block.OutputPort(outputCounter).SamplingMode = 'sample';
		end
	else
		block.NumOutputPorts = 0;
		block.NumInputPorts = ports;
		for inputCounter = 1:ports
			block.InputPort(inputCounter).Dimensions = 1;
			block.InputPort(inputCounter).DatatypeID = 3; %% 3 = uint8
			block.InputPort(inputCounter).Complexity = 'Real';
			block.InputPort(inputCounter).DirectFeedthrough = false;  %% We will not use the direct (.Data) value of the input to calculate the direct (.Data) value of the output
			block.InputPort(inputCounter).SamplingMode = 'sample';
		end
	end

	% Number of S-Function parameters expected
	% (tsamp, canBus, frameType, inputNumber, dataType, byteOrder)
	block.NumDialogPrms     = 5;

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

%%******************************* end of sfcn_lin_master.m ********************************