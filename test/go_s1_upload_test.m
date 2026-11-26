function tests = go_s1_upload_test
% Tests of the Bluetooth upload to a Moduline S1, without a controller and without a radio:
% go_s1_package against packages the Deployment Tool made (goco_fixtures.json), and
% go_s1_upload against a controller that is not there (i_controller below).
%
%   cd <repository>; librarySetup; results = runtests('test/go_s1_upload_test.m')
	tests = functiontests(localfunctions);
end

function setupOnce(testCase)
	here = fileparts(mfilename('fullpath'));
	fixtures = jsondecode(fileread(fullfile(here, 'goco_fixtures.json')));
	testCase.TestData.fixtures = fixtures;
	testCase.TestData.raw = i_inflate(matlab.net.base64decode(fixtures.payload_b64));
	folder = tempname; mkdir(folder);
	testCase.addTeardown(@() rmdir(folder, 's'));
	testCase.TestData.bin = fullfile(folder, 'MyModel_1.2.3.bin');
	file = fopen(testCase.TestData.bin, 'w'); fwrite(file, testCase.TestData.raw); fclose(file);
end

%% The package %%

function test_package_is_what_the_deployment_tool_makes(testCase)
	cases = testCase.TestData.fixtures.cases;
	for k = 1:numel(cases)
		made = go_s1_package(testCase.TestData.raw, cases(k).app_id, cases(k).version);
		testCase.verifyEqual(made, matlab.net.base64decode(cases(k).unsigned_b64), ...
			sprintf('App ID %s, version %s', cases(k).app_id, cases(k).version));
	end
end

function test_package_keeps_every_byte_value(testCase)
	raw = uint8([0:255, 255:-1:0, repmat(200, 1, 300)]);
	made = go_s1_package(raw, '', '0.0.0');
	testCase.verifyEqual(i_inflate(made(59:end)), raw, 'the payload behind the 58-byte header');
	testCase.verifyEqual(typecast(made(21:24), 'uint32'), uint32(numel(raw)), 'the uncompressed size');
end

function test_package_refuses_what_does_not_fit(testCase)
	raw = testCase.TestData.raw;
	testCase.verifyError(@() go_s1_package(raw, repmat('A', 1, 33), '1.2.3'), 'go_s1_package:appId');
	testCase.verifyError(@() go_s1_package(raw, 'GOCO', '1.2'), 'go_s1_package:version');
	testCase.verifyError(@() go_s1_package(raw, 'GOCO', '1.2.256'), 'go_s1_package:version');
	testCase.verifyError(@() go_s1_package(zeros(1, hex2dec('1F8001'), 'uint8'), '', '1.2.3'), 'go_s1_package:tooLarge');
end

%% The upload %%

function test_blank_controller_is_programmed(testCase)
	[radio, seen] = i_controller(struct('serial', 'A4CG-B034-A003-A002'));
	evalc('go_s1_upload(testCase.TestData.bin, radio=radio)');
	got = seen();
	testCase.verifyEqual(got.container, go_s1_package(testCase.TestData.raw, '', '1.2.3'));
	testCase.verifyEqual(got.control, {1, [5, double(char(datetime('today', 'Format', 'yyyy-MM-dd')))], 2}, 'START, RELEASE_DATE, END');
	testCase.verifyTrue(got.closed);
end

function test_package_carries_the_app_id_of_the_controller(testCase)
	[radio, seen] = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'appId', 'GS-PUMP-01'));
	evalc('go_s1_upload(testCase.TestData.bin, radio=radio, version=''2.0.1'')');
	got = seen();
	testCase.verifyEqual(got.container, go_s1_package(testCase.TestData.raw, 'GS-PUMP-01', '2.0.1'));
end

function test_old_esp_firmware_gets_no_release_date(testCase)
	[radio, seen] = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'esp', [1 1 0]));
	evalc('go_s1_upload(testCase.TestData.bin, radio=radio)');
	got = seen();
	testCase.verifyEqual(got.control, {1, 2});
