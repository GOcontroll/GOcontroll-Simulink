%%***************************************************************************************
%% file         go_s1_package.m
%% brief        Packs a raw Moduline S1 application image into the firmware package
%%              (magic "GOCO") that the controller takes over Bluetooth.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2026 (c) by GOcontroll     http://www.gocontroll.com    All rights reserved
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

function container = go_s1_package(raw, appId, version)
% GO_S1_PACKAGE Pack a raw application image for the Moduline S1.
%
%   CONTAINER = GO_S1_PACKAGE(RAW, APPID, VERSION) returns the unsigned package as a row of
%   uint8: a header (magic "GOCO", the App ID, the version, sizes, the SHA-256 of the
%   payload and a CRC-16) followed by RAW as a zlib stream.
%
%   RAW      the image as uint8, what objcopy -O binary writes (the .bin of a build)
%   APPID    the App ID of the controller, at most 32 ASCII characters; '' for a blank one
%   VERSION  'major.minor.patch', every field 0-255
%
%   The package is not signed. A controller that carries a public key takes signed
%   packages only: flash it with the station Flash of the GOcontroll Suite.
%
%   The same format as the GOcontroll Suite and the Deployment Tool write.

% STM32H573: page 252 and up is EEPROM emulation and is never written
APP_MAX_SIZE = hex2dec('1F8000');

raw = uint8(raw(:)');
appId = char(appId);
if numel(raw) > APP_MAX_SIZE
	error('go_s1_package:tooLarge', ...
		'The image is %d bytes; at most %d fits before the protected region of the flash.', numel(raw), APP_MAX_SIZE);
end
if any(appId > 127)
	error('go_s1_package:appId', 'The App ID must be ASCII.');
end
if numel(appId) > 32
	error('go_s1_package:appId', 'The App ID may be at most 32 ASCII characters, got %d.', numel(appId));
end
parts = regexp(strtrim(char(version)), '^(\d+)\.(\d+)\.(\d+)$', 'tokens', 'once');
if isempty(parts) || any(str2double(parts) > 255)
	error('go_s1_package:version', 'The version must be major.minor.patch with every field 0-255, got "%s".', char(version));
end

% Two header versions that differ only in the width of the App ID field
if numel(appId) > 8
	headerVersion = 4; appIdSize = 32;
else
	headerVersion = 2; appIdSize = 8;
end

payload = i_deflate(raw);
digest = java.security.MessageDigest.getInstance('SHA-256');
digest.update(typecast(payload, 'int8')); % a Java byte is signed
sha = typecast(digest.digest(), 'uint8')';

header = [uint8('GOCO'), uint8(headerVersion), ...
	uint8([appId, repmat(' ', 1, appIdSize - numel(appId))]), ...
	uint8(str2double(parts)), ...
	typecast(uint32(numel(payload)), 'uint8'), ...   % little-endian
	typecast(uint32(numel(raw)), 'uint8'), ...
	sha];
crc = i_crc16(header);
container = [header, uint8(bitand(crc, 255)), uint8(bitshift(crc, -8)), payload];

end % end of function go_s1_package()


function out = i_deflate(raw)
% raw as a zlib stream (RFC 1950), which the ESP32 of the controller inflates. Level 9, as
% the Deployment Tool packs: the same image gives the same package, byte for byte.
	bytes = java.io.ByteArrayOutputStream();
	stream = java.util.zip.DeflaterOutputStream(bytes, java.util.zip.Deflater(9));
	stream.write(typecast(raw, 'int8'), 0, numel(raw)); % a Java byte is signed
	stream.close();
	out = typecast(bytes.toByteArray(), 'uint8')';
end

function crc = i_crc16(data)
% CRC-16/CCITT (init 0xFFFF, poly 0x1021), as the ESP32 computes it.
	crc = uint32(65535);
	for k = 1:numel(data)
		crc = bitxor(crc, bitshift(uint32(data(k)), 8));
		for bit = 1:8
			if bitand(crc, 32768)
				crc = bitand(bitxor(bitshift(crc, 1), 4129), 65535);
			else
				crc = bitand(bitshift(crc, 1), 65535);
			end
		end
	end
	crc = double(crc);
end
