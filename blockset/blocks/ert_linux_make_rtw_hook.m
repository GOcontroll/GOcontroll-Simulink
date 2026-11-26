%%***************************************************************************************
%% file         ert_linux_make_rtw_hook.m
%% brief        Hook file with functionality that is invoked during the build process
%%              of an embedded coder real-time target.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2018 (c) by HAN Automotive     http://www.han.nl     All rights reserved
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
%%
%%---------------------------------------------------------------------------------------
%%                            L I C E N S E
%%---------------------------------------------------------------------------------------
%% This file is part of the HANcoder Matlab/Simulink Blockset environment. For the
%% licensing terms, please contact HAN Automotive.
%%
%% This software has been carefully tested, but is not guaranteed for any particular
%% purpose. HAN Automotive does not offer any warranties and does not guarantee the
%% accuracy, adequacy, or completeness of the software and is not responsible for any
%% errors or omissions or the results obtained from use of the software.
%%
%%***************************************************************************************
function ert_linux_make_rtw_hook(hookMethod,modelName,rtwroot,templateMakefile,buildOpts,buildArgs,buildInfo)
% ERT_MAKE_RTW_HOOK - This is the standard ERT hook file for the RTW build
% process (make_rtw), and implements automatic configuration of the
% models configuration parameters.  When the buildArgs option is specified
% as 'optimized_fixed_point=1' or 'optimized_floating_point=1', the model
% is configured automatically for optimized code generation.
%
% This hook file (i.e., file that implements various RTW callbacks) is
% called by RTW for system target file ert.tlc.  The file leverages
% strategic points of the RTW process.  A brief synopsis of the callback
% API is as follows:
%
% ert_make_rtw_hook(hookMethod, modelName, rtwroot, templateMakefile,
%                   buildOpts, buildArgs)
%
% hookMethod:
%   Specifies the stage of the RTW build process.  Possible values are
%   entry, before_tlc, after_tlc, before_make, after_make and exit, etc.
%
% modelName:
%   Name of model.  Valid for all stages.
%
% rtwroot:
%   Reserved.
%
% templateMakefile:
%   Name of template makefile.  Valid for stages 'before_make' and 'exit'.
%
% buildOpts:
%   Valid for stages 'before_make' and 'exit', a MATLAB structure
%   containing fields
%
%   modules:
%     Char array specifying list of generated C files: model.c, model_data.c,
%     etc.
%
%   codeFormat:
%     Char array containing code format: 'RealTime', 'RealTimeMalloc',
%     'Embedded-C', and 'S-Function'
%
%   noninlinedSFcns:
%     Cell array specifying list of non-inlined S-Functions.
%
%   compilerEnvVal:
%     String specifying compiler environment variable value, e.g.,
%     D:\Applications\Microsoft Visual
%
% buildArgs:
%   Char array containing the argument to make_rtw.  When pressing the build
%   button through the Configuration Parameter Dialog, buildArgs is taken
%   verbatim from whatever follows make_rtw in the make command edit field.
%   From MATLAB, it's whatever is passed into make_rtw.  For example, its
%   'optimized_fixed_point=1' for make_rtw('optimized_fixed_point=1').
%
%   This file implements these buildArgs:
%     optimized_fixed_point=1
%     optimized_floating_point=1
%
% You are encouraged to add other configuration options, and extend the
% various callbacks to fully integrate ERT into your environment.

