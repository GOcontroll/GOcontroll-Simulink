function platform = go_target_platform(target)
%%***************************************************************************************
%% file         go_target_platform.m
%% brief        Single source of truth that maps a GOcontroll target name onto the
%%              code generation platform it belongs to.
%%
%%              Returns 'STM'   for the Moduline S1  (STM32H5, bare metal + FreeRTOS)
%%              Returns 'Linux' for the Moduline M1 / L4 / HMI1 (embedded Linux)
%%
%%              Legacy names ('GOcontroll Moduline IV' etc.) are still accepted so a
%%              model that was saved with an older blockset keeps building. New models
%%              always store the short product names.
%%
%% Copyright 2026 (c) GOcontroll   http://www.gocontroll.com   All rights reserved
%%***************************************************************************************

    switch target
        case {'Moduline S1', ...
              'GOcontroll Moduline IoT'}                % legacy
            platform = 'STM';

        case {'Moduline M1', 'Moduline L4', 'Moduline HMI1', ...
              'GOcontroll Moduline Mini', ...           % legacy
              'GOcontroll Moduline IV', ...             % legacy
              'GOcontroll Moduline Display'}            % legacy
            platform = 'Linux';

        otherwise
            error('go_target_platform:unknownTarget', ...
                  ['Unknown GOcontroll target ''%s''. Open Configuration Parameters ' ...
                   '> Code Generation > GOcontroll options and select a valid Target.'], ...
                  target);
    end
end
