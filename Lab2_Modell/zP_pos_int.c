/*  Laborversuch Regelungstechnik 2: Motorboard
    (C) 2019 W.Lindermeir, W.Zimmermann

    Hochschule Esslingen
    Autoren:   W. Lindermeir und W.Zimmermann, Dez. 2019

    Uebersetzen mit      mex *.c           Eingabe von der Matlab-Kommandozeile aus
    Hinweis:             Falls der C/C++-Compiler noch nicht fuer Matlab konfiguriert ist,
                         vor dem Uebersetzen 'mex -setup' aufrufen.
*/

// *******************************************************************************************
// Verwenden Sie diese vordefinierten Datentypen für Integergrößen
#define int8    char
#define int16   short
#define int32   long
#define uint8   unsigned char
#define uint16  unsigned short
#define uint32  unsigned long

// *******************************************************************************************
// Groessen, die Sie mit einem sinnvollen Wert belegen muessen: 
// *******************************************************************************************
// ToDo: Festlegung der Skalierungsfaktoren 
#define POS_Np    0       // Skalierungsfaktor für Positionsregler
#define POS_Np1   0       // Skalierungsfaktor für Positionsregler

#define S_FUNCTION_NAME zP_pos_int    // Dateiname ohne ".c"
#define NO_PARAMETERS 2               // 2 Parameter aus Maske: Abtastzeit und k_Rpos des P-Reglers
#define S_FUNCTION_LEVEL 2
#include <assert.h>
#include "simstruc.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S);

#include "Sblock.h"

void S_FUNCTION_NAME(double e, double *u, int init, SimStruct *S)
{   
    double k_Rpos;                   // Reglerparameter aus Maske
    static int32 kp_pos_N;           // Skalierter Reglerparameter des Positionsreglers
    int32 e_position = (int32) e;    // Regelfehler in Ticks (400 Ticks pro Umdrehung)
    int32 set_rpm;                   // Stellgroesse des Positionsreglers bzw. Sollwert fuer den unterlagerten Drehzahlregler

    if(init){
        assert(POS_Np >= POS_Np1);
        k_Rpos = mxGetPr(ssGetSFcnParam(S, 1))[0];   // Reglerparameter k_Rpos aus Maske
        set_rpm = 0;
        // Skalierte Reglerparameter init
        // ToDo: Bestimmen von kp_pos_N
        kp_pos_N = 0;
    }else{
        // Sättigung Regelfehler
        e_position = (e_position >  56000000) ?  56000000 : e_position;
        e_position = (e_position < -56000000) ? -56000000 : e_position;

        // ToDo: Bestimmen der Stellgröße
        set_rpm = 0;
    }
    // Stellgrößenbeschränkung durch nachfolgendes Sättigungselement in Simulink
    *u = (double) set_rpm;
}
