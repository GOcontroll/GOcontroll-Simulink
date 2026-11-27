function tests = go_other_blocksets_test
% Tests of go_other_blocksets, the warning when MATLAB also reaches another copy of the
% blockset. The other copies are made up (targetFiles=, libraryFiles=), so the tests do not
% depend on what is installed on this computer.
%
%   cd <repository>; librarySetup; results = runtests('test/go_other_blocksets_test.m')
	tests = functiontests(localfunctions);
end

function setupOnce(testCase)
	testCase.TestData.repo = fileparts(fileparts(mfilename('fullpath')));
	folder = tempname; mkdir(folder);
	testCase.addTeardown(@() rmdir(folder, 's'));
	testCase.TestData.folder = folder;
end

function test_only_this_copy_gives_no_warning(testCase)
	repo = testCase.TestData.repo;
	others = testCase.verifyWarningFree(@() go_other_blocksets(repo, ...
		targetFiles={i_tlc(repo)}, libraryFiles={i_lib(repo)}));
	testCase.verifyEmpty(others);
end

function test_this_copy_written_differently_gives_no_warning(testCase)
	repo = testCase.TestData.repo;
	written = [strrep(repo, '\', '/') '/'];
	if ispc, written = upper(written); end
	testCase.verifyWarningFree(@() go_other_blocksets(repo, targetFiles={i_tlc(written)}, libraryFiles={}));
end

function test_copy_on_the_path_is_named(testCase)
	other = i_copy(testCase, 'clone', 'GOcontroll V4.9.9');
	others = testCase.verifyWarning(@() go_other_blocksets(testCase.TestData.repo, ...
		targetFiles={i_tlc(testCase.TestData.repo), i_tlc(other)}, libraryFiles={}), 'GOcontroll:otherBlockset');
	testCase.verifyEqual(numel(others), 1);
	testCase.verifyEqual(others.root, other);
	testCase.verifyEqual(others.version, 'V4.9.9');
	testCase.verifyFalse(others.addOn);
end

function test_library_from_another_copy_is_named(testCase)
	other = i_copy(testCase, 'library', 'GOcontroll V5.0.0');
	others = testCase.verifyWarning(@() go_other_blocksets(testCase.TestData.repo, ...
		targetFiles={i_tlc(testCase.TestData.repo)}, libraryFiles={i_lib(other)}), 'GOcontroll:otherBlockset');
	testCase.verifyEqual({others.root}, {other});
end

function test_one_copy_is_named_once(testCase)
	other = i_copy(testCase, 'twice', 'GOcontroll V5.0.0');
	others = testCase.verifyWarning(@() go_other_blocksets(testCase.TestData.repo, ...
		targetFiles={i_tlc(other)}, libraryFiles={i_lib(other), i_lib(other)}), 'GOcontroll:otherBlockset');
	testCase.verifyEqual(numel(others), 1);
end

function test_copy_without_version_file(testCase)
	other = fullfile(testCase.TestData.folder, 'no-version');
	others = testCase.verifyWarning(@() go_other_blocksets(testCase.TestData.repo, ...
		targetFiles={i_tlc(other)}, libraryFiles={}), 'GOcontroll:otherBlockset');
	testCase.verifyEqual(others.version, 'unknown version');
end

function test_addon_is_recognised_and_named(testCase)
	other = i_copy(testCase, fullfile('MATLABadd_ons_2023b', 'Collections', 'GOcontroll-Simulink'), 'GOcontroll V5.0.0');
	lastwarn('');
	evalc('others = go_other_blocksets(testCase.TestData.repo, targetFiles={i_tlc(other)}, libraryFiles={});');
	[message, id] = lastwarn();
	testCase.verifyEqual(id, 'GOcontroll:otherBlockset');
	testCase.verifyTrue(others.addOn);
	testCase.verifySubstring(message, other);
	testCase.verifySubstring(message, 'V5.0.0');
	testCase.verifySubstring(message, '(Add-On)');
	testCase.verifySubstring(message, 'Manage Add-Ons');
end

%% Helpers %%

% A made-up copy of the blockset in the test folder, with a version file.
function root = i_copy(testCase, name, version)
	root = fullfile(testCase.TestData.folder, name);
	mkdir(fullfile(root, 'blockset'));
	file = fopen(fullfile(root, 'blockset', 'GOcontroll_Simulink_version.m'), 'w');
	fprintf(file, 'function [version] = GOcontroll_Simulink_version()\n\nversion = ''%s'';\n', version);
	fclose(file);
end

function file = i_tlc(root)
	file = fullfile(root, 'blockset', 'ert', 'ert_gocontroll.tlc');
end

function file = i_lib(root)
	file = fullfile(root, 'blockset', 'blocks', 'GOcontroll_Generic.mdl');
end
