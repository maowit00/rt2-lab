% Vorbereitungsaufgabe 4: PWM-Ansteuerung

% Labor 1 Regelungstechnik 2 - Motorboard

clear;
clc;

%% Konstanten
PWM_max = 255;
U_PWM_max = 3.3;
Verstaerkung = -3;
U_Offset = 5;

%% Funktionen (@ = anonyme Funktion)

% PWM-Wert -> PWM-Spannung
p_2_up = @(p) p / PWM_max * U_PWM_max;

% PWM-Spannung -> Motorspannung
p_2_uM = @(p) U_Offset + Verstaerkung * p_2_up(p);

%% PWM-Werte
PWM = [0, 128, 255];

%% Berechnung mit for-Schleife

for k = 1:length(PWM)

    U_PWM(k) = p_2_up(PWM(k));
    u_Motor(k) = p_2_uM(PWM(k));

end

%% Tabelle ausgeben

Ergebnisse = table(PWM', U_PWM', u_Motor', ...
    'VariableNames', {'PWM_Wert', 'U_PWM_V', 'u_Motor_in_V'});

disp(Ergebnisse);
