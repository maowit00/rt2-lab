% Vorbereitungsaufgabe 7c - Simulation der Strecke (Motorboard)
% RT2 Labor 1
% Fall c: u_Motor,in: 0V -> 4V, Generator an Stromquelle, I_DC: 0 -> +200 mA

clear;
close all;
clc;

%% Initialisierung
run('strecke_params_m.m');

plots_directory = 'plots/vb7';

if ~exist(plots_directory, 'dir')
    mkdir(plots_directory);
end

model = 'Strecke';

load_system(model);

block_path = @(name) [model, '/' name];

% Eingangsspannung
u_in_Block = block_path('u_in');

% Generatorstrom
I_DC_Block = block_path('I_DC');

% Generatorschalter (Eingang 1 = Stromquelle, Eingang 2 = Lampe)
Generator_Schalter = block_path('Last_Schalter');

% Belastungsschalter (Eingang 1 = Last wirkt, Eingang 2 = keine Last)
Belastung_Schalter = block_path('Belasted');

% Simulation Parms
Simulationsdauer = 5;
SprungEingangsspannung_t = 1;
SprungStromquelle_t = 3;

%% Simulation

simInput = Simulink.SimulationInput(model);

simInput = simInput.setModelParameter('StopTime', ...
    num2str(Simulationsdauer));

% Sprung der Eingangsspannung: 0 V -> 4 V
simInput = simInput.setBlockParameter(u_in_Block, ...
    'Time', num2str(SprungEingangsspannung_t));
simInput = simInput.setBlockParameter(u_in_Block, 'After', '4');

% Generator: Stromquelle, Sprung auf +200 mA
simInput = simInput.setBlockParameter(Generator_Schalter, 'sw', '1');
simInput = simInput.setBlockParameter(Belastung_Schalter, 'sw', '1');
simInput = simInput.setBlockParameter(I_DC_Block, ...
    'Time', num2str(SprungStromquelle_t));
simInput = simInput.setBlockParameter(I_DC_Block, 'After', '0.2');

simOutput = sim(simInput);

t = simOutput.t;
n = simOutput.n;
i_M = simOutput.i;

fprintf('Fall c: n_end = %.1f U/min, i_end = %.4f A\n', n(end), i_M(end));


% ------ Drehzahl Plot ------

fig = figure();

plot(t, n, 'r', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
grid on;

ylabel('Drehzahl n in [U/min]');
xlabel('Zeit in [s]');

title(['7c: u_{Motor,in}: 0V \rightarrow 4V,' ...
    ' Generator an Stromquelle, I_{G,DC}: 0A \rightarrow +0.2A']);

exportgraphics(fig, fullfile(plots_directory, 'vb7c_n.svg'), ...
    ContentType='vector');

% ------ Strom Plot ------

fig = figure();

plot(t, i_M, 'b', 'LineWidth', 2);

xlim([0, Simulationsdauer]);
grid on;

ylabel('Motorstrom i_M in [A]');
xlabel('Zeit in [s]');

title(['7c: u_{Motor,in}: 0V \rightarrow 4V,' ...
    ' Generator an Stromquelle, I_{G,DC}: 0A \rightarrow +0.2A']);

exportgraphics(fig, fullfile(plots_directory, 'vb7c_i.svg'), ...
    ContentType='vector');
