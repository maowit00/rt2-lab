% Vorbereitungsaufgabe 5: ADC-Werte berechnen

% Labor 1 Regelungstechnik 2 - Motorboard

clear;
clc;

%% Konstanten

ADC_max = 4095;
U_ADC_max = 3.3;
U_Offset = 1.65;

% Tachogenerator
K_Tacho = 2 / 1000;
G_Tacho = -0.106;

% Stromsensor
K_Strom = 1 / 0.1;
G_Strom = -0.16;

%% Vorgegebene Werte

n = [-7800, -7000, 0, 7000, 7800];
i = [-1, 0, 1];

%% Drehzahl berechnen

for k = 1:length(n)

    ADC_n(k) = n_2_ADC(n(k), K_Tacho, G_Tacho, ...
        U_Offset, U_ADC_max, ADC_max);

end

%% Strom berechnen

for k = 1:length(i)

    ADC_i(k) = i_2_ADC(i(k), K_Strom, G_Strom, ...
        U_Offset, U_ADC_max, ADC_max);

end

%% Tabellen ausgeben

T_Drehzahl = table(n', ADC_n', ...
    'VariableNames', {'Drehzahl', 'ADC_Wert'});

T_Strom = table(i', ADC_i', ...
    'VariableNames', {'Strom', 'ADC_Wert'});

disp(T_Drehzahl);
disp(T_Strom);


%% Umwandlung Funktionen

function adc = n_2_ADC(n, K, G, Offset, U_max, ADC_max)

    U = Offset + G * K * n;
    adc = round(U / U_max * ADC_max);
    adc = min(ADC_max, max(0, adc));

end


function adc = i_2_ADC(i, K, G, Offset, U_max, ADC_max)

    U = Offset + G * K * i;
    adc = round(U / U_max * ADC_max);
    adc = min(ADC_max, max(0, adc));

end
