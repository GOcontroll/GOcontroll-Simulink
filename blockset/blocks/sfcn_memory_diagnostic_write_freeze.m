%%***************************************************************************************
%% file         sfcn_memory_diagnostic_write.m
%% brief        Level-2 M file S-Function for GOcontroll diagnostic code system.
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
function sfcn_memory_diagnostic_write_freeze(block)
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

	% dialog params
	% diagType = 1;
	signals = 2;

	signalsData = block.DialogPrm(signals).Data;
	numSignals = length(signalsData);

	% inputs
	diagCode = 1;
	signals = 2:numSignals+1;

	%% Register number of input and output ports
	block.NumInputPorts = numSignals+1;
	block.NumOutputPorts = 0;

	addSimpleInput(block,diagCode, DatatypeID.Uint32);

	for inputCounter = signals
		addSimpleInput(block, inputCounter, DatatypeID.Single);
	end

	%% Number of S-Function parameters expected
	block.NumDialogPrms     = 2;
	block.SampleTimes = [-1 0];
	%% -----------------------------------------------------------------
	%% Register methods called at run-time
	%% -----------------------------------------------------------------

	block.RegBlockMethod('Start', @Start);

	block.RegBlockMethod('Outputs', @Outputs);

	block.RegBlockMethod('Update', @Update);

	block.RegBlockMethod('WriteRTW', @WriteRTW);
end

function Start(~)
end


function Outputs(~)
end


function Update(~)
end

function WriteRTW(block)
	diagType = 1;
	signals = 2;
	signalsData = block.DialogPrm(signals).Data;
	block.WriteRTWParam('string', 'diagType', num2str(block.DialogPrm(diagType).Data));
	block.WriteRTWParam('string', 'num_signals', num2str(length(signalsData)));
	for idx = 0:length(signalsData)-1
		sigName = char(sprintf("signal%d",idx));
		data = char(signalsData(idx+1));
		block.WriteRTWParam('string', sigName, data);
	end
end

%%******************************* end of sfcn_memory_diagnostic_write_freeze.m **********************