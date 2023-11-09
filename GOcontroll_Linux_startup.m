% This m-file will be run automatically when starting model 'GOcontroll_Linux.slx'
% The name of the m-file should not be altered

% This m-file will be run when loading the model because it is added to the
% model callbacks: see File->Model Properties->Model Properties->Callbacks

% In this m-file the signals and parameters for HANtune can be defined. As
% an example the signal LedValue and parameter HANtuneOverride are already
% defined.
% You can add your own signals and parameters to this m-file below

%% Signals
% To view a signal in HANtune right click the signal line in Simulink and
% go to properties, at the top enter a name for the signal if it doesn't
% have one yet. Then switch to the Code Generation tab and set the Storage
% class to ExportedGlobal.

% Defining System Information Signals
SI_FreeStack = Simulink.Signal;
SI_FreeStack.StorageClass = 'ExportedGlobal';
SI_CPUload = Simulink.Signal;
SI_CPUload.StorageClass = 'ExportedGlobal';
SI_FreeHeap = Simulink.Signal;
SI_FreeHeap.StorageClass = 'ExportedGlobal';

%% Tunable Parameters
% Defining a parameter for editing in HANtune
LEDRed = Simulink.Parameter; % Define as parameter
LEDRed.StorageClass = 'ExportedGlobal'; % Only Exported Global will be visible in HANtune
LEDRed.Value = 50; % Initial value is set to zero, no override

%% Constant Parameters
UDPBuffSize = 4;

%% UDP data packets
SineWaveTypes = {'uint8','uint8','uint8','uint8'};
SineWaveSizes = {1,1,1,1};
