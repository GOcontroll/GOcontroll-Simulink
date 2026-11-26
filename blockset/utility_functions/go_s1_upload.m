%%***************************************************************************************
%% file         go_s1_upload.m
%% brief        Programs a built application into a Moduline S1 over Bluetooth.
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

function go_s1_upload(binFile, options)
% GO_S1_UPLOAD Program a built application into a Moduline S1 over Bluetooth.
%
%   GO_S1_UPLOAD(BINFILE) packs the image BINFILE (the .bin of a build) for the Moduline S1
%   that is in range, sends it over Bluetooth and follows the controller until it has
%   programmed the application and restarts into it. With more than one Moduline S1 in range
%   it stops and lists them.
%
%   GO_S1_UPLOAD(BINFILE, controller=ID) programs the controller with that serial number
%   (as on its label, "A4CG-B034-A003-A002") or that Bluetooth address (12 hex digits).
%
%   GO_S1_UPLOAD(BINFILE, version=VERSION) names the version of the application. The default
%   is the version at the end of the file name (MyModel_1.2.3.bin), or 0.0.0.
%
%   The package is not signed: a controller that carries a public key refuses it, and this
%   function says so before anything is sent. Flash such a controller with the station Flash
%   of the GOcontroll Suite.
%
%   Needs Bluetooth on this computer (Windows or macOS) and MATLAB R2019b or later.
%
%   options.radio is how the controller is reached; a test replaces it with a controller
%   that is not there.
%
%   See also ManualUpload, go_s1_package.

arguments
	binFile {mustBeTextScalar}
	options.controller {mustBeTextScalar} = ''
	options.version {mustBeTextScalar} = ''
	options.radio struct = i_bluetooth()
end

binFile = char(binFile);
wanted = strtrim(char(options.controller));
version = char(options.version);
radio = options.radio;

if ~isfile(binFile)
	error('go_s1_upload:noFile', 'Could not find %s. Build the model first.', binFile);
end
file = fopen(binFile, 'r');
raw = fread(file, Inf, '*uint8')';
fclose(file);
if isempty(version)
	token = regexp(binFile, '_(\d{1,3}\.\d{1,3}\.\d{1,3})(\.[A-Za-z][A-Za-z0-9]*)?$', 'tokens', 'once');
	if isempty(token), version = '0.0.0'; else, version = token{1}; end
end

[gatt, signals, take] = i_find(radio, wanted);
closeLink = onCleanup(@() gatt.close());

fprintf('### Moduline S1 connected (serial number %s, application %s, ESP firmware %s)\n', ...
	i_field(signals, 'controller_serial', 'not read'), ...
	i_field(signals, 'app_version', 'not read'), i_field(signals, 'esp_version', 'not read'));

% What the controller would refuse is said here: letting the controller find out costs the
% whole transfer.
if ~isfield(signals, 'app_id')
	error('go_s1_upload:noState', ...
		['The controller did not say which App ID it carries, so no package can be made for it. ' ...
		 'Try again; if it stays so, check that no phone or other computer is connected to the controller.']);
end
if isfield(signals, 'update_ble') && ~signals.update_ble
	error('go_s1_upload:switchedOff', 'Updating over Bluetooth is switched off for this controller.');
end
if isfield(signals, 'public_key') || (isfield(signals, 'signing_enabled') && signals.signing_enabled)
	error('go_s1_upload:signed', ...
		['This controller carries a public key and takes signed packages only. The blockset does ' ...
		 'not sign: flash %s with the station Flash of the GOcontroll Suite, which asks for the private key.'], binFile);
end
appId = i_field(signals, 'app_id', '');
container = go_s1_package(raw, appId, version);
fprintf('### Uploading %s as version %s (%d bytes, %d packed) over Bluetooth\n', ...
	binFile, version, numel(raw), numel(container));

info = dir(binFile);
i_program(gatt, take, signals, container, version, char(datetime(info.datenum, 'ConvertFrom', 'datenum', 'Format', 'yyyy-MM-dd')));
fprintf('### Upload finished: the controller restarts into version %s\n', version);

end % end of function go_s1_upload()


%% Finding the controller %%

