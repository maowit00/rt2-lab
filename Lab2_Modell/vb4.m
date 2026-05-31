% Labor Regelungstechnik 2: Lab2

% %% Vorbereitungsaufgabe 4

%% -------------- Reset WorkSpace ---------------

clear;
close all;
clc;

slCharacterEncoding('UTF-8')

%% ------------- Batch Simulation ---------------

plots_directory = 'plots/vb4/';

if ~exist(plots_directory, 'dir')
  mkdir(plots_directory);
end

run 'params_m.m'

model = 'Regelkreis_ZK';

load_system(model);

fetch_block = @(name) [model, '/' name];

%% ------------- Position Sprungantwort ---------

SimInput = Simulink.SimulationInput(model);

Sollposition_Block = fetch_block('Step1');

SimInput = SimInput.setBlockParameter(Sollposition_Block, 'Before', '0');
SimInput = SimInput.setBlockParameter(Sollposition_Block, 'After', '0.0025');

SimInput = SimInput.setBlockParameter(Sollposition_Block, 'Time', '1');

SimInput = SimInput.setBlockParameter(fetch_block('Manual Switch5'), 'sw', '0');

SimOutput = sim(SimInput);

t = SimOutput.t;
n = SimOutput.n;
phi = SimOutput.phi;

% ------ Position Plot ------

fig = figure();

plot(t, phi, 'b','LineWidth', 1.5);

ylabel('Position als gemessene spannung [U]');
xlabel('Zeit (sec)');

xlim([0, 5]);

title('Sprungantwort Positionregelung mit Reibung');

exportgraphics(fig, ...
  [plots_directory, 'Reibung_Sprungantwort_Positionregelung_phi.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

% ------ Drehzahl Plot ------

fig = figure();

plot(t, n, 'r','LineWidth', 1.5);

xlim([0, 5]);

ylabel('Drehzahl [U/min]');
xlabel('Zeit (sec)');

title('Sprungantwort Positionregelung mit Reibung');

exportgraphics(fig, ...
  [plots_directory, 'Reibung_Sprungantwort_Positionregelung_n.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

%% ---------------- Ohne Haftreibung ------------

friction_offset = 0;

SimOutput = sim(SimInput);

t = SimOutput.t;
n = SimOutput.n;
phi = SimOutput.phi;

% ------ Position Plot ------

fig = figure();

plot(t, phi, 'b','LineWidth', 1.5);

ylabel('Position als gemessene spannung [U]');
xlabel('Zeit (sec)');

xlim([0, 5]);

title('Sprungantwort Positionregelung ohne Reibung');

exportgraphics(fig, ...
  [plots_directory, 'OhneReibung_Sprungantwort_Positionregelung_phi.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

% ------ Drehzahl Plot ------

fig = figure();

plot(t, n, 'r','LineWidth', 1.5);

xlim([0.9, 1.3]);

ylabel('Drehzahl [U/min]');
xlabel('Zeit (sec)');

title('Sprungantwort Positionregelung ohne Reibung');

exportgraphics(fig, ...
  [plots_directory, 'OhneReibung_Sprungantwort_Positionregelung_n.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

