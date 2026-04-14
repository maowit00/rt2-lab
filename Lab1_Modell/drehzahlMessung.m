% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%
% Fehler der Drehzahlmessung
%

% Tragen Sie hier Ihre Messwerte fuer nTTL und nADC als Zeilenvektoren in aufsteigender Reihenfolge ein
% Einheit [U/min]

nTTL = [  ];    % <<<<<< hier Messwerte eintragen

nADC = [  ];    % <<<<<< hier Messwerte eintragen

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Ab hier keine Veränderungen notwendig
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clf

% Diagramm der Messwerte
plot(nTTL,nTTL,'b', nTTL, nADC, 'rO')

% Berechnen der Steigung und des Offsets der Ausgleichsgeraden (Details siehe 'doc polyfit')
P=polyfit(nTTL, nADC, 1);
nOfs = P(2);    % Offsetfehler in U/min --> Soll: 0
kn   = P(1);    % Steigung              --> Soll: 1.0

% Zeichnen der Ausgleichsgeraden (Details siehe 'doc polyval')
hold on
plot(nTTL, polyval(P, nTTL),'r');
hold off

txt = sprintf('Drehzahlmessung:  Steigung  kn=%6.4f (soll: 1.0)  --- Offset  nOfs=%6.0f U/min (soll: 0)\n', kn, nOfs);
title(txt)
grid minor
legend('Soll', 'Messwerte', 'Ausgleichsgerade','Location','NorthWest')
xlabel('nTTL')
ylabel('nADC')

disp(txt)



