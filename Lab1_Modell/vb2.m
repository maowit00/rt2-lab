% Vorbereitungsaufgabe 2: Berechnung der stationaeren Motordrehzahl

% Labor 1 Regelungstechnik 2 - Motorboard

clear;
clc;

%% 1. Gegebene Konstanten aus Tabelle 1 (Seite 9)

U_N   = 12;       % Nennspannung [V]
R     = 11.9;     % Wicklungswiderstand [Ohm]
I_0   = 20e-3;    % Leerlaufstrom [A]
n_0   = 7800;     % Leerlaufdrehzahl [U/min]
I_max = 580e-3;   % Maximaler Dauerstrom [A]

%% 2. Elektrische Nennleistung

P_el_N = U_N * I_max;

%% 3. Elektrische Verlustleistung bei Nennstrom

P_V_el_N = R * I_max^2;

%% 4. Elektrische Leerlaufleistung

P_el_0 = U_N * I_0;

%% 5. Elektrische Verlustleistung im Leerlauf

P_V_el_0 = R * I_0^2;

%% 6. Mechanische Reibungsverluste

P_mech_0 = P_el_0 - P_V_el_0;

%% 7. Berechnung des Reibbeiwertes

% Umrechnung der Drehzahl von U/min in U/s
n_0_s = n_0 / 60;

% Winkelgeschwindigkeit berechnen
omega_0 = 2 * pi * n_0_s;

% Reibbeiwert berechnen
d = P_mech_0 / omega_0^2;

%% 8. Induzierte Spannung im Leerlauf

U_EMK_0 = U_N - R * I_0;

%% 9. Induzierte Spannung bei maximalem Dauerstrom

U_EMK_N = U_N - R * I_max;

%% 10. Stationaere Nenndrehzahl berechnen

% Gegenspannung ist proportional zur Drehzahl
n_N = n_0 * (U_EMK_N / U_EMK_0);

%% 11. Ergebnisse ausgeben

fprintf('\n========================================\n');
fprintf('  VORBEREITUNGSAUFGABE 2 - MOTORBOARD\n');
fprintf('========================================\n\n');

fprintf('Elektrische Nennleistung:\n');
fprintf('Pel_N     = %.4f W\n\n', P_el_N);

fprintf('Elektrische Verlustleistung bei Nennstrom:\n');
fprintf('PV_el_N   = %.5f W\n\n', P_V_el_N);

fprintf('Elektrische Leerlaufleistung:\n');
fprintf('Pel_0     = %.4f W\n\n', P_el_0);

fprintf('Verlustleistung im Leerlauf:\n');
fprintf('PV_el_0   = %.5f W\n\n', P_V_el_0);

fprintf('Mechanische Reibungsverluste:\n');
fprintf('Pmech_0   = %.5f W\n\n', P_mech_0);

fprintf('Reibbeiwert:\n');
fprintf('d         = %.4e Nm*s/rad\n\n', d);

fprintf('Induzierte Spannung im Leerlauf:\n');
fprintf('UEMK_0    = %.4f V\n\n', U_EMK_0);

fprintf('Induzierte Spannung bei Nennstrom:\n');
fprintf('UEMK_N    = %.4f V\n\n', U_EMK_N);

fprintf('========================================\n');
fprintf('STATIONAERE NENNDREHZAHL:\n');
fprintf('n_N       = %.2f U/min\n', n_N);
fprintf('========================================\n');
