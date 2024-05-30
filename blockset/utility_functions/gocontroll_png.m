function image = gocontroll_png()
	%get the path of this script so we can find the image.
	mfilePath = mfilename('fullpath');
	if contains(mfilePath,'LiveEditorEvaluationHelper')
		mfilePath = matlab.desktop.editor.getActiveFilename;
	end
	%remove the filename from the end
	[path, ~, ~] = fileparts(mfilePath);

	image = imread(fullfile(path, '..', '..', 'GOcontroll.png'));
end
