% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%

% Default Sprungwerte fuer PWM Sprungwertgenerator
u_pwm_a = 220;
u_pwm_e = 180;

% Motor und Generator (als Last)
R  = 11.9;        % Motoranschlusswiderstand [Ohm]
L  = 2e-3;        % Motorinduktivitaet [Henry]
km = 14.8e-3;     % Motorkonstante [Nm/A]
R_shunt = 1.0;    % Widerstand des Strommessshunts
R_Lampe = 12.4;   % Widerstand der Lampe auf dem HPS Motorboard

% Signal Conditioning fuer PWM
k_cond_pwm_k = 10/255;      k_cond_pwm_T = 752e-6; 
k_cond_pwm = k_cond_pwm_k * tf([1],[k_cond_pwm_T 1]);

% HPS Leistungsverstaerker
k_HPS_u    = 12/5;

% Signal Conditioning fuer i
k_cond_i_T = 560e-6;
k_cond_i   = tf([1],[k_cond_i_T 1]);

% Signal Conditioning fuer Drehzahl n
k_cond_n_T = 560e-6;
k_cond_n   = tf([1],[k_cond_n_T 1]);

% Umrechnung in pos_tick
k_cond_pos = 400/(2*pi);

% Traegheitsmomente
Jk = 0.7e-6;          % Traegheitsmoment [Nm/s^2] mit Schalter "links": klein
Jg = 1.5e-6;          % Traegheitsmoment [Nm/s^2] mit Schalter "rechts": gross
J  = Jk;             % Auswahl je nach J Schalter auf dem Motorboard

% Modellierung der Reibung 
friction_gain = 5.55028e-07;      % Beiwert fuer fluessige Reibung (Gleitreibung) in [Nmsec/rad]
friction_offset = 0.000160479;    % Beiwert fuer trockene Reibung (Haftreibung) in [Nm]

% Anfangswerte fuer Simulation
phi_AW = 0 * pi / 180; % Anfangswinkel in Grad
omega_AW = 0;          % Anfangswert fuer die Winkelgeschwindigkeit 

% Reglerauslegung
% PI - Drehzahlregler
k_Rn = 0.01;  % P-Verstaerkung PI-Drehzahlregler
Tn_n = 0.09;   % [sec] Nachstellzeit PI-Drehzahlregler (Pol/Nullstellenkompensation)
% Positionsregler
k_Rpos = 0.3;        % P-Verstaerkung P-Positionsregler
n_max = 7800;      % U/min fuer Stellgroessenbegrenzung des Positionsreglers

% Abtastzeit des uC in Sekunden
T = 1000e-6;     

% Ueberlastschutz des Motors
I_max_stat = 0.58;    % Maximal erlaubter Dauerstrom [A]
I2dt_max = 0.2;       % Maximal erlaubtes I2t [A^2s]
T_thermisch = 3;      % Thermische Zeitkonstante [s] fuer Abkuehlung nach Ueberlast
