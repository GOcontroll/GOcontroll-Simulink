%%***************************************************************************************
%% file         AddASAP2Elements.m
%% brief        Adds signals and parameters as Simulink.Signals/Parameters function.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2019 (c) by HAN Automotive     http://www.han.nl     All rights reserved
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

function ASAP2Post(ASAP2file, MAPfile, Target, stationID, XCP_Cro, XCP_Dto, XCP_TCP_Port, XCP_TCP_IPAddress)
% ASAP2POST Post-processing of ASAP2 file.

%   Copyright 1994-2010 The MathWorks, Inc.
% 	Adapted by Jason van Kolfschoten 05-2016

% Declare boolean which registers if an element is in ASAP2 but not in MAP
ElementOptimizedOut = false;

if nargin~=8
    DAStudio.error('RTW:asap2:invalidInputParam',mfilename);
end

% Check MATLAB version
MatlabVersion = version;
if(str2num(MatlabVersion(1)) < 8)
    % these placement holders are used untill MATLAB 2012a
    addrPrefix = '@MemoryAddress@';
    addrSuffix = '@';
else
    % these placement holders are used from MATLAB 2012a on
    addrPrefix = '0x0000 \/\* @ECU_Address@';
    addrSuffix = '@ \*\/';
end

% Extract contents of ASAP2 file
if exist(ASAP2file,'file')
    ASAP2fileString = fileread(ASAP2file);
else
    DAStudio.error('RTW:asap2:UnableFindFile',ASAP2file);
end

% Extract contents of MAP file
if exist(MAPfile,'file')
    MAPfileString = fileread(MAPfile);
else
    DAStudio.error('RTW:asap2:UnableFindFile',MAPfile);
end

% Find the signal and parameter names with their corresponding memory addresses in the .map file
%pairs = regexp(MAPfileString,'\n([0-9a-fA-F]{8})\s{1}[a-z]\s{5}[A-Z]\s{1}[.][A-Za-z]+\t{1}[0-9a-f]{8}\s{1}(\w+)','tokens');

% Check if target has 64 bit architecture
if (strcmp(Target, 'i.MX8'))
pairs = regexp(MAPfileString,'\n\s{16}0x([0-9a-fA-F]{16})\s+(\w+)','tokens');
else
pairs = regexp(MAPfileString,'\n\s{16}0x([0-9a-fA-F]{8})\s+(\w+)','tokens');
end

% Store symbol names and corresponding symbol values into a hash table
MAPfileHash = containers.Map;


for i = 1:length(pairs)

    MAPfileHash(pairs{i}{2}) = pairs{i}{1}; % pairs{i}{2} is name, pairs{1} is memory address
end
% Get all element names with pre- and suffix from the ASAP2 file
ASAP2Elements = regexp(ASAP2fileString,[addrPrefix '(\w+)' addrSuffix],'match');
% Get rid of pre- and suffixes
ASAP2Elements = regexprep(ASAP2Elements,[addrPrefix '(\w+)' addrSuffix],'$1');
% Loop through the elements to check which one isn't in the MAP file
for j = 1:length(ASAP2Elements)
    if isKey(MAPfileHash,ASAP2Elements{j})
        % Do nothing
    else
        % Add the Element name to the Hash so no error will come when substituting the addresses
        % in the ASAP2 file
        MAPfileHash(ASAP2Elements{j}) = '0x0000';
        ElementOptimizedOut = true;
    end
end

