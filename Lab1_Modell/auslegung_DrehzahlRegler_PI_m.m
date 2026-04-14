% Laborversuch: Motorboard
%  Regelungstechnik 2
%  (C) 2019 W.Lindermeir, W.Zimmermann
%  Hochschule Esslingen
%
% Skript zur Auslegung des PI-Drehzahl-Reglers

clear all; close all;

fprintf('\n\n');
fprintf('*************************************************\n');
fprintf('Skript zur Dimensionierung des PI-Drehzahlreglers\n');
fprintf('*************************************************\n\n');
% Parameter einlesen
params_m;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Streckenuebertragungsfunktion
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_el   = tf([1],[L R+R_shunt]);
G_mech = tf([1],[J friction_gain]);

G_Strecke_n = k_cond_pwm * k_HPS_u * feedback(G_el * km * G_mech, km) * 60/(2*pi);

fprintf(1,'\n##### Regelstrecke ###################################################################\n');
fprintf('Streckenuebertragungsfunktion der Drehzahl-Regelstrecke\n');
G_Strecke_n
damp(G_Strecke_n);

[num,den] = tfdata(G_Strecke_n, 'v');
fprintf('\nNullstellen Regelstrecke\n');
roots(num)

fprintf('Gleichstromverstaerkung\n');
dcgain(G_Strecke_n)

%Suche nach der langsamsten Zeitkonstante der Regelstrecke
[Wn,Z,P] = damp(G_Strecke_n);    % damp sortiert die Zeitkonstanten 
fprintf('Reelle Zeitkonstanten der Drehzahl-Regelstrecke:\n');
ZK_erkannt = 0;
for i = 1:length(Z),
    if isreal(P(i)) == 1 ,
        if ZK_erkannt == 0,
           ZK_erkannt = 1;
           langsamste_ZK = 1/Wn(i);
        end;
        fprintf('%f [sec]       ', 1/Wn(i) );
    end;
end; % for i
fprintf('\n\n');

fprintf(1,'\n##### Offener Regelkreis ###################################################################\n');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Messglied fuer Drehzahl
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_mess_n    = k_cond_n;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Auslegung des PI-Drehzahlreglers
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Tn_n = langsamste_ZK;       % Kompensation der langsamsten Streckenzeitkonstanten
k_Rn = 0.1;                 % TODO: Hier die passende Reglerverstärkung einstellen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Tn_n = Tn_n;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Offener Kreis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
GOL = k_Rn * tf([Tn_n 1],[Tn_n 0]) * G_Strecke_n * G_mess_n;
GOL = minreal(GOL);     %Pol-Nullstellen-Kompensation ausführen, d.h. kürzen

%figure
%rlocus(GOL);
%title('Wurzelortskurve der Drehzahlregelung gezeichnet fuer Ortskurvenparameter = 1');

figure
margin(GOL);
txt = sprintf('Offener Drehzahlregelkreis mit PI-Regler kR_n=%4.2f  Tn_n=%6.3f [sec]', k_Rn, Tn_n);
legend(txt)

fprintf(1,'\n##### Geschlossener Regelkreis ############################################################\n');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Fuehrungsuebertragungsfunktion des geschlossenen Drehzahlregelkreises
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
G_fuehrung_n = k_cond_n_k*k_ADC*feedback(k_Rn * tf([Tn_n 1],[Tn_n 0]) * G_Strecke_n, G_mess_n); 
G_fuehrung_n = minreal( G_fuehrung_n );

fprintf('Fuehrungsuebertragungsfunktion des Drehzahlregelkreises\n');
G_fuehrung_n
damp(G_fuehrung_n);

[num,den] = tfdata(G_fuehrung_n, 'v');
fprintf('\nNullstellen Geschlossener Regelkreis\n');
roots(num)

[Wn,Z,P] = damp(G_fuehrung_n);
fprintf('Reelle Zeitkonstanten der Drehzahl-Fuehrungsuebertragungsfunktion:\n');
for i = 1:length(Z),
    if isreal(P(i)) == 1 , 
        fprintf('%f [sec]        ', 1/Wn(i) );
    end
end; % for i
fprintf('\n\n');

figure
step(G_fuehrung_n);
title('Sprungantwort der Fuehrungsuebertragungsfunktion der Drehzahlregelung');

figure
bode(G_fuehrung_n);
title('Bodediagramm der Fuehrungsuebertragungsfunktion der Drehzahlregelung');

figure
pzmap(G_fuehrung_n);
title('Pol/Nullstellenplan der Fuehrungsuebertragungsfunktion der Drehzahlregelung');

[Gm,Pm,Wcg,Wcp] = margin(GOL);

fprintf('\n');
fprintf('Verwendete P-Verstaerkung des Drehzahlreglers:    %f\n', k_Rn);
fprintf('Verwendete Nachstellzeit  des Drehzahlreglers:    %f [sec]\n', Tn_n);
fprintf('Drehzahlregelkreis: Phasenreserve %3.0f° bei %3.0f [rad/sec]\n', Pm, Wcp);

