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
//  ToDo: eventuell weitere Groessen definieren

    if (init){                    // Initialisierungen
        //########################################################
        // Parameter Abtastzeit T, Reglerparameter Kp und Tn holen
        //########################################################
        if(ssGetSFcnParamsCount(S) != 3){
            ssSetErrorStatus(S,"Anzahl Parameter muss hier 3 sein!");
            return;
        }
        T  = mxGetPr(ssGetSFcnParam(S, 0))[0];   // Abtastzeit (aus Maske)
        Kp = mxGetPr(ssGetSFcnParam(S, 1))[0];	 // Reglerparameter Kp 
        Tn = mxGetPr(ssGetSFcnParam(S, 2))[0];   // Reglerparameter Tn (analoger Regler; aus Maske)

        //########################################################
        // Parameter des digitalen Reglers initialisieren
        //########################################################
//	ToDo: Parameter und Signale initialisieren
    } else {                                      // Berechnung des Ausgangssignals
//	ToDo: Regler mit Anti-Windup-Stellgroessenbegrenzung implementieren
	    *u = 0;
    }
}