try

    % Construct the string containing the target information
    ECU_String = ['\t\tECU\t\t\t\t"', Target,'"\n'];
    % Construct string with the CPU type
    if (strcmp(Target, 'Linux') || strcmp(Target, 'Other'))
        CPU = 'Generic';
    elseif (strcmp(Target, 'Lely GCB') || strcmp(Target, 'i.MX6'))
        CPU = 'i.MX6';
	elseif (strcmp(Target, 'i.MX8'))
        CPU = 'i.MX8';
    else
        CPU = 'Unknown';
    end

    % Build CPU_TYPE string
    CPU_TYPE = ['\t\tCPU_TYPE\t\t"', CPU,'"\n'];
	% disp(CPU_TYPE)

    % Get the _data and _etext memory addresses from the MAPfile
	% Check if target has 64 bit architecture
	if (strcmp(Target, 'i.MX8')) %64 bit
	MemAddr_data = regexp(MAPfileString,'\n[.]data\s+0x[0-9a-f]{16}(\s)+0x[0-9a-f]+','tokens');
	MemAddr_etext = regexp(MAPfileString,'\n[.]rodata\s+0x[0-9a-f]{16}(\s)+0x[0-9a-f]+','tokens');
	else % 32 bit
	MemAddr_data = regexp(MAPfileString,'\n[.]data\s+0x[0-9a-f]{8}(\s)+0x[0-9a-f]+','tokens');
	MemAddr_etext = regexp(MAPfileString,'\n[.]rodata\s+0x[0-9a-f]{8}(\s)+0x[0-9a-f]+','tokens');
	end

    % substract _data from _etext after converting from hex to dec
    MemOffset = int2str(hex2dec(MemAddr_etext{1}) - hex2dec(MemAddr_data{1}));

    % Build the string to place in the A2L file
    MemOffsetString = ['\t\tSYSTEM_CONSTANT\t"RAM-FlashOffset"  "', MemOffset,'"\n'];
    % Get memory address and length of kXcpStationId string from .map file in case of 32 bit architecture

	% Get memory address and length of kXcpStationId string from .map file
	% Check if target has 64 bit architecture
	if (strcmp(Target, 'i.MX8')) %64 bit
	MemAdd_StationID = regexp(MAPfileString,'\n\s+0x([0-9a-fA-F]){16}\s+uniqueIdString+','tokens');
	else
	MemAdd_StationID = regexp(MAPfileString,'\n\s+0x([0-9a-fA-F]){8}\s+uniqueIdString+','tokens');
	end

    if ~isempty(MemAdd_StationID)
        EPK_len = ['\t\tSYSTEM_CONSTANT\t"XcpStationID string length"  "', int2str(length(stationID)),'"\n']; % Build the string that holds the length of the EPK string
        Addr_EPK = ['\tADDR_EPK\t\t0x', MemAdd_StationID{1}{1},'\n']; % Build string with address of kXcpStationId, ADDR_EPK
    else
        EPK_len = '\t\tSYSTEM_CONSTANT\t"XcpStationID string length"  "0x0000"\n'; % EPK string not in MAP
        Addr_EPK = '\tADDR_EPK\t\t0x0000\n'; % EPK string not in MAP
    end
    % Construct the EPK string holding the stationID
    EPK = ['\t\tEPK\t\t\t\t"', stationID,'"\n'];
    % Insert information strings with in the MOD_PAR field
    newASAP2fileString = regexprep(ASAP2fileString,'/end MOD_PAR', [Addr_EPK, CPU_TYPE, ECU_String, EPK, EPK_len, MemOffsetString,'\t/end MOD_PAR'],'once');

%% Add XCP interface information for HANtune
if( (XCP_Cro ~= 0) || (XCP_TCP_Port ~= 0) ) % Only add the IF_DATA field when XCP on CAN or TCP is used
    IF_DATA = '\t/begin IF_DATA XCP\n';
    IF_DATA_END = '\t/end IF_DATA';

    % Add information for CAN communication
    if (XCP_Cro ~= 0) % XCP over CAN data found
        IF_DATA_CAN = strcat('\t\t/begin XCP_ON_CAN\n',...
                        '\t\t\t0x100\n',...
                        '\t\t\tCAN_ID_BROADCAST 0x7FF\n',...
                        '\t\t\tCAN_ID_MASTER 0x', dec2hex(XCP_Cro),'\n',...
                        '\t\t\tCAN_ID_SLAVE 0x', dec2hex(XCP_Dto),'\n',...
                        '\t\t\tBAUDRATE 0x', dec2hex(XCP_Baudrate*1000),'\n',...
                        '\t\t/end XCP_ON_CAN\n');
        IF_DATA = strcat(IF_DATA,IF_DATA_CAN);
    end
    % Add information for TCP communication
    if (XCP_TCP_Port ~= 0) % XCP over TCP data found
        IF_DATA_TCP = strcat('\t\t/begin XCP_ON_TCP_IP\n',...
                        '\t\t\t0x100\n',...
                        '\t\t\t',int2str(XCP_TCP_Port),'\n',...
                        '\t\t\t"HOST_NAME"',{' '},Target,'\n',...
                        '\t\t\t"ADDRESS" ',{' '}, XCP_TCP_IPAddress,'\n',...
                        '\t\t/end XCP_ON_TCP_IP\n');
        IF_DATA = strcat(IF_DATA,IF_DATA_TCP);
    end

    IF_DATA = strcat(IF_DATA,IF_DATA_END); % Add the end to the string
    newASAP2fileString = regexprep(newASAP2fileString,'/end MOD_PAR', strcat('/end MOD_PAR\n\n',IF_DATA),'once');
end

    % create the contents of the new asap2 file by overwriting the placeholders by the memory addresses.
    newASAP2fileString = regexprep(newASAP2fileString,[addrPrefix '(\w+)' addrSuffix], '${MAPfileHash($1)}');
    % Write ASAP2 file string to file.
    fid = fopen(ASAP2file, 'w');
    fprintf(fid,'%s',newASAP2fileString);
    fclose(fid);

catch
    error('ASAP2 file generation failed');
end

if ElementOptimizedOut
    disp('Warning! One or more parameters or signals were optimized out and are not included in the A2L file'); %could be warndlg
end
end % End of function ASAP2Post