end

function test_notifications_that_do_not_arrive(testCase)
	% The state comes in thirds and COMPLETE not at all: the App ID is still the controller's.
	[radio, seen] = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'appId', 'GS-PUMP-01', 'lossy', true));
	evalc('go_s1_upload(testCase.TestData.bin, radio=radio)');
	got = seen();
	testCase.verifyEqual(got.container, go_s1_package(testCase.TestData.raw, 'GS-PUMP-01', '1.2.3'));
end

function test_last_phase_that_does_not_arrive(testCase)
	% No "done" from the controller: after 20 s of silence it is asked what it runs.
	radio = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'phases', [1 0; 3 100], 'after', [1 2 3]));
	said = evalc('go_s1_upload(testCase.TestData.bin, radio=radio)');
	testCase.verifySubstring(said, 'Upload finished');
end

function test_controller_with_a_public_key_is_refused_before_sending(testCase)
	[radio, seen] = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'publicKey', true));
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio)), 'go_s1_upload:signed');
	got = seen();
	testCase.verifyEmpty(got.control);
	testCase.verifyTrue(got.closed);
end

function test_bluetooth_updates_switched_off(testCase)
	radio = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'updateBle', false));
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio)), 'go_s1_upload:switchedOff');
end

function test_controller_that_refuses_the_package(testCase)
	radio = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'phases', [1 0; 7 0]));
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio)), 'go_s1_upload:refused');
end

function test_controller_that_is_busy(testCase)
	radio = i_controller(struct('serial', 'A4CG-B034-A003-A002', 'busy', true));
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio)), 'go_s1_upload:busy');
end

function test_no_controller_in_range(testCase)
	radio = i_controller(struct('serial', {}));
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio)), 'go_s1_upload:notFound');
end

function test_several_in_range_need_a_choice(testCase)
	[radio, seen] = i_controller(struct('serial', {'A4CG-B034-A003-A001', 'A4CG-B034-A003-A002'}));
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio)), 'go_s1_upload:several');
	got = seen();
	testCase.verifyEmpty(got.opened, 'none was connected to');
end

function test_the_serial_number_chooses(testCase)
	[radio, seen] = i_controller(struct('serial', {'A4CG-B034-A003-A001', 'A4CG-B034-A003-A002'}));
	evalc('go_s1_upload(testCase.TestData.bin, radio=radio, controller=''a4cg-b034-a003-a002'')');
	got = seen();
	testCase.verifyEqual(got.programmed, "AABBCCDDEE02");
	testCase.verifyError(@() i_quiet(@() go_s1_upload(testCase.TestData.bin, radio=radio, controller='A4CG-0000-0000-0000')), ...
		'go_s1_upload:notFound');
end

function test_the_bluetooth_address_chooses_without_a_scan(testCase)
	[radio, seen] = i_controller(struct('serial', {'A4CG-B034-A003-A001', 'A4CG-B034-A003-A002'}));
	evalc('go_s1_upload(testCase.TestData.bin, radio=radio, controller=''aa:bb:cc:dd:ee:01'')');
	got = seen();
	testCase.verifyEqual(got.programmed, "AABBCCDDEE01");
	testCase.verifyEqual(got.scans, 0);
end

function test_file_that_is_not_there(testCase)
	radio = i_controller(struct('serial', 'A4CG-B034-A003-A002'));
	testCase.verifyError(@() go_s1_upload('nowhere_1.2.3.bin', radio=radio), 'go_s1_upload:noFile');
end

%% Helpers %%

function i_quiet(call)
	[~] = evalc('call()');
end

