function image = gocontroll_png(name)
	% Returns an image from blockset/assets for use in a mask icon.
	%   gocontroll_png()        the GOcontroll logo with a controller (GOcontroll.png)
	%   gocontroll_png(name)    assets/<name>.png, for example 'GOcontroll_Moduline_S1'
	if nargin < 1
		name = 'GOcontroll';
	end
	%get the path of this script so we can find the image.
	mfilePath = mfilename('fullpath');
	if contains(mfilePath,'LiveEditorEvaluationHelper')
		mfilePath = matlab.desktop.editor.getActiveFilename;
	end
	%remove the filename from the end
	[path, ~, ~] = fileparts(mfilePath);

	image = imread(fullfile(path, '..', 'assets', [name '.png']));
end
