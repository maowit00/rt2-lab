% Vorbereitungsaufgabe 7a - Simulation der Strecke (Motorboard)
% RT2 Labor 1

clear;
close all;
clc;

%% Export SVG of Strecke.slx

open_system("Strecke");

plots_directory = 'plots/vb7';

if ~exist(plots_directory, 'dir')
    mkdir(plots_directory);
end

print("-sStrecke", "-dsvg", ...
    fullfile(plots_directory, "Strecke.svg"));

%% Run All Simulation

run vb7a.m
run vb7b.m
run vb7c.m
run vb7d.m
