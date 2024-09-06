%%***************************************************************************************
%% file         sfcn_memory_diagnostic_delete_single_code.m
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
function sfcn_memory_diagnostic_delete_single_code(block)
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
	block.NumInputPorts = 2;
	block.NumOutputPorts = 0;

	block.NumDialogPrms     = 0;
	block.SampleTimes = [-1 0];
	%% -----------------------------------------------------------------
	%% Register methods called at run-time
	%% -----------------------------------------------------------------

	%%
	%% Start:
	%%   Functionality    : Called in order to initialize state and work
	%%                      area values
	%%   C-Mex counterpart: mdlStart
	%%
	block.RegBlockMethod('Start', @Start);

	%%
	%% Outputs:
	%%   Functionality    : Called to generate block outputs in
	%%                      simulation step
	%%   C-Mex counterpart: mdlOutputs
	%%
	block.RegBlockMethod('Outputs', @Outputs);

	%%
	%% Update:
	%%   Functionality    : Called to update discrete states
	%%                      during simulation step
	%%   C-Mex counterpart: mdlUpdate
	%%
	block.RegBlockMethod('Update', @Update);
end

function Start(~)

  %% No start

end


function Outputs(~)

  %% No output

end


function Update(~)

  %% No update

end

%%******************************* end of sfcn_memory_diagnostic_delete_single_code.m **********************