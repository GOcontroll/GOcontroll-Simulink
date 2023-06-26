%%***************************************************************************************
%% file         AddASAP2Elements.m
%% brief        Adds signals and parameters as Simulink.Signals/Parameters function.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2019 (c) by HAN Automotive     http://www.han.nl     All rights reserved
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
function AddASAP2Elements(modelName)
% find settings block
disp('### ASAP2 function started\n');
HANcoderBlocks = find_system(modelName, 'RegExp', 'on', 'LookUnderMasks', 'All', 'Tag', 'HANcoder_TARGET_.');
for index=1:1:length(HANcoderBlocks) % Loop through the HANcoder blocks
    HANcoderStruct = get_param(HANcoderBlocks{index},'UserData');
    BlockType = HANcoderStruct.BlockType;
    if strcmp(BlockType,'Settings')
        MaskValues = get_param(HANcoderBlocks{index},'MaskValues');
        if strcmp(MaskValues{1},'on')
            disp('### Adding all undefined parameters and signals to the ASAP2 file');
            AddParameters(modelName)
            AddSignals(modelName)
        end
        break
    end
end
end % end of function AddASAP2Elements()

%% A function to find all constants in the model which are not defined in the workspace
% and defining them as Simulink.Parameter so they will appear in HANtune
function AddParameters(modelName)
% Find all constants and gains
constants = find_system(modelName, 'LookUnderMasks', 'all', 'BlockType', 'Constant');
gains = find_system(modelName, 'LookUnderMasks', 'all', 'BlockType', 'Gain');
% Get the values of the constants and gains
ConstantNames = get_param(constants,'Value');
GainNames = get_param(gains,'Gain');
Names = [ConstantNames; GainNames];
% Declare cell array of strings to hold the names that were already
% declared in this function
FoundNames = {};
indexFoundNames = 1;
W = evalin('base','who'); %Get workspace names
for i = 1:length(Names)
    Name = Names{i};
    if isvarname(Name) % Check if it is a valid variable name, this filters out the numbers
        if ~ismember(Name,W) % Check if the name of the constant is in the workspace
            if(isempty(find(ismember(FoundNames, Name),1))) %Check if this has been found before
                % Add name to list of found blocks
                FoundNames{indexFoundNames} = Name;
                % Increment the index for the next name
                indexFoundNames = indexFoundNames + 1;
                UserValue = GetUserValue(Name);
                assignin('base', Name, Simulink.Parameter);
                evalin('base', strcat(Name,'.StorageClass = ''ExportedGlobal'';'));
                evalin('base', strcat(Name,'.Value = ',UserValue,';'));
            end
        end
    end
end
end % End of function AddParameters()

%% A function to find all signals in the model which are not defined in the workspace
% and defining them as Simulink.Signal so they will appear in HANtune
function AddSignals(modelName)
disp('##### add signals\n');
% Find all signals
Lines=find_system(modelName,'FindAll','On','LookUnderMasks','All','FollowLinks','On','Type','line');
LineNames = get(Lines,'Name');
% Loop through signals
for index = 1:length(LineNames)
    if(isvarname(LineNames{index}))
        if(~exist(LineNames{index},'var'))
            assignin('base', LineNames{index}, Simulink.Signal);
            evalin('base', strcat(LineNames{index},'.StorageClass = ''ExportedGlobal'';'));
        end
    end
end
end % End of function AddSignals()

%% A function that prompts the user for the value of the parameter
function UserValue = GetUserValue(Name)
UserValue = '0'; % Set to zero as fallback
prompt = ['Please enter a value for ',Name];
userInput = inputdlg(prompt, Name, 1,{'0'});
userInput = userInput{1};
% Check which inputs are digits
userInputDigits = isstrprop(userInput, 'digit');
if isempty(userInput)
    error('No input was given, please define the parameters by hand')
else
    if(isempty(find(userInputDigits==0, 1))) % No non-digit found
        [num,status] = str2num(userInput);
        if (status)
            UserValue = userInput;
        end
    else % Non digit found, check if this was a point or a minus sign
        if (~isempty(find(userInput=='.', 1)) || ~isempty(find(userInput=='-', 1)))
            [num,status] = str2num(userInput);
            if (status)
                UserValue = userInput;
            else
               error('Please only use numberic digits');
            end
        elseif (~isempty(find(userInput==',', 1)))
            error('Please use the point as decimal separator, NOT the comma');
        else % If it wasn't a comma it must have been another character
            error('Please only use numberic digits');
        end
    end
end
end % end of function GetUserValue()
%% end of file