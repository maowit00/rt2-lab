% Vorbereitungsaufgabe 13 - Dimensionierung des P-Positionsreglers
% RT2 Labor 1 - Motorboard

clear;
close all;
clc;

%% 1. Initialisierung und Parameter

run('params_m.m');

plots_directory = 'plots/vb13';

if ~exist(plots_directory, 'dir')
    mkdir(plots_directory);
end

%% ------------- Batch Simulation ---------------

% Simulation Parms
Simulationsdauer = 2.5;

model = 'Regelkreis_ZK';

load_system(model);

fetch_block = @(name) [model, '/' name];

% Simulation Input

simInput = Simulink.SimulationInput(model);

simInput = simInput.setModelParameter('StopTime', ...
    num2str(Simulationsdauer));

SollPosition_Block = fetch_block('Step1');

simInput = simInput.setBlockParameter(SollPosition_Block, ...
    'Before', '0');

simInput = simInput.setBlockParameter(SollPosition_Block, ...
    'After', '10');

Solldrehzal_Schalter = fetch_block('Manual Switch');

simInput = simInput.setBlockParameter(Solldrehzal_Schalter, ...
    'sw', '0');

SollPosition_Schalter = fetch_block('Manual Switch5');

simInput = simInput.setBlockParameter(SollPosition_Schalter, ...
    'sw', '0');


%% Simulate

simOutput = sim(simInput);

t = simOutput.t;
n = simOutput.n;
i_M = simOutput.i;
phi = simOutput.phi;

% ------ Position Plot ------


fig = figure();

plot(t, phi, 'g', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
grid on;

ylabel('Position n in (U = Umdrehungen)');
xlabel('Zeit in [s]');

title('VB13: Position Sollwertsprung von 0 auf 10U');

exportgraphics(fig, fullfile(plots_directory, 'vb13_phi.svg'), ...
    ContentType='vector');

% ------ Drehzahl Plot ------


fig = figure();

plot(t, n, 'r', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
grid on;

ylabel('Drehzahl n in [U/min]');
xlabel('Zeit in [s]');

title('VB13: Drehzahl Sollwertsprung von 0 auf 10U');

exportgraphics(fig, fullfile(plots_directory, 'vb13_n.svg'), ...
    ContentType='vector');

% ------ Strom Plot ------

fig = figure();

plot(t, i_M, 'b', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
grid on;

ylabel('Motorstrom i_M in [A]');
xlabel('Zeit in [s]');

title('VB13: Strom Sollwertsprung von 0 auf 10U');

exportgraphics(fig, fullfile(plots_directory, 'vb13_i.svg'), ...
    ContentType='vector');
