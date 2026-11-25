%%***************************************************************************************
%% file         sfcn_gocontroll_output_module_monitor.m
%% brief        Level-2 M file S-Function for GOcontroll output module.
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
function sfcn_gocontroll_output_module_monitor(block)
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
%% Dialog parameters
tsamp = 1;
% moduleSlot = 2;
C1duty = 3;
% C2duty = 4;
% C3duty = 5;
% C4duty = 6;
% C5duty = 7;
C6duty = 8;

% Number of S-Function parameters expected
block.NumDialogPrms     = 8;
block.SampleTimes = [block.DialogPrm(tsamp).Data 0];

%% Register number of input and output ports
baseOutputs = 10;
block.NumInputPorts = 0;
num_duty = 0;
for idx = C1duty:C6duty
	if block.DialogPrm(idx).Data == 1
		num_duty = num_duty + 1;
	end
end
block.NumOutputPorts = baseOutputs+num_duty;

moduleTemp = 1;
addSimpleOutput(block, moduleTemp, DatatypeID.Int16);

moduleGnd = 2;
addSimpleOutput(block, moduleGnd, DatatypeID.Int16);

moduleSup = 3;
addSimpleOutput(block, moduleSup, DatatypeID.Uint16);

moduleStat = 4;
addSimpleOutput(block, moduleStat, DatatypeID.Uint32);

moduleChannelsStart = 5;
offset = moduleChannelsStart-C1duty; % offset between output port number and dialog parameter number.
for idx = C1duty:C6duty
	addSimpleOutput(block, idx+offset, DatatypeID.Int16);
	if block.DialogPrm(idx).Data == 1 % this port receives duty cycle feedback
		offset = offset + 1; % we've added another output port so we need to increase the offset
		addSimpleOutput(block, idx+offset, DatatypeID.Uint16);
	end
end

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


%%******************************* end of sfcn_gocontroll_output_module_monitor.m **********************
