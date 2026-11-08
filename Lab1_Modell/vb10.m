% Vorbereitungsaufgabe 10 - Dimensionierung des P-Positionsreglers
% RT2 Labor 1 - Motorboard

clear;
close all;
clc;

%% 1. Initialisierung und Parameter

run('params_m.m');

plots_directory = 'plots/vb10';

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

Solldrehzahl_Block = fetch_block('Step');

simInput = simInput.setBlockParameter(Solldrehzahl_Block, ...
    'Before', '3000');

simInput = simInput.setBlockParameter(Solldrehzahl_Block, ...
    'After', '3100');

Solldrehzal_Schalter = fetch_block('Manual Switch');

simInput = simInput.setBlockParameter(Solldrehzal_Schalter, ...
    'sw', '1');


%% Simulate

simOutput = sim(simInput);

t = simOutput.t;
n = simOutput.n;
i_M = simOutput.i;
phi = simOutput.phi;

% ------ Drehzahl Plot ------


fig = figure();

plot(t, n, 'r', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
ylim([2900, 3200]);
grid on;

ylabel('Drehzahl n in [U/min]');
xlabel('Zeit in [s]');

title('VB10: Drehzahl Sollwertsprung von 3000 auf 3100 U/min');

exportgraphics(fig, fullfile(plots_directory, 'vb10_n.svg'), ...
    ContentType='vector');

% ------ Strom Plot ------

fig = figure();

plot(t, i_M, 'b', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
ylim([0, 0.3])
grid on;

ylabel('Motorstrom i_M in [A]');
xlabel('Zeit in [s]');

title('VB10: Strom Sollwertsprung von 3000 auf 3100 U/min');

exportgraphics(fig, fullfile(plots_directory, 'vb10_i.svg'), ...
    ContentType='vector');