function [gatt, signals, take] = i_find(radio, wanted)
% Connect to the controller that is meant and return what it says about itself.
	isAddress = ~isempty(regexp(wanted, '^([0-9A-Fa-f]{2}[:-]?){5}[0-9A-Fa-f]{2}$', 'once'));
	if isAddress
		addresses = string(upper(regexprep(wanted, '[:-]', '')));
	else
		fprintf('### Looking for a Moduline S1 over Bluetooth...\n');
		addresses = radio.scan();
		if isempty(addresses)
			error('go_s1_upload:notFound', ...
				['No Moduline S1 found over Bluetooth. Check that the controller is powered and within ' ...
				 'a few metres, that Bluetooth is on, and that no phone or other computer is connected to it.']);
		end
		if isempty(wanted) && numel(addresses) > 1
			error('go_s1_upload:several', ...
				['%d Moduline S1 controllers are in range (Bluetooth addresses %s, strongest first). Name the one ' ...
				 'to program by its serial number or address: the option S1 controller of the model, or ' ...
				 'ManualUpload <model> <serial number>.'], numel(addresses), strjoin(addresses, ', '));
		end
	end

	seen = strings(1, 0);
	for k = 1:numel(addresses)
		try
			gatt = radio.open(char(addresses(k)));
		catch ME
			if isAddress || isempty(wanted)
				error('go_s1_upload:connect', 'Could not connect to the Moduline S1 at %s: %s', addresses(k), ME.message);
			end
			continue % another controller in range that does not answer: not the one that is meant
		end
		take = i_inbox(gatt);
		% what the upload decides on; the serial number only when it chooses the controller
		needed = {'esp_version', 'app_id', 'signing_enabled', 'update_ble'};
		if ~isAddress && ~isempty(wanted), needed{end+1} = 'controller_serial'; end
		signals = i_start(gatt, take, needed);
		serial = i_field(signals, 'controller_serial', '');
		if isAddress || isempty(wanted) || strcmpi(serial, wanted)
			return
		end
		seen(end+1) = string(serial); %#ok<AGROW>
		gatt.close();
	end
	error('go_s1_upload:notFound', ...
		'No Moduline S1 with serial number %s in range. In range: %s.', wanted, strjoin(seen(seen ~= ""), ', '));
end

function take = i_inbox(gatt)
% What the controller sent, one notification at a time and in the order it came: take()
% gives the next one as {characteristic, bytes}, or {} when there is none yet. Nothing is
% lost between one wait and the next: the controller sends its first flash progress right
% behind the status that ends the transfer.
	pending = {};
	take = @next;

	function item = next()
		if isempty(pending), pending = gatt.poll(); end
		if isempty(pending)
			item = {};
		else
			item = pending{1};
			pending(1) = [];
		end
	end
end

function signals = i_start(gatt, take, needed)
% Subscribe and read the INFO state: the ESP32 sends it as a burst right after the
% subscription. MATLAB's Bluetooth does not pass on every notification of such a burst
% (measured on Windows with R2023b: about one in three arrives, another third each time),
% so the state is asked for again until the signals in NEEDED have all been seen.
	gatt.subscribe('status');
	gatt.subscribe('info'); % last: the controller answers with its whole state
	signals = i_ask_again(gatt, take, i_collect(take, struct()), needed);
end

function signals = i_ask_again(gatt, take, signals, needed)
% Subscribe to INFO anew, which makes the controller send its state again, until the
% signals in NEEDED are known or it has been asked 25 times.
	for attempt = 1:25
		if all(isfield(signals, needed)), return, end
		gatt.unsubscribe('info');
		gatt.subscribe('info');
		signals = i_collect(take, signals);
	end
end

function signals = i_collect(take, signals)
% Read INFO until it has been quiet for 0.3 s, at most 2 s.
	started = tic; last = [];
	while toc(started) < 2 && (isempty(last) || toc(last) < 0.3)
		item = take();
		if ~isempty(item) && strcmp(item{1}, 'info')
			signals = i_decode(signals, item{2});
			last = tic;
		end
	end
end

