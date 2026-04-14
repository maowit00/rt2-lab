% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%
% Skript zur Auslegung des P-Positions-Reglers

clear all; close all;

fprintf('\n\n');
fprintf('*************************************************\n');
fprintf('Skript zur Analyse der Positionsregelung mit PI-Regler\n');
fprintf('*************************************************\n\n');
% Parameter holen
% ACHTUNG: In params_m.m müssen die Parameter k_Rn, Tn_n
% nach Auslegung des Drehzahlreglers angepasst werden, bevor
% der Positionsregler ausgelegt wird!
params_m;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Streckenuebertragungsfunktion
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_el   = tf([1],[L R+R_shunt]);
G_mech = tf([1],[J friction_gain]);

%Vernachlässigung der elektrischen Zeitkonstanten des Motors L/R+R_shunt
G_el   = tf([1],[R+R_shunt]);
%Vernachlässigung der Zeitkonstanten des PT2 PWM-Filters
k_cond_pwm = k_cond_pwm_k;
%Vernachlässigung der Zeitkonstanten des PT1 Drehzahlmessfilters-Filters
k_cond_n = k_cond_n_k * k_ADC;

G_Strecke_n = k_cond_pwm * k_HPS_u * feedback(G_el * km * G_mech, km) * 60/(2*pi);
G_mess_n    = k_cond_n;

G_fuehrung_n = feedback(k_Rn * tf([Tn_n 1],[Tn_n 0]) * G_Strecke_n, G_mess_n); 
G_fuehrung_n = minreal( G_fuehrung_n );

G_Strecke_pos = G_fuehrung_n * (2*pi/60) * tf([1],[1 0]);               % Drehzahl von U/min -> rad/sec
G_Strecke_pos = minreal( G_Strecke_pos );                               % Position in rad

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Messglied fuer Position
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_mess_pos    = k_cond_pos;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% offener Kreis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GOL = k_Rpos * G_Strecke_pos * G_mess_pos;
GOL = minreal(GOL);

% PI-Positionsregler
fprintf('\n');
%k_Rpos = 1;
n_pos  = input('Nullstelle des PI-Positionsreglers bei  s = -1/Tn_pos = ');

Tn_pos = 1/abs(n_pos);
G_Rpos = k_Rpos * tf([Tn_pos 1],[Tn_pos 0]);

fprintf('Uebertragungsfunktion des offenen Regelkreises mit PI-Regler\n');
GOL = G_Rpos * GOL

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Wurzelortskurve des Positionsregelkreises mit PI-Regler
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(1)
rlocus(GOL);
grid on
txt = sprintf('Wurzelortskurve des Positionsregelkreises mit PI-Lageregler Tnpos = 1/%4.1f = %6.3f sec', 1/Tn_pos, Tn_pos);
title(txt)

fprintf('Verwendete P-Verstaerkung des Positionsreglers: %6.2f\n', k_Rpos);
fprintf('Verwendete Nachstellzeit  des Positionsreglers: %7.3f = 1/%4.1f [sec]\n', Tn_pos, 1/Tn_pos);
fprintf('\nUnterlagerter Drehzahlregelkreis mit ...\n');
fprintf('Verwendete P-Verstaerkung des Drehzahlreglers:  %6.2f\n', k_Rn);
fprintf('Verwendete Nachstellzeit  des Drehzahlreglers:  %7.3f [sec]\n', Tn_n);

