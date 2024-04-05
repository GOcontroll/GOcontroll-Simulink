% This m-file will be run automatically when starting model 'GOcontroll_Linux.slx'
% The name of the m-file should not be altered

% This m-file will be run when loading the model because it is added to the
% model callbacks: see File->Model Properties->Model Properties->Callbacks

% In this m-file the signals and parameters for HANtune can be defined. As
% an example the signal LedValue and parameter HANtuneOverride are already
% defined.
% You can add your own signals and parameters to this m-file below

%% Signals
% To view a signal in HANtune, name a signal line then click it,
% hover till the little selection menu withthe three dots appears,
% from this menu select "add selected signals to code mappings"
% (note that you can do this for multiple lines in one go by selecting multiple lines).
%
% After this, navigate to the C CODE tab at the top, then from the Code
% Interface options select the "Individual Element Code Mappings" option.
% This will open up an interface at the bottom of the screen, in this interface
% navigate to the "Signals/States" tab, your signal should be visible in this list.
% Here you can set the storage class to "ExportedGlobal", this can also be 
% done for multiple signals in one go by selecting multiple signals in the list
% with ctrl or shift click, and then changing the storage class for any of the
% selected signals.

%% Tunable Parameters
% Defining a parameter for editing in HANtune
LEDRed = Simulink.Parameter; % Define as parameter
LEDRed.StorageClass = 'ExportedGlobal'; % Only Exported Global will be visible in HANtune
LEDRed.Value = uint8(50); % Initial value is set to zero with the data type uint8

%% Constant Parameters
UDPBuffSize = 5;

%% UDP data packets
SineWaveTypes = {'uint8','uint8','uint8','uint8'};
SineWaveSizes = {1,1,1,1};

set_param('GOcontroll_Linux/GOcontroll Moduline algorithm/UDP_Host', 'Commented', 'on');
set_param('GOcontroll_Linux/GOcontroll Moduline algorithm/UDP_Client', 'Commented', 'on');