function signals = i_decode(signals, data)
% One INFO TLV (type, length, data) as the signal it carries; other types are ignored, so
% this keeps working against newer ESP firmware.
	data = double(data);
	if numel(data) < 2 || numel(data) - 2 < data(2), return, end
	payload = data(3:2 + data(2));
	switch data(1)
		case 1,  if numel(payload) == 3, signals.app_version = sprintf('%d.%d.%d', payload); end
		case 11, if numel(payload) == 3, signals.esp_version = sprintf('%d.%d.%d', payload); end
		case 24, signals.controller_serial = strtrim(char(payload));
		case 51, signals.app_id = strtrim(char(payload));          % 0x33; empty: a blank controller
		case 52, if isscalar(payload), signals.signing_enabled = payload ~= 0; end   % 0x34
		case 54, if numel(payload) == 65, signals.public_key = payload; end          % 0x36
		case 56, if isscalar(payload), signals.update_ble = payload ~= 0; end        % 0x38
	end
end

function value = i_field(signals, name, fallback)
	if isfield(signals, name) && ~isempty(signals.(name)), value = signals.(name); else, value = fallback; end
end


%% Programming %%

function i_program(gatt, take, signals, container, version, releaseDate)
% Send a package and follow the controller until its application is programmed. Errors
% with the reason when the controller refuses it or the programming fails.
	START = 1; FINISH = 2; ABORT = 3; RELEASE_DATE = 5;   % CONTROL commands
	READY = 1; COMPLETE = 3; FAILED = 255;                % STATUS values
	CHUNK = 244;   % bytes per DATA write: fits the MTU the Moduline S1 offers
	PHASES = {'Preparing', 'Erasing the flash', 'Writing the software', 'Done', 'Programming failed', ...
		'Refused: not a valid package', 'Refused: the App ID of the package is not that of the controller', ...
		'Refused: the package arrived incomplete or corrupt', ...
		'Refused: the signature is invalid, made with another key, or missing'};

	while ~isempty(take()), end % what came in before now belongs to nothing
	gatt.write('control', START, true);
	i_await_status(take, READY, 15);
	% The package carries no date, so it is named here (ESP firmware 1.2.0 or later keeps it)
	esp = sscanf(i_field(signals, 'esp_version', '0.0.0'), '%d.%d.%d')';
	if esp(1) > 1 || (esp(1) == 1 && esp(2) >= 2)
		gatt.write('control', [RELEASE_DATE, uint8(releaseDate)], true);
	end

	total = numel(container); shown = -1;
	try
		for offset = 1:CHUNK:total
			last = min(offset + CHUNK - 1, total);
			gatt.write('data', container(offset:last), false);
			tenth = floor(last * 10 / total);
			if tenth > shown
				fprintf('### Sending the software: %d %%\n', tenth * 10);
				shown = tenth;
			end
		end
	catch ME
		try gatt.write('control', ABORT, true); catch, end
		error('go_s1_upload:transfer', 'The transfer to the controller stopped: %s', ME.message);
	end
	gatt.write('control', FINISH, true);

	% The controller answers END with STATUS COMPLETE and then reports the programming as
	% INFO 0x17: [phase, percentage]. Phase 4 is done; 5 and up end it without the software.
	% Any of these notifications can fail to arrive (see i_start), so none of them is waited
	% for on its own: flash progress also says the package was taken, and when the controller
	% has been silent for 20 s it is asked which application it has.
	before = i_field(signals, 'app_version', '');
	started = tic; heard = tic; taken = false; shown = [-1 -1]; unchanged = 0;
	while toc(started) < 600
		item = take();
		if isempty(item)
			if ~taken && toc(started) > 30
				error('go_s1_upload:timeout', 'The controller did not answer in time.');
			end
			if taken && toc(heard) > 20
				current = i_field(i_ask_again(gatt, take, struct(), {'app_version'}), 'app_version', '');
				if strcmp(current, version) && ~strcmp(before, version)
					return % it runs the new version: the report of the last phase did not arrive
				end
				unchanged = unchanged + 1;
				if unchanged >= 3 && ~isempty(current) && ~strcmp(current, version)
					error('go_s1_upload:noResult', ...
						['The controller stopped reporting and still has application %s: the software was ' ...
						 'not programmed. Try again.'], current);
				end
				heard = tic;
			end
			continue
		end
		data = double(item{2});
		if strcmp(item{1}, 'status') && ~isempty(data)
			if data(1) == COMPLETE, taken = true; end
			if data(1) == FAILED
				error('go_s1_upload:busy', ...
					'The controller reports an error: it is busy with another update, or the transfer went out of step.');
			end
			continue
		end
		if ~strcmp(item{1}, 'info') || numel(data) ~= 4 || data(1) ~= 23 || data(2) ~= 2, continue, end
		taken = true; heard = tic; unchanged = 0;
		phase = data(3);
		if phase == 4, return, end
		if phase > 4
			if phase <= numel(PHASES), label = PHASES{phase}; else, label = sprintf('Programming failed (phase %d)', phase); end
			error('go_s1_upload:refused', '%s.', label);
		end
		step = [phase, floor(data(4) / 25)];
		if phase >= 1 && ~isequal(step, shown)
			fprintf('### %s: %d %%\n', PHASES{phase}, data(4));
			shown = step;
		end
	end
	error('go_s1_upload:timeout', ...
		'The controller did not report within 10 minutes that it programmed the software. Check which application it runs.');