function raw = i_inflate(zlib)
	bytes = typecast(uint8(zlib), 'int8'); % a Java byte is signed
	stream = java.util.zip.InflaterInputStream(java.io.ByteArrayInputStream(bytes));
	out = java.io.ByteArrayOutputStream();
	com.mathworks.mlwidgets.io.InterruptibleStreamCopier.getInterruptibleStreamCopier().copyStream(stream, out);
	raw = typecast(out.toByteArray(), 'uint8')';
end

function [radio, seen] = i_controller(units)
% A radio with the Moduline S1 controllers UNITS in range (a struct array; serial is
% required). What a unit does follows s1-ble-gatt-profile section 1 of the Knowledge Base:
% the INFO state after subscribing, STATUS after START and END, INFO 0x17 while it programs.
%   appId      the App ID in the controller (default none: a blank controller)
%   esp        the ESP interface version (default [2 2 0])
%   publicKey  true: the controller carries a public key
%   updateBle  false: updating over Bluetooth is switched off
%   busy       true: START is answered with ERROR
%   phases     rows [phase percentage] sent after END (default: prepare, erase, write, done)
%   after      the application version the controller reports after END (default: unchanged)
%   lossy      true: of every INFO state only a third arrives, another third each time, and
%              STATUS COMPLETE does not arrive at all: what MATLAB's Bluetooth does to
%              notifications that come right behind each other
% seen() returns what happened.
	log = struct('container', uint8([]), 'control', {{}}, 'closed', false, 'opened', strings(1, 0), ...
		'programmed', "", 'scans', 0);
	addresses = compose("AABBCCDDEE%02d", 1:numel(units));
	radio.scan = @doScan;
	radio.open = @doOpen;
	seen = @report;

	function happened = report()
		happened = log;
	end

	function found = doScan()
		log.scans = log.scans + 1;
		found = addresses;
	end

	function gatt = doOpen(address)
		unit = units(addresses == string(address));
		log.opened(end+1) = string(address);
		log.closed = false;
		queue = {};
		asked = 0;                 % how often INFO was subscribed to
		application = [0 9 0];     % the version of the application in the controller
		gatt.subscribe = @doSubscribe;
		gatt.unsubscribe = @(~) [];
		gatt.write = @doWrite;
		gatt.poll = @doPoll;
		gatt.close = @doClose;

		function doSubscribe(name)
			if ~strcmp(name, 'info'), return, end
			asked = asked + 1;
			state = {11, option('esp', [2 2 0]); 1, application; 24, double(unit.serial); ...
				5, [112 48]; ...                 % KL15 voltage: a type the upload does not use
				51, double(option('appId', '')); 52, double(option('publicKey', false)); ...
				56, double(option('updateBle', true))};
			if option('publicKey', false), state(end+1, :) = {54, [4, zeros(1, 64)]}; end
			for k = 1:size(state, 1)
				if ~option('lossy', false) || mod(k + asked, 3) == 0, tlv(state{k, :}); end
			end
		end

		function doWrite(name, bytes, ~)
			bytes = double(bytes);
			if strcmp(name, 'data')
				log.container = [log.container, uint8(bytes)];
				return
			end
			log.control{end+1} = bytes;
			switch bytes(1)
				case 1 % START
					log.container = uint8([]);
					if option('busy', false), queue{end+1} = {'status', 255}; else, queue{end+1} = {'status', 1}; end
				case 2 % END
					if ~option('lossy', false), queue{end+1} = {'status', 3}; end
					phases = option('phases', [1 0; 2 50; 2 100; 3 40; 3 100; 4 100]);
					for k = 1:size(phases, 1), tlv(23, phases(k, :)); end
					application = option('after', application);
					log.programmed = string(address);
			end
		end

		function items = doPoll()
			items = queue;
			queue = {};
		end

		function doClose()
			log.closed = true;
		end

		function tlv(type, payload)
			queue{end+1} = {'info', uint8([type, numel(payload), payload])};
		end

		function value = option(name, fallback)
			if isfield(unit, name) && ~isempty(unit.(name)), value = unit.(name); else, value = fallback; end
		end
	end
end
