
% Labor Regelungstechnik 2: Lab2

% %% Vorbereitungsaufgabe 6

%% -------------- Reset WorkSpace ---------------

clear;
close all;
clc;

slCharacterEncoding('UTF-8')

plots_directory = 'plots/vb6/';

if ~exist(plots_directory, 'dir')
  mkdir(plots_directory);
end

%% -------------- System Definition -------------

Z = [-20];              % Nullstellen
P = [0; -18.5; -120];   % Polstellen
K = 1;                  % Verstärkungsfaktor

G = zpk(Z, P, K);

%% -------------- WOK Plot ----------------------


fig = figure();

rlocus(G);

grid on;

title('Wurzelortskurve des Positionsregelkreises mit P-Regler');

exportgraphics(fig, ...
  [plots_directory, 'WOK_PRegel.png'], ...
  "Resolution", 300, "BackgroundColor", "white");

savefig(fig, [plots_directory, 'WOK_PRegel.fig']);

%% -------------- Bifurkationspunkte ------------

% %% Symbolic Math Toolbox

syms s

K_s = 0;

for j = 1:length(P)
    K_s = K_s + 1 /  (s - P(j));
end
for i = 1:length(Z)
    K_s = K_s - 1 / (s - Z(i));
end

bifurcation_roots = double(solve(K_s == 0, s));

fprintf('K(s) = \n\t');

disp(K_s);

disp(bifurcation_roots);

