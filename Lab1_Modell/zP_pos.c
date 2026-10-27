/*  Laborversuch Regelungstechnik 2: Motorboard
    (C) 2017-2019 W.Lindermeir, W.Zimmermann

    Hochschule Esslingen
    Autoren:   W. Lindermeir und W.Zimmermann,  Dez. 2019

    Uebersetzen mit      mex *.c           Eingabe von der Matlab-Kommandozeile aus
    Hinweis:             Falls der C/C++-Compiler noch nicht fuer Matlab konfiguriert ist,
                         vor dem Uebersetzen 'mex -setup' aufrufen.
*/

#define S_FUNCTION_NAME zP_pos
// 2 Parameter fuer zeitdiskreten P--Regler: Abtastzeit T Reglerparameter Kp
#define NO_PARAMETERS 2
#define S_FUNCTION_LEVEL 2
#include "simstruc.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S);

#include "Sblock.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S)
{ 
    static double Kp;                           // Reglerparameter analoger Regler (aus Maske)

    if (init){                                  // Initialisierungen
        //#########################
        // Reglerparameter Kp holen
        //#########################
        if(ssGetSFcnParamsCount(S) != 2){
            ssSetErrorStatus(S,"Anzahl Parameter muss hier 2 sein!");
            return;
        }
        Kp = mxGetPr(ssGetSFcnParam(S, 1))[0];
        *u = 0;
    } else {                                    // Berechnung des Ausgangssignals
        // ToDo: Stellgroesse berechnen
        *u = 0;
    }
}

