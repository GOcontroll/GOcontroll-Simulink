%%***************************************************************************************
%% file         sfcn_MQTT_subscribe.m
%% brief        Level-2 M file S-Function for the MQTT Subscribe block.
%%              STM target only. Subscribes to a topic at startup and outputs the
%%              received payload, its length, and a new-data flag each model step.
%%
%%   Input ports:
%%     1 : topic - uint8 array (null-terminated string, max 64 characters).
%%                 Read once at model initialisation to set the subscription topic.
%%
%%   Output ports:
%%     1 : new_data  - uint8 scalar, 1 if a new message arrived since the last step
%%     2 : length    - uint16 scalar, number of valid bytes in the last message
%%     3 : payload   - uint8 array, length = payload_size dialog parameter
%%
%%   Dialog parameters:
%%     1 : tsamp        - sample time in seconds
%%     2 : payload_size - maximum payload length in bytes (sets output port 3 width)
%%
%%   Expected RTWdata fields (set by the mask initialisation callback):
%%     qos : QoS level - 0, 1 or 2
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
function sfcn_MQTT_subscribe(block)
	setup(block);
end


%% Function: setup ===================================================
function setup(block)
	block.NumInputPorts  = 1;
	block.NumOutputPorts = 3;

	%% Port 1 in: topic string as uint8 array (null-terminated, max 64 chars).
	%% Read once at model initialisation - not each step.
	block.InputPort(1).DatatypeID        = 3;    % uint8
	block.InputPort(1).Complexity        = 'Real';
	block.InputPort(1).Dimensions        = -1;   % inherited - width set by signal
	block.InputPort(1).DirectFeedthrough = false;

	%% Port 1 out: new-data flag (uint8 scalar, 1 = new message since last step)
	block.OutputPort(1).DatatypeID  = 3;            % uint8
	block.OutputPort(1).Complexity  = 'Real';
	block.OutputPort(1).Dimensions  = 1;

	%% Port 2 out: received payload length (uint16 scalar)
	block.OutputPort(2).DatatypeID  = 5;            % uint16
	block.OutputPort(2).Complexity  = 'Real';
	block.OutputPort(2).Dimensions  = 1;

	%% Port 3 out: received payload bytes (uint8 vector, length = payload_size)
	block.OutputPort(3).DatatypeID  = 3;            % uint8
	block.OutputPort(3).Complexity  = 'Real';
	block.OutputPort(3).Dimensions  = block.DialogPrm(2).Data;

	block.NumDialogPrms = 2;
	block.SampleTimes = [block.DialogPrm(1).Data 0];

	block.RegBlockMethod('SetInputPortDimensions', @SetInpPortDims);
	block.RegBlockMethod('Start',  @Start);
	block.RegBlockMethod('Outputs', @Outputs);
end

function SetInpPortDims(block, idx, di)
	block.InputPort(idx).Dimensions = di;
end

function Start(~)
	%% No simulation-time behaviour; code generation is handled by TLC.
end

function Outputs(block)
	%% No simulation-time behaviour needed; code generation is handled by TLC.
	payload_size = block.DialogPrm(2).Data;
	block.OutputPort(1).Data = uint8(0);
	block.OutputPort(2).Data = uint16(0);
	block.OutputPort(3).Data = zeros(payload_size, 1, 'uint8');
end

%%******************************* end of sfcn_MQTT_subscribe.m ***************************
