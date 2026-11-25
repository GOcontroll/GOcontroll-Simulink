%%***************************************************************************************
%% file         sfcn_software_version.m
%% brief        Level-2 M-file S-Function for the software version block.
%%              Outputs Major, Minor and Patch as uint8 constants and registers the
%%              version in GO_controller_info for later retrieval.
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
function sfcn_software_version(block)
	setup(block);
end

%% Function: setup ===================================================
%% Abstract:
%%   Set up the S-function block's basic characteristics such as:
%%   - Input ports  : none
%%   - Output ports : 3 (Major, Minor, Patch - uint8 scalars)
%%   - Dialog params: 3 (major, minor, patch)
%%
%%   Required         : Yes
%%   C-Mex counterpart: mdlInitializeSizes

function setup(block)
	block.NumInputPorts  = 0;
	block.NumOutputPorts = 3;

	%% Port 1 out: Major version (uint8)
	block.OutputPort(1).DatatypeID = 3;  % uint8
	block.OutputPort(1).Complexity = 'Real';
	block.OutputPort(1).Dimensions = 1;

	%% Port 2 out: Minor version (uint8)
	block.OutputPort(2).DatatypeID = 3;  % uint8
	block.OutputPort(2).Complexity = 'Real';
	block.OutputPort(2).Dimensions = 1;

	%% Port 3 out: Patch version (uint8)
	block.OutputPort(3).DatatypeID = 3;  % uint8
	block.OutputPort(3).Complexity = 'Real';
	block.OutputPort(3).Dimensions = 1;

	%% Dialog parameters: major, minor, patch (passed via block Parameters field)
	block.NumDialogPrms = 3;
	block.SampleTimes = [-1 0];  % Inherited - version is constant, runs with model rate

	block.RegBlockMethod('Start', @Start);
	block.RegBlockMethod('Outputs', @Outputs);
end

function Start(~)
	%% No simulation action needed - only triggers TLC Start for code generation
end

function Outputs(block)
	%% Simulation: output the actual version values from the mask parameters.
	block.OutputPort(1).Data = uint8(block.DialogPrm(1).Data);
	block.OutputPort(2).Data = uint8(block.DialogPrm(2).Data);
	block.OutputPort(3).Data = uint8(block.DialogPrm(3).Data);
end

%%******************************* end of sfcn_software_version.m ***************************
