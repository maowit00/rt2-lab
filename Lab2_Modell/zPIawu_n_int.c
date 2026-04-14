/*  Laborversuch Regelungstechnik 2: Motorboard
    (C) 2019 W.Lindermeir, W.Zimmermann

    Hochschule Esslingen
    Autoren:   W. Lindermeir und W.Zimmermann, Dez. 2019

    Uebersetzen mit      mex *.c           Eingabe von der Matlab-Kommandozeile aus
    Hinweis:             Falls der C/C++-Compiler noch nicht fuer Matlab konfiguriert ist,
                         vor dem Uebersetzen 'mex -setup' aufrufen.
*/

#define int8    char
#define int16   short
#define int32   long
#define uint8   unsigned char
#define uint16  unsigned short
#define uint32  unsigned long

#define RPM_NN    7                         // Skalierungsfaktor fuer P-Anteil
#define RPM_NI   14                         // Skalierungsfaktor fuer I-Anteil

#define S_FUNCTION_NAME zPIawu_n_int        // Dateiname ohne ".c"
#define NO_PARAMETERS 3
#define S_FUNCTION_LEVEL 2
#include "simstruc.h"
#include <limits.h>

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S);
int32 sat_add(int32 a, int32 b);

#include "Sblock.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S)
{   
    double T;              // Abtastzeit (aus Maske)
    double Kp;             // Reglerparameter Kp (aus Maske)
    double Tn;             // Nachstellzeit (aus Maske)
    static int32 kp_rpm_N; // skalierter Reglerparameter P-Anteil
    static int32 ki_rpm_M; // skalierter Reglerparameter I-Anteil

    int32 e_rpm = (int32) e;
    int32 up_rpm;               // P-Anteil
    int32 ui_rpm_M;             // neuer Wert des I-Anteils
    static int32 ui_rpm_M_1;    // alter Wert des I-Anteils
    int32 rpm_control_u;        // Stellgröße

    const int32 max_rpm_control_output = +127 * (1 << 4);  // Stellgroessenbeschraenkung oben
    const int32 min_rpm_control_output = -128 * (1 << 4);  // Stellgroessenbeschraenkung
                                                           // * 16 wegen Aufteilung von n_p in n_p1 n_p2

    if (init) {   
        // Parameter aus Maske holen
        T  = mxGetPr(ssGetSFcnParam(S, 0))[0];   // Abtastzeit (aus Maske)
        Kp = mxGetPr(ssGetSFcnParam(S, 1))[0];   // Reglerparameter Kp 
        Tn = mxGetPr(ssGetSFcnParam(S, 2))[0];   // Reglerparameter Tn (analoger Regler; aus Maske)
        // Skalierte Reglerparameter init
        kp_rpm_N = Kp * (1<<RPM_NN);          // 153
        ki_rpm_M = Kp * T / Tn * (1<<RPM_NI); // 590
        // I-Anteil init
        ui_rpm_M_1 = 0;
        rpm_control_u = 0;
    }else{
        // Sättigung Regelfehler
        e_rpm = (e_rpm >  1000000) ?  1000000 : e_rpm;
        e_rpm = (e_rpm < -1000000) ? -1000000 : e_rpm;

	      up_rpm   = (kp_rpm_N * e_rpm) >> RPM_NN;                // P Anteil 
        ui_rpm_M = sat_add( ki_rpm_M * e_rpm, ui_rpm_M_1);      // I Anteil (skaliert)
        rpm_control_u = sat_add( up_rpm, ui_rpm_M >> RPM_NI); // Stellgröße

        if (rpm_control_u > max_rpm_control_output){     // Anti-Windup
           ui_rpm_M = ui_rpm_M_1;          // I-Anteil einfrieren
	      } else if (rpm_control_u < min_rpm_control_output){
           ui_rpm_M = ui_rpm_M_1;          // I-Anteil einfrieren
	      }

        ui_rpm_M_1 = ui_rpm_M;                                // Index shift
        
        // Stellgrößenbeschränkung auf min_rpm_control_output <= . <= max_rpm_control_output
        // durch nachfolgendes Sättigungselement in Simulink
    }
    *u = (double) rpm_control_u;
}

int32 sat_add(int32 a, int32 b)
{
 int32 s = a + b;
 if( a > 0 && b > 0 && s < 0){
     s = LONG_MAX;
 }else if( a < 0 && b < 0 && ( s > 0 || s == LONG_MIN) ){
     s = -LONG_MAX;
 }
 return(s);
}

