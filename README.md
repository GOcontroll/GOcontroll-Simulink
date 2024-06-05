# GOcontroll-Simulink
A Matlab/Simulink toolbox for working with GOcontroll Moduline controllers, it contains a blockset to access the hardware and a toolchain to compile it.  
This is the branch made with Matlab 2023b, it will not work for 2018b, see the 2018b branch for that version.

## Upgrading from the 2018b blockset/installing

Notice: Change the default addon location so it contains no spaces. Make really doesn't like spaces in file paths.  
This can be done in the **HOME** tab of **Matlab** -> **preferences** (environment section) -> **MATLAB** -> **Add-Ons**.  
Change the installation folder to something like "MATLAB*your matlab version*_addons", do this for all your Matlab installations.  
This way it is also easier to maintain version compatibility between toolboxes and Matlab. For more info see [the help page](https://nl.mathworks.com/help/matlab/matlab_env/get-add-ons.html).  
For Windows users it is especially recommended to change this folder to a shorter path. Windows by default has a 256 character file path limit and there have been issues because of this, it should be fine as long as your username part of the path is shorter than 20 characters.  

To update your project to the 2023b blockset you can do these steps:
1. Copy your project so we have a test version for the new blockset without changing your current version.
2. Install the GOcontroll-Simulink blockset through either the mltbx downloaded from the github releases page or through the Matlab add-on explorer.
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
- The template project is created in Matlab Simulink R2023b. This is the tested version so it is strongly recommended to use this version of Matlab.
- To compile some necessary mex files on Windows you may need to install the "MATLAB Support for MinGW-w64 C/C++/Fortran Compiler" add-on from the Matlab add-on explorer.

## Setting up a new project

In Matlab on the 'HOME' tab, select 'Simulink Model' from the 'New' dropdown menu. There will be a template called 'GOcontroll Linux', this template contains a basic model and the required settings for the toolchain, from this template you can start building your model.

Please let us know when interface blocks are not working properly. You can contact us at support@gocontroll.com  
Or make a pull request with changes to fix issues that you are experiencing.

The changelog can be found in blockset/ert_linux_target_version.m