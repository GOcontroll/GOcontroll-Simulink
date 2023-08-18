# GOcontroll-Simulink
A base project containing the GOcontroll blockset to start developing a Simulink model

## Important notice for users upgrading to this blockset with an older controller

For licensing reasons one library required for building the model must be dynamically linked in, this means that ./blockset/lib/IIO/libiio.so.0 must be uploaded to the target controller to /usr/lib/ \
If your controller is running release 0.5.0 or higher you are not affected and can ignore this. \
To check what release your controller is running see /etc/controller_update/current-release.txt or run the identify command and look for the repo release field (all on the controller).

## GOcontroll Moduline IV toolchain setup

To compile the Linux bases blockset for GOcontroll Moduline IV, some initial steps needs to be made:
- The template project is created in Matlab Simulink R2018b. This is the tested version so it is strongly recommended to use this version of Matlab.
- To generate code, you have to install some additional toolboxes to Matlab. This could require some extra licensing. Check https://openmbd.com/getting-started/installation/ for the toolboxes that need to be installed. Ignore the version information on this website.
- Finally you need to add the right compiler to you system. For Windows this is: gcc-arm-9.2-2019.12-mingw-w64-i686-aarch64-none-linux-gnu. \
    For Linux Host systems it is:  aarch64-none-linux-gnu.
- The website where this compiler can be found is: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads. Once downloaded, extract it and add a system environment variable to point to the bin directory of the compiler. For Windows for example: C:\Program Files (x86)\gcc-arm-9.2-2019.12-mingw-w64-i686-aarch64-none-linux-gnu\bin.
On Linux ~/.bashrc must be appended with the line: export PATH=/your/compiler/bin:$PATH.
Reboot may be necessary to ‘activate’ this environment variable. On Linux one can run: source ~/.bashrc to update the path, or reboot.
(tested on ubuntu 20.04)

## Setting up a new project

To maintain an up-to-date blockset in all your projects and ensure proper version control capabilities here are some tips:
- Clone this repository in some location.
- Make a new directory in which your project will be housed and set up your version control here.
- Copy the everything in from this repository to your project folder except the blockset folder and your .git folder (which is probably invisible for you).
- make a symbolic link from the Cloned repository blockset folder to your project folder. In Linux this can be done with the ln -s command, in Windows the mklink command can be used in cmd.

Just to mention, be sure to open the .slx (template project) from within your Matlab environment. You need to see the .slx file in the ‘current folder’ tree otherwise the path references are incorrect. Once the Simulink template project is opened, you see the led wave program. When the compiler is available, you should be able to build this project.

## Adding your own blockset

To add your own blocks to the project the following pattern should be followed:
- The name of the folder should start with blockset_
- In the root of this folder should be a Makefile, a librarySetup.m and a setupBlocks.m script

The rest of what it looks like is up to you and should be accurately described in these 2 files

### the Makefile

Add any source code files to the ADD_SRCS variable
`ADD_SRSCS += source.c`

Add included folders to the CC_INCLUDES variable
`CC_INCLUDES += -I"$(BIN_PATH)/blockset_yours/included_folder"`

It might be handy to make your own path variable
`YOUR_SOURCES_PATH = $(BIN_PATH)/blockset_yours/included_folder`

Add a rule for compiling your sources, or multiple if your sources are spread out along multiple paths

```
%.obj : $(YOUR_SOURCES_PATH)/%.c
	echo Compiling "your sources" path $(notdir $<) -o $(notdir $@)
	@$(CC) -c -o $@ $< $(CC_FLAGS) $(CC_INCLUDES)
```

To make your build system Windows and Linux compatible use the HOST variable

```
ifeq ($(HOST), GLNXA64) Linux
    Actions for Linux systems
else
    Actions for Windows systems
endif
```

add libraries to the LIBS variable:
```
LIBS += $(OAES_PATH)/liboaes_lib.a
```

### the librarySetup.m script

This script is meant to setup the paths for matlab so it can see your blockset
example:
```
OS = computer();
if OS=="GLNXA64" % for Linux hosts
    addpath(sprintf('%s', pwd));        %add the blockset_ rootfolder
    addpath(sprintf('%s/blocks',pwd)); %add the matlab files for the blockset
    addpath(sprintf('%s/code',pwd));   %add the source code folder
else % for Windows hosts
    addpath(sprintf('%s', pwd));
    addpath(sprintf('%s\\blocks',pwd));
    addpath(sprintf('%s\\code',pwd));
end
```

### the setupBlocks.m script

This script is meant to setup your blocksets in the Simulink library
example:
```
Browser(BrowserIndex).Library = 'NameOfTheLibraryFile';  	% you could have blockset_test.slx it would be blockset_test
Browser(BrowserIndex).Name    = 'DisplayNameOfTheLibrary';	% any name you wish
Browser(BrowserIndex).IsFlat  = 0;
BrowserIndex = BrowserIndex + 1; 				% increment the BrowserIndex with the amount of libraries that you have added so any other blocksets can be properly initialized aswell
```

### the makeHook.m script

This script lets you do something after the .tlc files have been processed, but before make. For example a header file with some model dependent parameters can be constructed to compile your blockset. For example you want to set some buffer size in your code in simulink.
This script will get run in the "generated_code" folder of your project.
example:
```
fprintf('\n### Adding data to UDP_config.h...\n');
%% get the modelname from the folder name
OS = computer();
if OS=="GLNXA64" % if the host is Linux
    splitPath = split(pwd, "/");
else % if the host is Windows
    splitPath = split(pwd, "\\");
end
dirName = splitPath(length(splitPath));
dirNameSplit = split(dirName, "_generated_code");
modelName = char(dirNameSplit{1});

%% get the necessary information from the model
nrOfUDPReceiveBlocks = searchUDPreceive(modelName);
UDPBuffSize = getUDPBuffSize(modelName);
%% Open file in write mode 'w'
file = fopen('UDP_config.h', 'w');
if file == -1
     error('### failed to open UDP_config.h');
end
fprintf(file, '#ifndef __UDP_CONFIG_H__ \n#define __UDP_CONFIG_H__\n');
fprintf(file, '#define UDPBUFFNUM                     %d\n', nrOfUDPReceiveBlocks);
fprintf(file, '#define UDPBUFFSIZE                     %s\n', UDPBuffSize);
fprintf(file, '#endif');
fclose(file);
    
%% Search for UPDReceive blocks %%
%% This function searches for UDP receive blocks and adds a define to the
%% UDP_config.h file with the number of blocks found %

function nrOfUDPReceiveBlocks = searchUDPreceive(modelName)
    % build an array with all the blocks that have a Tag starting with HANcoder_TARGET_
    blockArray = find_system(modelName, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'MaskType', 'UDP receive');
    % only perform check if at least 1 or more HANcoder Target blocks were used
    nrOfUDPReceiveBlocks = length(blockArray);
end

function UDPBuffSize = getUDPBuffSize(modelName)
    config = find_system(modelName, 'RegExp', 'on', 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'MaskType', 'Configure UDP socket');
    if length(config)
        UDPBuffSize = get_param(config{1}, "buffer_length");
    else
        UDPBuffSize = "0";
    end
end
```

Please let us know when interface blocks are not working properly. You can contact us at support@gocontroll.com \
Or make a pull request with changes to fix issues that you are experiencing.
