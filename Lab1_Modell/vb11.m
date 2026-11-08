% Vorbereitungsaufgabe 10 - Dimensionierung des P-Positionsreglers
% RT2 Labor 1 - Motorboard

clear;
close all;
clc;

%% 1. Initialisierung und Parameter

run('params_m.m');

plots_directory = 'plots/vb11';

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
    'Before', '1000');

simInput = simInput.setBlockParameter(Solldrehzahl_Block, ...
    'After', '6000');

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
ylim([0, 6500]);
grid on;

ylabel('Drehzahl n in [U/min]');
xlabel('Zeit in [s]');

title('VB10: Drehzahl Sollwertsprung von 1000 auf 6000 U/min');

exportgraphics(fig, fullfile(plots_directory, 'vb11_n.svg'), ...
    ContentType='vector');

% ------ Strom Plot ------

fig = figure();

plot(t, i_M, 'b', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
ylim([0, 1])
grid on;

ylabel('Motorstrom i_M in [A]');
xlabel('Zeit in [s]');

title('VB10: Strom Sollwertsprung von 1000 auf 6000 U/min');

exportgraphics(fig, fullfile(plots_directory, 'vb11_i.svg'), ...
    ContentType='vector');
