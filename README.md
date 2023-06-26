# GOcontroll-Simulink
A base project containing the GOcontroll blockset to start developing a Simulink model

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
    addpath(sprintf('%s//blocks',pwd)); %add the matlab files for the blockset
    addpath(sprintf('%s//code',pwd));   %add the source code folder
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

Please let us know when interface blocks are not working properly. You can contact us by support@gocontroll.com
Or make a pull request with changes to fix issues that you are experiencing.
