%%***************************************************************************************
%% file         sfcn_MQTT_publish.m
%% brief        Level-2 M file S-Function for the MQTT Publish block.
%%              STM target only. Publishes the input signal as a raw byte payload
%%              on a runtime-configurable MQTT topic every model step.
%%
%%   Input ports:
%%     1 : topic   - uint8 array (null-terminated string, max 64 characters)
%%     2 : payload - any data type and width, transmitted as raw bytes
%%     3 : length  - uint16 scalar, actual byte count to publish (only when
%%                   len_source dialog param == 1, i.e. "From port")
%%
%%   Dialog parameters (must match Parameters field):
%%     1 : tsamp      - sample time
%%     2 : len_source - 0 = auto (sizeof payload), 1 = from port 3
%%
%%   Expected RTWdata fields (set by the mask initialisation callback):
%%     qos    : QoS level - 0, 1 or 2
%%     retain : retain flag - 0 = no retain, 1 = broker retains for new subscribers
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%% Copyright 2025 (c) by GOcontroll http://www.gocontroll.com All rights reserved
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
%%***************************************************************************************
function sfcn_MQTT_publish(block)
	setup(block);
end


%% Function: setup ===================================================
function setup(block)
	len_from_port = block.DialogPrm(2).Data;   %% 0 = auto, 1 = from port

	if len_from_port == 2
		block.NumInputPorts = 3;
	else
		block.NumInputPorts = 2;
	end
	block.NumOutputPorts = 0;

	%% Port 1: topic string as a uint8 array (null-terminated).
	block.InputPort(1).DatatypeID        = 3;     %% uint8
	block.InputPort(1).Complexity        = 'Real';
	block.InputPort(1).Dimensions        = -1;    %% inherited - width set by signal
	block.InputPort(1).DirectFeedthrough = true;

	%% Port 2: payload - accept any data type and width.
	block.InputPort(2).DatatypeID        = -1;    %% inherited
	block.InputPort(2).Complexity        = 'Real';
	block.InputPort(2).Dimensions        = -1;    %% inherited
	block.InputPort(2).DirectFeedthrough = true;

	%% Port 3 (optional): explicit payload byte count (uint16 scalar).
	if len_from_port == 2
		block.InputPort(3).DatatypeID        = 5;    %% uint16
		block.InputPort(3).Complexity        = 'Real';
		block.InputPort(3).Dimensions        = 1;
		block.InputPort(3).DirectFeedthrough = true;
	end

	block.NumDialogPrms = 2;
	block.SampleTimes = [block.DialogPrm(1).Data 0];

	block.RegBlockMethod('SetInputPortDimensions', @SetInpPortDims);
	block.RegBlockMethod('SetInputPortDataType',   @SetInpPortDType);
	block.RegBlockMethod('Outputs', @Outputs);
end

%% Called by Simulink during model update to propagate dimensions into each port.
function SetInpPortDims(block, idx, di)
	block.InputPort(idx).Dimensions = di;
end

%% Called by Simulink during model update to propagate the data type into port 2.
function SetInpPortDType(block, idx, dt)
	block.InputPort(idx).DatatypeID = dt;
end

function Outputs(~)
	%% No simulation-time behaviour needed; code generation is handled by TLC.
end

%%******************************* end of sfcn_MQTT_publish.m *****************************
