% Laboraufgabe 2 / Tabelle 4: Messung der Motorkonstanten k_m

% Labor 1 - Motorboard Teil 1 (Anleitung Abschnitt 4.2.4)

% n_O = 60 / (100 * T_O)       mit T_O in sec, Ergebnis in U/min
% k_m = | U_EMK / (2*pi*n) |   mit U_EMK in V und n in U/sec

close all;
clear;
clc;

%% Messwerte (Tabelle 4)

PWM   = [220, 180, 75, 35];   % PWM-Wert
U_EMK = [NaN, NaN, NaN, NaN]; % Klemmenspannung-Motor[V] (Vorzeichen!!)
n_T   = [NaN, NaN, NaN, NaN]; % <<<<<< LCD-Anzeige nT [U/min] eintragen
n_E   = [NaN, NaN, NaN, NaN]; % <<<<<< LCD-Anzeige nE [U/min] eintragen
T_O   = [NaN, NaN, NaN, NaN]; % Periodendauer TTL-Signal am Oszi [us]

%% Berechnung

% Periodendauer [us] -> Drehzahl [U/min], Vorzeichen wie U_EMK
T_2_n = @(T, U) sign(U) * 60 ./ (100 * T * 1e-6);

% Drehzahl [U/min] und U_EMK [V] -> Motorkonstante [Nm/A]
n_2_km = @(n, U) abs(U ./ (2 * pi * n / 60));

n_O = T_2_n(T_O, U_EMK);
k_m = n_2_km(n_O, U_EMK);

k_m_mittel = mean(k_m);

%% MessFehler

km_params = 14.8e-3;               % Vergleichswert aus params_m.m [Nm/A]

abweichung = (k_m_mittel - km_params) / km_params * 100;

%% Ausgabe

zeile = @(name, werte, fmt) ...
    fprintf(['%-14s' repmat(fmt, 1, numel(werte)) '\n'], name, werte);

fprintf('\n');
zeile('PWM-Wert:',   PWM,   '%12g');
zeile('U_EMK [V]',   U_EMK, '%12g');
zeile('n_T [U/min]', n_T,   '%12.1f');
zeile('n_E [U/min]', n_E,   '%12.1f');
zeile('T_O [us]',    T_O,   '%12g');
zeile('n_O [U/min]', n_O,   '%12.1f');
zeile('k_m [Nm/A]',  k_m,   '%12.5f');


fprintf('\nErgebnis: Mittelwert von k_m = %.5f Nm/A\n', k_m_mittel);

fprintf('Abweichung (%.4f Nm/A): %+.1f %%\n', km_params, abweichung);

% mehr als +/-20 %% -> Rechnung und Einheiten pruefen!!

