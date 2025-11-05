%%***************************************************************************************
%% file         sfcn_gps.m
%% brief        Level-2 M file S-Function for the gps module
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
function sfcn_gps(block)
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

% Number of S-Function parameters expected
block.NumDialogPrms = 0;
block.SampleTimes = [1 0];

%% Register number of input and output ports
block.NumInputPorts = 0;
block.NumOutputPorts = 10;

longitude = 1;
addSimpleOutput(block, longitude, DatatypeID.Single);

latitude = 2;
addSimpleOutput(block, latitude, DatatypeID.Single);

altitude = 3;
addSimpleOutput(block, altitude, DatatypeID.Single);

velocity = 4;
addSimpleOutput(block, velocity, DatatypeID.Single);

year = 5;
addSimpleOutput(block, year, DatatypeID.Uint16);

month = 6;
addSimpleOutput(block, month, DatatypeID.Uint8);

day = 7;
addSimpleOutput(block, day, DatatypeID.Uint8);

hour = 8;
addSimpleOutput(block, hour, DatatypeID.Uint8);

minute = 9;
addSimpleOutput(block, minute, DatatypeID.Uint8);

second = 10;
addSimpleOutput(block, second, DatatypeID.Uint8);

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
