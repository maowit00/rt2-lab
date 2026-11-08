/*  Laborversuch Regelungstechnik 2: Motorboard
    (C) 2017-2019 W.Lindermeir, W.Zimmermann

    Hochschule Esslingen
    Autoren:   W. Lindermeir und W.Zimmermann,  Dez. 2019

    Uebersetzen mit      mex *.c           Eingabe von der Matlab-Kommandozeile aus
    Hinweis:             Falls der C/C++-Compiler noch nicht fuer Matlab konfiguriert ist,
                         vor dem Uebersetzen 'mex -setup' aufrufen.
*/

#define S_FUNCTION_NAME  zPIawu_n
// 3 Parameter fuer zeitdiskreten PI--Regler: Abtastzeit T und Reglerparameter (analog) Kp, Tn
#define NO_PARAMETERS 3
#define S_FUNCTION_LEVEL 2
#include "simstruc.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S);

#include "Sblock.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S)
{   
    double              T;                      // Abtastzeit (aus Maske)
    static double       Kp;                     // Reglerparameter Kp (aus Maske)
    double              Tn;                     // Nachstellzeit (aus Maske)
    static double       KI;                     // Reglerparameter digitaler Regler
    double              ui;                     // neuer Wert des I-Anteils
    const double        umax =  127,            // maximaler und
                        umin = -128;            // minimaler Stellgroessenwert

    double u_p;
    static double u_i;

    if (init){                    // Initialisierungen
        //########################################################
        // Parameter Abtastzeit T, Reglerparameter Kp und Tn holen
        //########################################################
        if(ssGetSFcnParamsCount(S) != 3){
            ssSetErrorStatus(S,"Anzahl Parameter muss hier 3 sein!");
            return;
        }
        T  = mxGetPr(ssGetSFcnParam(S, 0))[0];   // Abtastzeit (aus Maske)
        Kp = mxGetPr(ssGetSFcnParam(S, 1))[0];   // Reglerparameter Kp 
        Tn = mxGetPr(ssGetSFcnParam(S, 2))[0];   // Reglerparameter Tn (analoger Regler; aus Maske)

        //########################################################
        // Parameter des digitalen Reglers initialisieren
        //########################################################
        *u = u_i = 0;
        KI = Kp * T / Tn;
    } else {                                      // Berechnung des Ausgangssignals
        u_p = Kp * e;

        u_i += KI * e;

        if (u_p + u_i < umin)      *u = u_i = umin;
        else if (u_p + u_i > umax) *u = u_i = umax;
        else                       *u = u_p + u_i;
    }
}

