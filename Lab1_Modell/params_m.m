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
k_cond_pwm_k = -5/255*2;      k_cond_pwm_T1 = 340e-6;      k_cond_pwm_T2 = 400e-6;
k_cond_pwm = k_cond_pwm_k * tf([1],[k_cond_pwm_T1 1]) * tf([1],[k_cond_pwm_T2 1]);

% HPS Leistungsverstaerker
k_HPS_u    = 12/5;

% AD-Converter
k_ADC = 1023/5;

% Signal Conditioning fuer i
k_cond_i_k = -1/0.1 * 1/4;      k_cond_i_T = 2.2e-3;
k_cond_i   = k_cond_i_k * k_ADC * tf([1],[k_cond_i_T 1]);

% Signal Conditioning fuer Drehzahl n
k_cond_n_k = -2/1000 * 0.16;  k_cond_n_T = 4.7e-3;
k_cond_n   = k_cond_n_k * k_ADC * tf([1],[k_cond_n_T 1]);

% Umrechnung in pos_tick
k_cond_pos = -400/(2*pi);

% Traegheitsmomente
Jk = .7e-6;          % Traegheitsmoment [Nm/s^2] mit Schalter "links": klein
Jg = 2.5e-6;          % Traegheitsmoment [Nm/s^2] mit Schalter "rechts": gross % zu ändern in Vorbereitung
J  = Jk;             % Auswahl je nach J Schalter auf dem Motorboard

% Modellierung der Reibung 
friction_gain = 5.55028e-07;      % Beiwert fuer fluessige Reibung (Gleitreibung) in [Nmsec/rad]
friction_offset = 0.000160479;    % Beiwert fuer trockene Reibung (Haftreibung) in [Nm]

% Anfangswerte fuer Simulation
phi_AW = 0 * pi / 180; % Anfangswinkel in Grad
omega_AW = 0;          % Anfangswert fuer die Winkelgeschwindigkeit 

% Reglerauslegung
% PI - Drehzahlregler
k_Rn = 0.1;          % P-Verstaerkung PI-Drehzahlregler 
Tn_n = 0.04;   % [sec] Nachstellzeit PI-Drehzahlregler (Pol/Nullstellenkompensation)
% Positionsregler
k_Rpos = 1;        % P-Verstaerkung P-Positionsregler
n_max = 7800;      % U/min fuer Stellgroessenbegrenzung des Positionsreglers

% Abtastzeit des uC in Sekunden
T = 1536e-6;

% Ueberlastschutz des Motors
I_max_stat = 0.58;    % Maximal erlaubter Dauerstrom [A]
I2dt_max = 0.2;       % Maximal erlaubtes I2t [A^2s]
T_thermisch = 3;      % Thermische Zeitkonstante [s] fuer Abkuehlung nach Ueberlast
