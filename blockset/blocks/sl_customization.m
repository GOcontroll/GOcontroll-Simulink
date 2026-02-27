function sl_customization(cm)
% SL_CUSTOMIZATION  Register GOcontroll model templates in the New Model dialog.
%   Automatically loaded by Simulink when this folder is on the MATLAB path.
%   On first use the template .sltx is generated from a blank model with
%   ert_gocontroll.tlc already configured as the System Target File.

    mfilePath = mfilename('fullpath');
    [blockspath, ~, ~] = fileparts(mfilePath);
    templateFile = fullfile(blockspath, '..', 'utility_functions', 'GOcontroll_Target.sltx');

    if ~isfile(templateFile)
        try
            goc_create_template_file(templateFile);
        catch err
            % ert_gocontroll.tlc not on path yet (project still loading) — skip.
            % The template will be created on the next Simulink refresh.
            warning('GOcontroll: skipping template generation: %s', err.message);
            return;
        end
    end

    mt             = Simulink.ModelTemplateDesc;
    mt.Name        = 'GOcontroll Target';
    mt.Description = 'Blank model pre-configured for GOcontroll targets (ert_gocontroll.tlc)';
    mt.ModelFile   = templateFile;
    cm.addModelTemplateDesc(mt);
end

% -----------------------------------------------------------------------

function goc_create_template_file(templateFile)
% Creates a blank Simulink model, sets the STF to ert_gocontroll.tlc and
% exports it as a Simulink template (.sltx).

    stf = which('ert_gocontroll.tlc');
    if isempty(stf)
        error('ert_gocontroll.tlc not found on MATLAB path');
    end

    tempName = 'GOcontroll_Template_Internal';
    if bdIsLoaded(tempName)
        close_system(tempName, 0);
    end

    new_system(tempName);
    cs = getActiveConfigSet(tempName);
    cs.switchTarget(stf, []);

    Simulink.exportToTemplate(tempName, templateFile);
    close_system(tempName, 0);

    fprintf('GOcontroll: template created at %s\n', templateFile);
end
