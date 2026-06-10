% Labor Regelungstechnik 2: Lab2

% %% Vorbereitungsaufgabe 7

%% -------------- Reset WorkSpace ---------------

clear;
close all;
clc;

slCharacterEncoding('UTF-8')

%% ------------- Batch Simulation ---------------

plots_directory = 'plots/vb7/';

if ~exist(plots_directory, 'dir')
  mkdir(plots_directory);
end

run 'params_m.m'

mex zP_pos_int.c

mex zPIawu_n_int.c

model = 'Regelkreis_ZD_C_int';

load_system(model);

fetch_block = @(name) [model, '/' name];

%% ------------- Drehzahl Sprungantwort ---------

SimInput = Simulink.SimulationInput(model);

Solldrehzahl_Block = fetch_block('Step');

SimInput = SimInput.setBlockParameter(Solldrehzahl_Block, 'Before', '1000');
SimInput = SimInput.setBlockParameter(Solldrehzahl_Block, 'After', '3000');

SimInput = SimInput.setBlockParameter(fetch_block('Manual Switch'), 'sw', '1');

SimOutput = sim(SimInput);

t = SimOutput.t;
n = SimOutput.n;
phi = SimOutput.phi;

% ------ Drehzahl Plot ------

fig = figure();

plot(t, n, 'r','LineWidth', 1.5);

xlim([0.9, 1.3]);

ylabel('Drehzahl [U/min]');
xlabel('Zeit (sec)');

title('Sprungantwort Drehzahlregelung C ints');

exportgraphics(fig, ...
  [plots_directory, 'Sprungantwort_Drehzahlregelung_n.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

% ------ Position Plot ------

fig = figure();

plot(t, phi, 'b','LineWidth', 1.5);

xlim([0.9, 1.3]);

ylabel('Position [rad]');
xlabel('Zeit (sec)');

title('Sprungantwort Drehzahlregelung C ints');

exportgraphics(fig, ...
  [plots_directory, 'Sprungantwort_Drehzahlregelung_phi.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

%% ------------- Position Sprungantwort ---------

SimInput = Simulink.SimulationInput(model);

Sollposition_Block = fetch_block('Step1');

SimInput = SimInput.setBlockParameter(Sollposition_Block, 'Before', '0');
SimInput = SimInput.setBlockParameter(Sollposition_Block, 'After', '10');

SimInput = SimInput.setBlockParameter(fetch_block('Manual Switch5'), 'sw', '0');

SimOutput = sim(SimInput);

t = SimOutput.t;
n = SimOutput.n;
phi = SimOutput.phi;

% ------ Drehzahl Plot ------

fig = figure();

plot(t, n, 'r','LineWidth', 1.5);

xlim([0.9, 1.3]);

ylabel('Drehzahl [U/min]');
xlabel('Zeit (sec)');

title('Sprungantwort Positionregelung C ints');

exportgraphics(fig, ...
  [plots_directory, 'Sprungantwort_Positionregelung_n.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

% ------ Position Plot ------

fig = figure();

plot(t, phi, 'b','LineWidth', 1.5);

xlim([0.9, 1.3]);

ylabel('Position [rad]');
xlabel('Zeit (sec)');

title('Sprungantwort Positionregelung C ints');

exportgraphics(fig, ...
  [plots_directory, 'Sprungantwort_Positionregelung_phi.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

%% ----------------------------------------------
