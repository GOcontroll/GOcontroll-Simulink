function tests = go_codebase_cache_test
% Tests of go_codebase_cache, the cache behind Use precompiled binary, on a made-up
% CodeBase in a temporary folder: no compiler and no model are needed.
%
%   cd <repository>; librarySetup; results = runtests('test/go_codebase_cache_test.m')
	tests = functiontests(localfunctions);
end

function setup(testCase)
	root = tempname; mkdir(root);
	testCase.addTeardown(@() rmdir(root, 's'));
	code = fullfile(root, 'code');
	mkdir(fullfile(code, 'modules'));
	i_write(fullfile(code, 'GO_board.c'), '#include "GO_board.h"');
	i_write(fullfile(code, 'GO_board.h'), '#define GO_BOARD 1');
	i_write(fullfile(code, 'GO_xcp.c'), '#include "SYS_config.h"');
	i_write(fullfile(code, 'GO_communication_mqtt.c'), 'int mqtt;');
	i_write(fullfile(code, 'modules', 'GO_module_input.c'), 'int input;');
	testCase.TestData.root = root;
	testCase.TestData.code = code;
	% The objects a build left in its build folder
	objects = fullfile(root, 'objects');
	mkdir(objects);
	for name = {'GO_board', 'GO_xcp', 'GO_module_input'}
		i_write(fullfile(objects, [name{1} '.obj']), name{1});
	end
	testCase.TestData.objects = objects;
	testCase.TestData.spec = struct( ...
		'buildDir',   fullfile(root, 'build'), ...
		'key',        'Linux-GCC', ...
		'codeRoot',   code, ...
		'srcDirs',    {{code, fullfile(code, 'modules')}}, ...
		'exclude',    {{'GO_communication_mqtt.c'}}, ...
		'buildFiles', {{fullfile(code, 'GO_board.h')}}, ...
		'settings',   'gcc -O3');
end

function test_sources_are_split_and_excluded(testCase)
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyEqual(sort({cache.sources.name}), {'GO_board.c', 'GO_module_input.c'});
	testCase.verifyEqual({cache.perBuild.name}, {'GO_xcp.c'}, 'a source with SYS_config.h is compiled every build');
	testCase.verifyEqual(cache.dir, fullfile(testCase.TestData.root, 'build', 'codebase_objs', 'Linux-GCC'));
end

function test_empty_cache_is_not_usable(testCase)
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyFalse(cache.usable);
	testCase.verifySubstring(cache.reason, 'no cached CodeBase objects yet');
end

function test_stored_cache_is_usable(testCase)
	i_store(testCase);
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyTrue(cache.usable);
	testCase.verifyEqual(cache.reason, '');
	stored = dir(fullfile(cache.dir, '*.obj'));
	testCase.verifyEqual(sort({stored.name}), {'GO_board.obj', 'GO_module_input.obj'}, 'GO_xcp.obj is not cached');
end

function test_changed_header_makes_the_cache_again(testCase)
	i_store(testCase);
	i_write(fullfile(testCase.TestData.code, 'GO_board.h'), '#define GO_BOARD 22');
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyFalse(cache.usable);
	testCase.verifySubstring(cache.reason, 'GO_board.h');
end

function test_new_source_makes_the_cache_again(testCase)
	i_store(testCase);
	i_write(fullfile(testCase.TestData.code, 'modules', 'GO_module_output.c'), 'int output;');
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyFalse(cache.usable);
	testCase.verifySubstring(cache.reason, 'GO_module_output.c');
end

function test_removed_source_makes_the_cache_again(testCase)
	i_store(testCase);
	delete(fullfile(testCase.TestData.code, 'modules', 'GO_module_input.c'));
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyFalse(cache.usable);
	testCase.verifySubstring(cache.reason, 'GO_module_input.c');
end

function test_other_settings_make_the_cache_again(testCase)
	i_store(testCase);
	spec = testCase.TestData.spec;
	spec.settings = 'gcc -O2';
	cache = go_codebase_cache('open', spec);
	testCase.verifyFalse(cache.usable);
	testCase.verifySubstring(cache.reason, 'the compiler settings');
end

function test_each_platform_has_its_own_cache(testCase)
	i_store(testCase);
	spec = testCase.TestData.spec;
	spec.key = 'STM-GCC';
	cache = go_codebase_cache('open', spec);
	testCase.verifyFalse(cache.usable, 'objects for a Linux controller are no objects for the Moduline S1');
	testCase.verifyTrue(go_codebase_cache('open', testCase.TestData.spec).usable, 'the Linux cache stays');
end

function test_missing_object_is_not_cached(testCase)
	delete(fullfile(testCase.TestData.objects, 'GO_module_input.obj'));
	cache = go_codebase_cache('open', testCase.TestData.spec);
	objects = testCase.TestData.objects;
	testCase.verifyWarning(@() go_codebase_cache('store', cache, objects), 'GOcontroll:codebaseCache');
	testCase.verifyFalse(go_codebase_cache('open', testCase.TestData.spec).usable);
end

function test_object_missing_from_the_cache(testCase)
	i_store(testCase);
	cache = go_codebase_cache('open', testCase.TestData.spec);
	delete(fullfile(cache.dir, 'GO_board.obj'));
	cache = go_codebase_cache('open', testCase.TestData.spec);
	testCase.verifyFalse(cache.usable);
	testCase.verifySubstring(cache.reason, 'GO_board.obj');
end

function test_cache_of_an_older_blockset_is_removed(testCase)
	flat = fullfile(testCase.TestData.spec.buildDir, 'codebase_objs');
	mkdir(flat);
	i_write(fullfile(flat, 'GO_board.obj'), 'old');
	evalc('go_codebase_cache(''open'', testCase.TestData.spec)');
	testCase.verifyFalse(isfile(fullfile(flat, 'GO_board.obj')));
end

%% Helpers %%

function i_store(testCase)
	cache = go_codebase_cache('open', testCase.TestData.spec);
	evalc('go_codebase_cache(''store'', cache, testCase.TestData.objects)');
end

function i_write(file, text)
	f = fopen(file, 'w');
	fwrite(f, text);
	fclose(f);
end
