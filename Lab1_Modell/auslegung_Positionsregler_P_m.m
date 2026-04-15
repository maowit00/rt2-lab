% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%
% Skript zur Auslegung des P-Positions-Reglers

clear all; close all;

fprintf('\n\n');
fprintf('*************************************************\n');
fprintf('Skript zur Dimensionierung des P-Positionsreglers \n');
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

G_Strecke_n = k_cond_pwm * k_HPS_u * feedback(G_el * km * G_mech, km) * 60/(2*pi);
G_mess_n    = k_cond_n;

G_fuehrung_n = feedback(k_Rn * tf([Tn_n 1],[Tn_n 0]) * G_Strecke_n, G_mess_n); 
G_fuehrung_n = minreal( G_fuehrung_n );

G_Strecke_pos = G_fuehrung_n * (2*pi/60) * tf([1],[1 0]);               % Drehzahl von U/min -> rad/sec
G_Strecke_pos = minreal( G_Strecke_pos );                               % Position in rad

fprintf('Streckenuebertragungsfunktion der Positions-Regelstrecke\n');
G_Strecke_pos
damp(G_Strecke_pos);

[num,den] = tfdata(G_Strecke_pos, 'v');
fprintf('Nullstellen\n');
roots(num)

fprintf('Gleichstromverstaerkung\n');
dcgain(G_Strecke_pos)

[Wn,Z,P] = damp(G_Strecke_pos);
fprintf('Reelle Zeitkonstanten der Positions-Regelstrecke:\n');
for i = 1:length(Z),
    if isreal(P(i)) == 1 && P(i) < 0 , % stabiler, reeller Pol
        fprintf('%f [sec] ', 1/Wn(i) );
    end;
end; % for i
fprintf('\n');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Messglied fuer Position
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_mess_pos    = k_cond_pos;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Auslegung des P-Positionsreglers
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
k_Rpos = 0.356;                 % ToDo hier den passenden Wert einstellen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% offener Kreis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GOL = k_Rpos * G_Strecke_pos * G_mess_pos;
GOL = minreal(GOL);

%figure
%rlocus(GOL);
%title('Wurzelortskurve der Positionsregelung gezeichnet fuer Ortskurvenparameter = 1');

figure
margin(GOL);
%title('Bodediagramm des offenen Positionsregelkreises gezeichnet mit bereits gewaehltem P-Regler');
txt = sprintf('Offener Positionsregelkreis mit P-Regler k_Rpos=%4.2f\n(Unterlagerter PI-Drehzahlregler k_Rn=%4.2f  Tn_n=%6.3f [sec])', k_Rpos, k_Rn, Tn_n);
legend(txt)


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Fuehrungsuebertragungsfunktion des Positionsregelkreises
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_fuehrung_pos = -400/(2*pi) * feedback( k_Rpos * G_Strecke_pos, G_mess_pos);
G_fuehrung_pos = minreal( G_fuehrung_pos );

fprintf('Resultierende Fuehrungsuebertragungsfunktion fuer Positionsreglerauslegung\n');
G_fuehrung_pos
damp(G_fuehrung_pos);

[num,den] = tfdata(G_fuehrung_pos, 'v');
fprintf('Nullstellen\n');
roots(num)

[Wn,Z,P] = damp(G_fuehrung_pos);
fprintf('Reelle Zeitkonstanten der Positions-Fuehrungsuebertragungsfunktion:\n');
for i = 1:length(Z),
    if isreal(P(i)) == 1 , 
        fprintf('%f [sec] ', 1/Wn(i) );
    end
end; % for i
fprintf('\n');

figure
step(G_fuehrung_pos);
title('Sprungantwort der Fuehrungsuebertragungsfunktion der Positionsregelung');

figure
bode(G_fuehrung_pos);
title('Bodediagramm der Fuehrungsuebertragungsfunktion der Positionsregelung');

figure
pzmap(G_fuehrung_pos);
title('Pol/Nullstellenplan der Fuehrungsuebertragungsfunktion der Positionsregelung');

[Gm,Pm,Wcg,Wcp] = margin(GOL);
fprintf('\n');
fprintf('Verwendete P-Verstaerkung des Positionsreglers: %f\n', k_Rpos);
fprintf('Positionsregelkreis: Phasenreserve %3.0f° bei %3.0f [rad/sec]\n', Pm, Wcp);
fprintf('\nUnterlagerter Drehzahlregelkreis mit ...\n');
fprintf('Verwendete P-Verstaerkung des Drehzahlreglers:  %f\n', k_Rn);
fprintf('Verwendete Nachstellzeit  des Drehzahlreglers:  %f [sec]\n', Tn_n);
