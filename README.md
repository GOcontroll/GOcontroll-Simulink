# GOcontroll-Simulink
A Matlab/Simulink toolbox for working with GOcontroll Moduline controllers, it contains a blockset to access the hardware and a toolchain to compile it.  
This is the branch made with Matlab 2023b+, it will not work for 2018b, see the 2018b branch for that version.

## Upgrading from the 2018b blockset/installing

Notice: Change the default addon location so it contains no spaces. Make really doesn't like spaces in file paths.  
This can be done in the **HOME** tab of **Matlab** -> **preferences** (environment section) -> **MATLAB** -> **Add-Ons**.  
Change the installation folder to something like "MATLAB*your matlab version*_addons", do this for all your Matlab installations.  
This way it is also easier to maintain version compatibility between toolboxes and Matlab. For more info see [the help page](https://nl.mathworks.com/help/matlab/matlab_env/get-add-ons.html).  
For Windows users it is especially recommended to change this folder to a shorter path. Windows by default has a 256 character file path limit and there have been issues because of this, it should be fine as long as your username part of the path is shorter than 20 characters.  

To update your project to the 2023b+ blockset you can do these steps:
1. Copy your project so we have a test version for the new blockset without changing your current version.
2. Install the GOcontroll-Simulink blockset through either the mltbx downloaded from the github.com releases page or through the Matlab add-on explorer.
3. In your model, right click on an open space and open the 'Model Properties' menu, select the Callbacks tab, in either PreLoadFcn or PostLoadFcn remove the call to 'librarySetup' as this script will now disturb the newly installed toolbox.
4. Restart Matlab for good measure, now there are some potential breaking changes to check.
5. If you are using any output module monitor blocks, check these as they've had a change in this version, this causes the signals to no longer be attached to the correct outputs of the block.
6. Check if the CAN blocks are linked correctly to the library, if there is some issue here check [the changelog](blockset/ert_linux_target_version.m) of V3.4.0.

This blockset contains some new blocks, biggest of which are the UDP blocks and the new CAN blocks. These new blocks have examples included in the library that you can refer to. It is possible to use the new and old CAN blocks together, if one bus only has old or only has new blocks it will be good, mixing old and new blocks on one bus seems to work aswell but it is not recommended.

## Important notice for users upgrading to this blockset with an older controller

For licensing reasons one library required for building the model must be dynamically linked in, this means that ./blockset/lib/IIO/libiio.so.0 must be uploaded to the target controller to /usr/lib/  
If your controller is running release 0.5.0 or higher you are not affected and can ignore this.  
To check what release your controller is running see /etc/controller_update/current-release.txt or run the identify command and look for the repo release field (all on the controller).

## GOcontroll Moduline IV toolchain setup

To compile the Linux based blockset for GOcontroll Moduline IV/Mini/Display, some initial steps needs to be made:
- The template project is created in Matlab Simulink R2023b. 2024a is also confirmed to work and brings some nice improvements
- To compile some necessary mex files on Windows you may need to install the "MATLAB Support for MinGW-w64 C/C++/Fortran Compiler" add-on from the Matlab add-on explorer.
- To compile some necessary mex files on Mac you need to install and activate some Xcode programs. To install the proper components run `xcode-select --install` and `defaults write com.apple.dt.Xcode IDEXcodeVersionForAgreedToGMLicense 12.0` where 12.0 has to be replaced with your xcode version.

## Setting up a new project

### Rolling release
1. Install the addon from the Matlab addon explorer
2. In the 'HOME' tab of Matlab, from the 'New' dropdown menu, select 'Simulink Model'
3. Make a new Simulink model with the 'GOcontroll Linux' template
4. Save the model
5. Build

### Stable blockset

If you do not want the blockset to update, there are a couple of steps to be taken:

1. The add-on must be installed through the addon store to install the compilers and such
2. In the 'HOME' tab of Matlab, from the 'New' dropdown menu, select 'Project -> From Simulink Template'
3. Create the project where you want it
4. In the 'HOME' tab of Matlab, from the 'New' dropdown menu, select 'Simulink Model'
5. Make a new Simulink model with the 'GOcontroll Linux' template
6. Save the model
7. Build

## Adding a project specific library

To add your own reusable blocks it is possible to create your own library that will be loaded upon startup.  
It is not a good idea to modify the existing GOcontroll libraries, as this will cause major issues when you want to update.
1. Create a folder in your project that will contain the library.
2. In Matlab on the 'HOME' tab, select 'Simulink Model' from the 'New' dropdown menu. Select the 'Blank Library' option, place your block(s) in here and save it in this new folder
3. In this folder create a file called 'slblocks.m'
4. In this file should be the following with the 2 name fields replaced with relevant values for you:
```matlab
function blkStruct = slblocks()
	Browser.Library = 'YourLibraryFileName'; %no file extension, so 'Library.mdl' would be 'Library'
	Browser.Name = 'VisualLibraryName'; %this name will show up in the Library Browser
	blkStruct.Browser = Browser;
end
```
5. Add this folder to the path of your project so it gets properly loaded. This is done in the 'PROJECT' tab of Matlab, from this tab select 'Project Path'. Then use either 'Add Folder' or 'Add with Subfolders' to add your new folder to the path

## Developing for the blockset

To develop for this project, clone it, then open the project.  
This will disable the installed version and allow you to work with the one that is tracked in git.

Never commit +GOcontroll_Simulink_2023b_dev/getInstallationLocation.mlx, in theory this should work out fine, but as it is a binary file it will cause annoying merge conflicts.  
To force this run `git update-index --skip-worktree +GOcontroll_Simulink_2023b_dev/getInstallationLocation.mlx` in the repository.

## Support

Please let us know when interface blocks are not working properly. You can contact us at support@gocontroll.com  
Or make a pull request with changes to fix issues that you are experiencing.

The changelog can be found in blockset/ert_linux_target_version.m