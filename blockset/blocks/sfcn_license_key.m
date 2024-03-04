%%***************************************************************************************
%% file         sfcn_license_key.m
%% brief        Target Language Compiler file that contains the code generation specifics
%%              for an S-function with the same name.
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
function sfcn_license_key(block)
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
	block.NumOutputPorts = 0;

	% Number of S-Function parameters expected
	% (key, iv, license, check_file)
	block.NumDialogPrms     = 4;
	block.SampleTimes = [1 0];
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
	key = 1;
	iv = 2;
	license = 3;
	check_file = 4;

	block.WriteRTWParam('matrix', 'key', block.DialogPrm(key).Data);
	block.WriteRTWParam('string', 'key_len', num2str(length(block.DialogPrm(key).Data)));
	block.WriteRTWParam('string', 'iv', ['"' block.DialogPrm(iv).Data '"']);
	block.WriteRTWParam('string', 'license', ['"' block.DialogPrm(license).Data '"']);
	block.WriteRTWParam('string', 'check_file', ['"' block.DialogPrm(check_file).Data '"']);
end

%%******************************* end of sfcn_license_key.m *****************************
