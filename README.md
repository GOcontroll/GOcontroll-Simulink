# GOcontroll-Simulink
A base project containing the GOcontroll blockset to start developing a Simulink model
This is the branch made with Matlab 2023b, it will not work for 2018b, see the 2018b branch for that version.

## Upgrading from the 2018b blockset

To update your project to the 2023b blockset you can do these steps:
1. Copy your project so we have a test version for the new blockset without changing your current version.
2. Remove the `librarySetup.m` script and `blockset` folder from this copy.
3. Copy the `librarySetup.m` script and `blockset` folder from this repository to this place.
4. Open the Simulink file in Matlab R2023b (if you do not have a callback set up for the librarySetup.m script run it manually)
5. If you are using any output module monitor blocks, check these as they've had a change in this version, this causes the signals to no longer be attached to the correct outputs of the block.
6. Check if the CAN blocks are linked correctly to the library, if there is some issue here check [the changelog](blockset/ert_linux_target_version.m) of V3.4.0.

This blockset contains some new blocks, biggest of which are the UDP blocks and the new CAN blocks. These new blocks have examples included in the library that you can refer to. It is possible to use the new and old CAN blocks together, if one bus only has old or only has new blocks it will be good, mixing old and new blocks on one bus seems to work aswell but it is not recommended.

## Important notice for users upgrading to this blockset with an older controller

For licensing reasons one library required for building the model must be dynamically linked in, this means that ./blockset/lib/IIO/libiio.so.0 must be uploaded to the target controller to /usr/lib/  
If your controller is running release 0.5.0 or higher you are not affected and can ignore this.  
To check what release your controller is running see /etc/controller_update/current-release.txt or run the identify command and look for the repo release field (all on the controller).

## GOcontroll Moduline IV toolchain setup

To compile the Linux based blockset for GOcontroll Moduline IV/Mini/Display, some initial steps needs to be made:
- The template project is created in Matlab Simulink R2023b. This is the tested version so it is strongly recommended to use this version of Matlab.
- To generate code, you have to install some additional toolboxes to Matlab. This could require some extra licensing. Check https://openmbd.com/getting-started/installation/ for the toolboxes that need to be installed. Ignore the version information on this website.
- Finally you need to add the right compiler to you system. For Windows this is: gcc-arm-9.2-2019.12-mingw-w64-i686-aarch64-none-linux-gnu.  
    For Linux Host systems it is:  aarch64-none-linux-gnu.
- The website where this compiler can be found is: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads. Once downloaded, extract it and add a system environment variable to point to the bin directory of the compiler. For Windows for example: C:\Program Files (x86)\gcc-arm-9.2-2019.12-mingw-w64-i686-aarch64-none-linux-gnu\bin.  
On Linux ~/.bashrc must be appended with the line: export PATH=/your/compiler/bin:$PATH.  
A reboot may be necessary to ‘activate’ this environment variable. On Linux one can run: source ~/.bashrc to update the path, or reboot.
(tested on ubuntu 20.04)
- To compile some necessary mex files on Windows you need to install the "MATLAB Support for MinGW-w64 C/C++/Fortran Compiler" add-on from the Matlab add-on explorer.

## Setting up a new project

To maintain an up-to-date blockset in all your projects and ensure proper version control capabilities here are some tips:
1. Clone this repository in some location (`git clone https://github.com/GOcontroll/GOcontroll-Simulink.git` or using github desktop with the same link).
2. Make a new directory in which your project will be housed and set up your version control here.
3. Copy the everything in from this repository to your project folder except the blockset folder and your .git folder (which is probably invisible for you).
4. make a symbolic link from the Cloned repository blockset folder to your project folder. In Linux this can be done with the ln -s command, in Windows the mklink command can be used in cmd.  
`ln -s your/cloned/location/blockset your/project/path/`  
or on windows open a cmd (not powershell) window as administrator, navigate to your project folder and run  
`mklink /d blockset \your\cloned\location\blockset\`

If you would not like to link the blockset to your project because you would like more control over the version that is present in your project, repeat step 3 but also copy the blockset folder and don't do step 4.

Just to mention, be sure to open the .slx/.mdl (template project) from within your Matlab environment. You need to see the .slx file in the ‘current folder’ tree otherwise the path references are incorrect. Once the Simulink template project is opened, you see the led wave program. When the compiler is available, you should be able to build this project.

## Adding your own blockset

To add your own blocks to the project the following pattern should be followed:
- The name of the folder should start with blockset_
- In the root of this folder should be a Makefile, a librarySetup.m and a setupBlocks.m script

The rest of what it looks like is up to you and should be accurately described in these 3 files

### the Makefile

Add any source code files to the ADD_SRCS variable  
`ADD_SRSCS += source.c`

Add included folders to the CC_INCLUDES variable  
`CC_INCLUDES += -I"$(BIN_PATH)/blockset_yours/included_folder"`

It might be handy to make your own path variable  
`YOUR_SOURCES_PATH = $(BIN_PATH)/blockset_yours/included_folder`

Add a rule for compiling your sources, or multiple if your sources are spread out along multiple paths
```make
%.obj : $(YOUR_SOURCES_PATH)/%.c
	echo Compiling "your sources" path $(notdir $<) -o $(notdir $@)
	@$(CC) -c -o $@ $< $(CC_FLAGS) $(CC_INCLUDES)
```

add libraries to the LIBS variable:
```make
LIBS += $(YOUR_LIBS_PATH)/your_lib.a
```

### the librarySetup.m script

This script is meant to setup the paths for matlab so it can see your blockset  
example:
```matlab
addpath([pwd]);
addpath([pwd filesep 'blocks']);
addpath([pwd filesep 'code']);
```

### the setupBlocks.m script

This script is meant to setup your blocksets in the Simulink library  
example:
```matlab
Browser(BrowserIndex).Library = 'NameOfTheLibraryFile';  	% if you made blockset_test.slx/.mdl it would be blockset_test
Browser(BrowserIndex).Name    = 'DisplayNameOfTheLibrary';	% any name you wish
Browser(BrowserIndex).IsFlat  = 0;
BrowserIndex = BrowserIndex + 1; 				% increment the BrowserIndex with the amount of libraries that you have added so any other blocksets can be properly initialized aswell
```

Please let us know when interface blocks are not working properly. You can contact us at support@gocontroll.com  
Or make a pull request with changes to fix issues that you are experiencing.

The changelog can be found in blockset/ert_linux_target_version.m