persistent stationID;
persistent model_path;
persistent codebase_src_dirs_stm gccpath_stm;
switch hookMethod
	case 'error'
		fprintf('########################### ERROR\n');
		if isappdata(0, 'GOcontrollBuildRunning'), rmappdata(0, 'GOcontrollBuildRunning'); end
		% Called if an error occurs anywhere during the build.  If no error occurs
		% during the build, then this hook will not be called.  Valid arguments
		% at this stage are hookMethod and modelName. This enables cleaning up
		% any static or global data used by this hook file.
		disp(['### Real-Time Workshop build procedure for model: ''' modelName...
			''' aborted due to an error.']);
		
	case 'entry'
		fprintf('########################### ENTRY\n');
		% Tells goc_deleteBuildFiles that a build is running, see there.
		setappdata(0, 'GOcontrollBuildRunning', now); %#ok<TNOW1>
		% Stop when the Target does not belong to the platform this model is locked to.
		go_platform_lock_check(modelName);
		% Word sizes of the platform: a model made with an older blockset can still
		% have the 32-bit device type while it is built for a 64-bit Linux controller.
		go_target_hardware(modelName);
		% Called at start of code generation process (before anything happens.)
		% Valid arguments at this stage are hookMethod, modelName, and buildArgs.
		
		% The following warning must be given if the evaluation version is
		% used, comment for licensed use.
		%uiwait(warndlg('This version is for evaluation purposes only','HANcoder','modal'));
		% The following warning is given for the student version, comment for
		% licensed use.
		%uiwait(warndlg(sprintf('You are using an educational version of HANcoder\n\nCommercial usage is not allowed in any way\n\nContact hancoder@han.nl for more information'),'HANcoder','modal'));
		
		% Check if the code generation is started from the correct path
		
		model_path = get_param(bdroot, 'FileName');
		model_path = regexprep(model_path, ['\' filesep modelName '.slx'],''); %windows needs the '\' and linux seems fine with or without it
		model_path = regexprep(model_path, ['\' filesep modelName '.mdl'],'');
		
		% Check if this is a referenced model
		mdlRefTargetType = get_param(modelName,'ModelReferenceTargetType');
		isNotModelRefTarget = strcmp(mdlRefTargetType, 'NONE'); % NONE, SIM, or RTW
		
		%check if a custom setting is set for the code gen folder or a referenced model, if so don't perform this check
		if (~strcmp(pwd, get_param(0, 'CodeGenFolder')) || isNotModelRefTarget)
			if (~(strcmp(pwd,model_path)))
				errorMessage = strcat('The current folder is incorrect, please', ...
					' set the current folder to: ', model_path,...
					' instead of: ', pwd);
				error(errorMessage);
			end
		end
		
		fprintf(['\n### Starting Real-Time Workshop build procedure for ', ...
			'model: %s\n'],modelName);
		
	case 'before_tlc'
		% Called just prior to invoking TLC Compiler (actual code generation.)
		% Valid arguments at this stage are hookMethod, modelName, and
		% buildArgs



	case 'after_tlc'
		% Called just after to invoking TLC Compiler (actual code generation.)
		% Valid arguments at this stage are hookMethod, modelName, and
		% buildArgs
		%% get the necessary information from the model
		
		nrOfCANreceiveBlocks = searchCANreceive(modelName);
		% Add software version to the SYS_config.h file.
		fprintf('\n### Adding data to SYS_config.h...\n');
		formatOut = 'ddMMyy_HHmmSS';
		daten = char(datetime('now','Format', formatOut));
		stationID = strcat(modelName,daten); %assign to static variable as it is needed in the after_make step
		% Get XCP port
		XCPport = get_param(modelName,'tlcXcpTcpPort');

		% Open file in write mode 'w'
		file = fopen('SYS_config.h', 'w');
		if file == -1
			error('### failed to open SYS_config.h');
		end
		if numel(stationID) > 255
			msg = 'Error: Station ID is larger than 255 characters. Use a shorter name as ID!\n';
			% Display error message in the matlab command window.
			fprintf(msg);
			% Abort and display pop-up window with error message.
			error(msg);
		end
		fprintf(file, '#ifndef __SYS_CONFIG_H__ \n#define __SYS_CONFIG_H__\n');
		fprintf(file, '#define kXcpStationIdString            "%s"\n', stationID);
		fprintf(file, '#define kXcpStationIdLength            %d\n', numel(stationID));
		fprintf(file, '#define XCP_PORT_NUM                   %d\n', XCPport);
		fprintf(file, '#define CANBUFSIZE                     %d\n', nrOfCANreceiveBlocks);
		fprintf(file, '#endif');
		fclose(file);
		
	case 'before_make'
		% Called after code generation is complete, and just prior to kicking
		% off make process (assuming code generation only is not selected.)  All
		% arguments are valid at this stage.
		delete('*.obj')
		%get the path of this script so we can add include paths and such
		mfilePath = mfilename('fullpath');
		if contains(mfilePath,'LiveEditorEvaluationHelper')
			mfilePath = matlab.desktop.editor.getActiveFilename;
		end
		%remove the filename from the end
		[path, ~, ~] = fileparts(mfilePath);
		
		%set the compiler path in the tmf
		target = go_target_platform(get_param(modelName, 'tlcGOcontrollTarget'));
		if (~strcmp(target, 'STM'))
			if (strcmp(get_param(modelName, 'tlcLinuxCompiler'), 'GCC'))
				gccpath = getInstallationLocation('aarch64-none-linux-gnu-gcc');
				addTMFTokens(buildInfo, '|>CC<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-gcc'),    'LINK_INFO');
				addTMFTokens(buildInfo, '|>AS<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-as'),     'LINK_INFO');
				addTMFTokens(buildInfo, '|>AR<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-ar'),     'LINK_INFO');
				addTMFTokens(buildInfo, '|>LD<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-gcc'),    'LINK_INFO');
				addTMFTokens(buildInfo, '|>OC<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-objcopy'),'LINK_INFO');
				addTMFTokens(buildInfo, '|>OD<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-objdump'),'LINK_INFO');
				addTMFTokens(buildInfo, '|>SZ<|', makeToolPath(gccpath, 'aarch64-none-linux-gnu-size'),   'LINK_INFO');
				addCompileFlags(buildInfo, '-mcpu=cortex-a53 -Wa,-adhlns="$@.lst" -Wno-maybe-uninitialized -gdwarf -DGOCONTROLL_LINUX');
				addLinkFlags(buildInfo, '-Wl,-Map,$(BIN_PATH)/$(MODEL_NAME).map')
			elseif (strcmp(get_param(modelName, 'tlcLinuxCompiler'), 'Zig'))
				if (~strcmp(computer(), "MACA64")) %if not apple silicon it is x86
					zigpath = getInstallationLocation('Zig-x86');
				else %is apple silicon, use aarch64 possibly in the future also windows and linux
					zigpath = getInstallationLocation('Zig-aarch64');
				end
				zigpath = ['"' fullfile(zigpath, 'zig') '"'];
				addTMFTokens(buildInfo, '|>CC<|', [zigpath ' cc -target aarch64-linux-gnu.2.31'],'LINK_INFO');
				addTMFTokens(buildInfo, '|>AS<|', '','LINK_INFO');
				addTMFTokens(buildInfo, '|>AR<|', [zigpath ' ar'],'LINK_INFO');
				addTMFTokens(buildInfo, '|>LD<|', [zigpath ' cc -target aarch64-linux-gnu.2.31'],'LINK_INFO');
				addTMFTokens(buildInfo, '|>OC<|', [zigpath ' oc'],'LINK_INFO');
				addTMFTokens(buildInfo, '|>OD<|', '','LINK_INFO');
				addTMFTokens(buildInfo, '|>SZ<|', '','LINK_INFO');
				% GOCONTROLL_LINUX selects the Linux code of the CodeBase, as with GCC.
				addCompileFlags(buildInfo, '-mcpu=cortex_a53 -gdwarf64 -DGOCONTROLL_LINUX');
			else
				error("No valid compiler selected");
			end
			
			%add the source code/libraries to the tmf
			codepath = fullfile(path, '..', 'GOcontroll-CodeBase', 'code');
			modulepath = fullfile(codepath, 'modules');
			addIncludePaths(buildInfo, {codepath, modulepath});
			% The CodeBase no longer carries prebuilt libraries (OAES, libiio, JSON-C) and
			% needs none of them. GO_communication_mqtt.c is left out: on Linux it needs
			% libmosquitto, and the MQTT blocks of this blockset are for the Moduline S1 only.
			codeFiles = dir(fullfile(codepath, '*.c'));
			codeFiles = {codeFiles.name};
			codeFiles = codeFiles(~strcmp(codeFiles, 'GO_communication_mqtt.c'));
			addSourceFiles(buildInfo, codeFiles, codepath);
			addSourceFiles(buildInfo, '*.c', modulepath);


		elseif (strcmp(target, 'STM'))
			if (strcmp(get_param(modelName, 'tlcLinuxCompiler'), 'GCC'))
				gccpath = getInstallationLocation('arm-none-eabi-gcc');
				addTMFTokens(buildInfo, '|>CC<|', makeToolPath(gccpath, 'arm-none-eabi-gcc'),    'LINK_INFO');
				addTMFTokens(buildInfo, '|>AS<|', makeToolPath(gccpath, 'arm-none-eabi-as'),     'LINK_INFO');
				addTMFTokens(buildInfo, '|>AR<|', makeToolPath(gccpath, 'arm-none-eabi-ar'),     'LINK_INFO');
				addTMFTokens(buildInfo, '|>LD<|', makeToolPath(gccpath, 'arm-none-eabi-gcc'),    'LINK_INFO');
				addTMFTokens(buildInfo, '|>OC<|', makeToolPath(gccpath, 'arm-none-eabi-objcopy'),'LINK_INFO');
				addTMFTokens(buildInfo, '|>OD<|', makeToolPath(gccpath, 'arm-none-eabi-objdump'),'LINK_INFO');
				addTMFTokens(buildInfo, '|>SZ<|', makeToolPath(gccpath, 'arm-none-eabi-size'),   'LINK_INFO');
				addCompileFlags(buildInfo, '-mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wall -Wno-maybe-uninitialized -g -DGOCONTROLL_STM');
				addLinkFlags(buildInfo, '-Wl,-Map,$(BIN_PATH)/$(MODEL_NAME).map -Wl,--gc-sections -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float')
			elseif (strcmp(get_param(modelName, 'tlcLinuxCompiler'), 'Zig'))
				error("not implemented (yet?) switch to gcc compiler");
			else
				error("No valid compiler selected");
			end
			
			%add the source code/libraries to the tmf
			codepath    = fullfile(path, '..', 'GOcontroll-CodeBase', 'code');
			modulepath  = fullfile(codepath, 'modules');
			stmpath     = fullfile(codepath, 'stm');
			corepath    = fullfile(stmpath, 'Core');
			driverspath = fullfile(stmpath, 'Drivers');
			rtospath    = fullfile(stmpath, 'Middlewares', 'Third_Party', 'FreeRTOS', 'Source');
			seggerpath  = fullfile(driverspath, 'segger');

			addIncludePaths(buildInfo, {
				codepath,
				modulepath,
				stmpath,
				fullfile(corepath, 'Inc'),
				fullfile(driverspath, 'CMSIS', 'Include'),
				fullfile(driverspath, 'CMSIS', 'Device', 'ST', 'STM32H5xx', 'Include'),
				fullfile(driverspath, 'STM32H5xx_HAL_Driver', 'Inc'),
				fullfile(driverspath, 'STM32H5xx_HAL_Driver', 'Inc','Legacy'),
				fullfile(rtospath, 'CMSIS_RTOS_V2'),
				fullfile(rtospath, 'include'),
				fullfile(rtospath, 'portable', 'MemMang'),
				fullfile(rtospath, 'portable', 'GCC', 'ARM_CM33_NTZ', 'non_secure'),
				fullfile(seggerpath, 'Inc'),
			});
			addSourceFiles(buildInfo, '*.s', fullfile(corepath, 'Startup'));
			addLinkFlags(buildInfo, ['-T' fullfile(stmpath, 'STM32H573RITX_FLASH.ld')]);

			% Reuse pre-compiled CodeBase library when available; compile on first build.
			gccpath_stm = gccpath;
			cbaseSourceDirs = {
				codepath,
				modulepath,
				stmpath,
				fullfile(corepath, 'Src'),
				fullfile(driverspath, 'STM32H5xx_HAL_Driver', 'Src'),
				rtospath,
				fullfile(rtospath, 'CMSIS_RTOS_V2'),
				fullfile(rtospath, 'portable', 'MemMang'),
				fullfile(rtospath, 'portable', 'GCC', 'ARM_CM33_NTZ', 'non_secure'),
				fullfile(seggerpath, 'Src'),
			};
			usePrecompiled = strcmp(get_param(modelName, 'tlcUsePrecompiledLib'), 'Use precompiled binary');
			objCacheDir = fullfile(model_path, 'build', 'codebase_objs');
			cachedObjs = dir(fullfile(objCacheDir, '*.obj'));
			hasPrecompiled = usePrecompiled && ~isempty(cachedObjs);
			if ~hasPrecompiled
				if ~usePrecompiled
					disp('### Precompiled binary disabled -- compiling CodeBase from source...');
				else
					disp('### No cached CodeBase objects found -- compiling CodeBase (first build only)...');
				end
				codebase_src_dirs_stm = cbaseSourceDirs;
				addSourceFiles(buildInfo, '*.c', codepath);
				addSourceFiles(buildInfo, '*.c', modulepath);
				addSourceFiles(buildInfo, '*.c', stmpath);
				addSourceFiles(buildInfo, '*.c', fullfile(corepath, 'Src'));
				removeSourceFiles(buildInfo, 'main.c');
				addSourceFiles(buildInfo, '*.c', fullfile(driverspath, 'STM32H5xx_HAL_Driver', 'Src'));
				addSourceFiles(buildInfo, '*.c', rtospath);
				addSourceFiles(buildInfo, '*.c', fullfile(rtospath, 'CMSIS_RTOS_V2'));
				addSourceFiles(buildInfo, '*.c', fullfile(rtospath, 'portable', 'MemMang'));
				addSourceFiles(buildInfo, '*.c', fullfile(rtospath, 'portable', 'GCC', 'ARM_CM33_NTZ', 'non_secure'));
				addSourceFiles(buildInfo, '*.c', fullfile(seggerpath, 'Src'));
			else
				codebase_src_dirs_stm = {};
				disp(['### Found ' num2str(numel(cachedObjs)) ' cached CodeBase objects -- skipping source compile.']);
				% Remove stale CodeBase sources from buildInfo (do not compile them again)
				for d = 1:numel(cbaseSourceDirs)
					files = dir(fullfile(cbaseSourceDirs{d}, '*.c'));
					for k = 1:numel(files)
						removeSourceFiles(buildInfo, files(k).name);
					end
				end
				% Sources that include SYS_config.h carry this build's station ID, so they
				% are compiled every build instead of taken from the cache.
				% A cache made before this rule still holds their objects; those
				% are removed, otherwise they are linked twice.
				perBuildSrcs = findPerBuildSources(cbaseSourceDirs);
				for k = 1:numel(perBuildSrcs)
					staleObj = fullfile(objCacheDir, regexprep(perBuildSrcs(k).name, '\.c$', '.obj'));
					if isfile(staleObj)
						delete(staleObj);
						disp(['### Removed ' regexprep(perBuildSrcs(k).name, '\.c$', '.obj') ' from the CodeBase cache (compiled every build)']);
					end
				end
				cachedObjs = dir(fullfile(objCacheDir, '*.obj'));
				% Link each cached .obj directly -- avoids the --whole-archive trick
				% and prevents multiple-definition errors caused by .a double-linking.
				for k = 1:numel(cachedObjs)
					addLinkObjects(buildInfo, cachedObjs(k).name, objCacheDir, 1000, true, true);
				end
				for k = 1:numel(perBuildSrcs)
					addSourceFiles(buildInfo, perBuildSrcs(k).name, perBuildSrcs(k).folder);
				end
			end

		else
			error("No valid target selected");
		end
		
	case 'after_make'
		% Called after make process is complete. All arguments are valid at
		% this stage.
		
		% Check if this is the main model
		mdlRefTargetType = get_param(modelName,'ModelReferenceTargetType');
		isNotModelRefTarget = strcmp(mdlRefTargetType, 'NONE'); % NONE, SIM, or RTW
		
		if isNotModelRefTarget

			% Ensure build output directory exists
			buildDir = fullfile(model_path, 'build');
			if ~exist(buildDir, 'dir')
				mkdir(buildDir);
			end

			% Every build artefact is named after the model plus the version from the
			% Software version block, so each file states which build it belongs to.
			versionSuffix = getVersionSuffix(modelName);
			outputName = [modelName versionSuffix];

			% Cache CodeBase .obj files for reuse in subsequent builds
			if ~isempty(codebase_src_dirs_stm)
				objCacheDir = fullfile(buildDir, 'codebase_objs');
				if ~exist(objCacheDir, 'dir')
					mkdir(objCacheDir);
				else
					% Clear previous cache so stale objects do not accumulate
					delete(fullfile(objCacheDir, '*.obj'));
				end
				copied = 0;
				perBuildSrcs = findPerBuildSources(codebase_src_dirs_stm);
				for d = 1:numel(codebase_src_dirs_stm)
					files = dir(fullfile(codebase_src_dirs_stm{d}, '*.c'));
					for k = 1:numel(files)
						if strcmp(files(k).name, 'main.c'); continue; end
						if any(strcmp(files(k).name, {perBuildSrcs.name})); continue; end
						objName = [files(k).name(1:end-2) '.obj'];
						if isfile(objName)
							copyfile(objName, fullfile(objCacheDir, objName));
							copied = copied + 1;
						end
					end
				end
				if copied > 0
					disp(['### Cached ' num2str(copied) ' CodeBase objects to build/codebase_objs/']);
				else
					warning('### No CodeBase .obj files found to cache');
				end
				codebase_src_dirs_stm = {};
			end

			% Adding the memory addresses to the ASAP2 file
			
			% Get XCP port
			XCPport = get_param(modelName,'tlcXcpTcpPort');
			% Get XCP address
			XCPaddress = get_param(modelName,'tlcXcpTcpAddress');
			% Get the Linux target from the model parameters tab
			LinuxTarget = get_param(modelName,'tlcGOcontrollTarget');
			
			xcp_server = find_system(modelName, 'RegExp', 'on', 'MaskType', 'XCP Server');
	
			if (isscalar(xcp_server)) %check if there is only one, more than one will never reach this point
				medium = get_param(xcp_server{1}, 'server_type');
				can_send_id = eval(get_param(xcp_server{1}, 'send_id'));
				can_receive_id = eval(get_param(xcp_server{1}, 'receive_id'));
			else
				medium = 'TCP'; % no XCP server present, just generate an a2l for TCP
				can_send_id = hex2dec('666');
				can_receive_id = hex2dec('665');
			end

			if isfile(fullfile(pwd, '..', [modelName '.map']))
				MAPfile = fullfile(pwd, '..', [modelName '.map']);
				create_asap2(modelName,XCPport, XCPaddress, stationID, LinuxTarget, MAPfile, medium, can_send_id, can_receive_id);
				movefile(['..' filesep modelName '.map'], fullfile(buildDir, [outputName '.map']));
			else
				create_asap2(modelName,XCPport, XCPaddress, stationID, LinuxTarget, '', medium, can_send_id, can_receive_id);
			end

			%new versions of matlab sometimes put 2 spaces after /begin, this messes up HANtune (for now?)
			%fix the a2l so characteristics get properly loaded
			a2lfile_str = fileread([modelName '.a2l']);
			a2lfile_str = strrep(a2lfile_str, '/begin  ', '/begin ');
			a2lfile = fopen([modelName '.a2l'], 'w');
			fwrite(a2lfile, a2lfile_str);
			fclose(a2lfile);

			movefile([modelName '.a2l'], fullfile(buildDir, [outputName '.a2l']));
			try
				movefile(['..' filesep modelName '.elf'], fullfile(buildDir, [outputName '.elf']));
			catch err
				if (~strcmp(err.identifier, 'MATLAB:MOVEFILE:SourceAndDestinationSame'))
					rethrow(err)
				end
			end

			% Generate .srec and .bin from the .elf (STM target only)
			if strcmp(go_target_platform(LinuxTarget), 'STM')
				objcopyTool = makeToolPath(getInstallationLocation('arm-none-eabi-gcc'), 'arm-none-eabi-objcopy');
				elfFile  = fullfile(buildDir, [outputName '.elf']);
				srecFile = fullfile(buildDir, [outputName '.srec']);
				[ocStatus, ocOut] = system([objcopyTool ' -O srec "' elfFile '" "' srecFile '"']);
				if ocStatus == 0
					disp(['### Created ' outputName '.srec']);
				else
					warning('### srec generation failed: %s', ocOut);
				end
				binFile = fullfile(buildDir, [outputName '.bin']);
				[ocStatus, ocOut] = system([objcopyTool ' -O binary "' elfFile '" "' binFile '"']);
				if ocStatus == 0
					disp(['### Created ' outputName '.bin']);
				else
					warning('### bin generation failed: %s', ocOut);
				end
				artefactExts = {'.elf', '.map', '.a2l', '.srec', '.bin'};
			else
				artefactExts = {'.elf', '.map', '.a2l'};
			end

			% Drop the unversioned artefacts of an earlier build so only the
			% versioned set is left to pick up.
			if ~isempty(versionSuffix)
				for k = 1:numel(artefactExts)
					legacyFile = fullfile(buildDir, [modelName artefactExts{k}]);
					if isfile(legacyFile)
						delete(legacyFile);
					end
				end
			end
			disp(['### Build artefacts in build/ are named ' outputName '.*']);

			% Upload the build to the controller when the option Upload asks for it.
			% An upload that fails leaves the build as it is: the artefacts are in build/.
			if strcmp(get_param(modelName, 'tlcAutoUpload'), 'Auto upload')
				try
					if strcmp(go_target_platform(LinuxTarget), 'STM')
						ManualUpload(modelName, get_param(modelName, 'tlcUploadS1Controller'));
					else
						ManualUpload(modelName, XCPaddress, port=num2str(get_param(modelName, 'tlcUploadPort')));
					end
				catch uploadError
					warning('GOcontroll:uploadFailed', ...
						'### The build is ready in build/, but it was not uploaded: %s', uploadError.message);
				end
			end
		end


	case 'exit'
		% Called at the end of the RTW build process.  All arguments are valid
		% at this stage.
		if isappdata(0, 'GOcontrollBuildRunning'), rmappdata(0, 'GOcontrollBuildRunning'); end
		disp(['### Successful completion of Real-Time Workshop build ',...
			'procedure for model: ', modelName]);
		
		% Check if this is the main model
		mdlRefTargetType = get_param(modelName,'ModelReferenceTargetType');
		isNotModelRefTarget = strcmp(mdlRefTargetType, 'NONE'); % NONE, SIM, or RTW
		
		if isNotModelRefTarget
			% Delete generated build artefacts if requested.
			% Use a timer so Simulink's own onCleanup can cd out of the build
			% folder before we remove it (deleting the cwd causes an error).
			if strcmp(get_param(modelName, 'tlcDeleteBuildFiles'), 'Delete build files after build')
				modelDir    = fileparts(get_param(modelName, 'Filename'));
				slprjPath   = fullfile(modelDir, 'slprj');
				genCodePath = fullfile(modelDir, [modelName '_generated_code']);
				slxcFile    = fullfile(modelDir, [modelName '.slxc']);
				t = timer('ExecutionMode', 'singleShot', 'StartDelay', 3, ...
					'TimerFcn', @(src, ~) goc_deleteBuildFiles(src, slprjPath, genCodePath, slxcFile));
				start(t);
				disp('### Build files will be deleted after build completes');
			end
		end
	end
end
%%******************************* end of ert_linux_make_rtw_hook.m ****************************


%% Search for CANreceive blocks %%
% This function searches for CAN receive blocks and adds a define to the
% SYS_config.h file with the number of blocks found %
function nrOfCANreceiveBlocks = searchCANreceive(modelName)
% build an array with all the blocks that have a Tag starting with HANcoder_TARGET_
blockArray = find_system(modelName, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'MaskType', 'CAN receive');
% only perform check if at least 1 or more HANcoder Target blocks were used
nrOfCANreceiveBlocks = length(blockArray);
end

%% CodeBase sources that depend on the build %%
% Returns the CodeBase .c files that include SYS_config.h. That header is written
% again every build (station ID, XCP port, CAN buffer size), so an object compiled
% from one of these files must never come from the codebase_objs cache: the binary
% would then report the station ID of an older build and no longer match the a2l.
function srcs = findPerBuildSources(srcDirs)
srcs = struct('name', {}, 'folder', {});
for d = 1:numel(srcDirs)
	files = dir(fullfile(srcDirs{d}, '*.c'));
	for k = 1:numel(files)
		if contains(fileread(fullfile(files(k).folder, files(k).name)), 'SYS_config.h')
			srcs(end+1) = struct('name', files(k).name, 'folder', files(k).folder); %#ok<AGROW>
		end
	end
end
end

%% Version suffix for the output binary %%
% Looks up the Software version block and returns its version as
% '_<major>.<minor>.<patch>', so the binary is named <model>_x.x.x.bin.
% Returns an empty string when no usable version is found, in which case the
% binary keeps the plain model name.
function suffix = getVersionSuffix(modelName)
suffix = '';
versionBlocks = find_system(modelName, 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'MaskType', 'Software version');
if isempty(versionBlocks)
	warning('### No Software version block found, naming the binary without a version');
	return;
end
if ~isscalar(versionBlocks)
	warning('### More than one Software version block found, using %s', versionBlocks{1});
end
% The mask initialisation stores the evaluated version numbers in RTWdata.
% Reading them here means the name matches the version compiled into the binary.
rtwData = get_param(versionBlocks{1}, 'RTWdata');
if ~isstruct(rtwData) || ~all(isfield(rtwData, {'major', 'minor', 'patch'}))
	warning('### Software version block holds no version data, naming the binary without a version');
	return;
end
suffix = ['_' rtwData.major '.' rtwData.minor '.' rtwData.patch];
end

%% Build a quoted compiler tool path, or just the tool name if installRoot is empty
%% (assumes the tool is available on the system PATH when installRoot is empty).
function toolpath = makeToolPath(installRoot, toolName)
	if isempty(installRoot)
		toolpath = toolName;
	else
		toolpath = ['"' fullfile(installRoot, 'bin', toolName) '"'];
	end
end