end

function i_await_status(take, expected, seconds)
	started = tic;
	while toc(started) < seconds
		item = take();
		if isempty(item) || ~strcmp(item{1}, 'status') || isempty(item{2}), continue, end
		status = item{2}(1);
		if status == expected, return, end
		if status == 255
			error('go_s1_upload:busy', ...
				'The controller reports an error: it is busy with another update, or the transfer went out of step.');
		end
	end
	error('go_s1_upload:timeout', 'The controller did not answer in time.');
end


%% Bluetooth %%

function radio = i_bluetooth()
% The Bluetooth of this computer. Every Moduline S1 advertises the same name, so they are
% told apart by address here and by serial number after connecting.
	radio.scan = @i_scan;
	radio.open = @i_open;
end

function addresses = i_scan()
% The Bluetooth addresses of the Moduline S1 controllers in range, strongest first.
	addresses = strings(1, 0);
	list = blelist("Name", "Moduline-S1", "Timeout", 5);
	if ~isempty(list)
		list = sortrows(list(list.Name == "Moduline-S1", :), 'RSSI', 'descend');
		addresses = string(list.Address)';
	end
end

function gatt = i_open(address)
% The OTA service of one controller: write to DATA and CONTROL, notifications of STATUS and
% INFO. poll() gives what came in since the last call, as {characteristic, bytes}.
	SERVICE = "9A3E5F72-8C1D-4B6A-A7E2-3F0D8C5B1E9F";
	device = ble(address);
	chars.data    = characteristic(device, SERVICE, "9A3E5F73-8C1D-4B6A-A7E2-3F0D8C5B1E9F");
	chars.control = characteristic(device, SERVICE, "9A3E5F74-8C1D-4B6A-A7E2-3F0D8C5B1E9F");
	chars.status  = characteristic(device, SERVICE, "9A3E5F75-8C1D-4B6A-A7E2-3F0D8C5B1E9F");
	chars.info    = characteristic(device, SERVICE, "9A3E5F76-8C1D-4B6A-A7E2-3F0D8C5B1E9F");
	queue = {};

	gatt.write = @send;
	gatt.subscribe = @listen;
	gatt.unsubscribe = @(name) unsubscribe(chars.(name));
	gatt.poll = @poll;
	gatt.close = @hangup;

	function send(name, bytes, withResponse)
		if withResponse, type = 'withresponse'; else, type = 'withoutresponse'; end
		write(chars.(name), bytes, 'uint8', type);
	end

	function listen(name)
		if isempty(chars.(name).DataAvailableFcn)
			chars.(name).DataAvailableFcn = @(source, ~) received(name, source); % subscribes as well
		else
			subscribe(chars.(name));
		end
	end

	function received(name, source)
		queue{end+1} = {name, uint8(read(source, 'oldest'))};
	end

	function items = poll()
		pause(0.02); % MATLAB runs received() for what came in while it waits here
		items = queue;
		queue = {};
	end

	function hangup()
		% The callbacks hold this workspace and with it the connection: let go of them first.
		names = {'status', 'info'};
		for k = 1:numel(names)
			try
				chars.(names{k}).DataAvailableFcn = [];
				unsubscribe(chars.(names{k}));
			catch
			end
		end
		chars = [];
		device = []; %#ok<NASGU> the connection ends with the last reference to it
	end
end
