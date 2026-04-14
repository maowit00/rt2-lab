% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%

% erste Initialisierung der Parameter 

% Motor und Generator (als Last)
R  = 11.9;             % Motoranschlusswiderstand [Ohm]
L  = 2e-3;             % Motorinduktivitaet [Henry]
km = 14.8e-3;          % Motorkonstante [Nm/A]
R_shunt = 1;           % Widerstand des Strommessshunts

% Traegheitsmomente
J = 1e-6;         % Traegheitsmoment [Nm/s^2] mit Schalter "links": klein

% Modellierung der Reibung 
friction_gain = 6e-07;     % Beiwert fuer fluessige Reibung (Gleitreibung)

% Lasten
R_Lampe = 12.4;        % Widerstand der Lampe auf dem HPS Motorboard

phi_AW = 0 * pi / 180; % Anfangswinkel in Grad
omega_AW = 0;          % Anfangswert fuer die Winkelgeschwindigkeit 

