/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: nRegler.h
 *
 * Code generated for Simulink model 'nRegler'.
 *
 * Model version                  : 6.0
 * Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
 * C/C++ source code generated on : Thu Jun 11 15:34:26 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Freescale->HC(S)12
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_nRegler_h_
#define RTW_HEADER_nRegler_h_
#include "rtwtypes.h"
#include <string.h>
#ifndef nRegler_COMMON_INCLUDES_
#define nRegler_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* nRegler_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Macros for accessing real-time model data structure */

/* Block states (default storage) for system '<Root>' */
typedef struct
{
    real_T UnitDelay_DSTATE;           /* '<S2>/Unit Delay' */
    real_T I_states;                   /* '<S2>/I' */
}
DW_nRegler_T;

/* External inputs (root inport signals with default storage) */
typedef struct
{
    real_T Soll;                       /* '<Root>/Soll' */
    real_T Ist;                        /* '<Root>/Ist' */
}
ExtU_nRegler_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct
{
    real_T StellU;                     /* '<Root>/StellU' */
    real_T StellB;                     /* '<Root>/StellB' */
}
ExtY_nRegler_T;

/* Parameters (default storage) */
struct P_nRegler_T_
{
    real_T u_Value;                    /* Expression: 0
                                        * Referenced by: '<S2>/0'
                                        */
    real_T P_NumCoef;                  /* Expression: [kp]
                                        * Referenced by: '<S2>/P'
                                        */
    real_T P_DenCoef;                  /* Expression: [1]
                                        * Referenced by: '<S2>/P'
                                        */
    real_T P_InitialStates;            /* Expression: 0
                                        * Referenced by: '<S2>/P'
                                        */
    real_T UnitDelay_InitialCondition; /* Expression: 0
                                        * Referenced by: '<S2>/Unit Delay'
                                        */
    real_T Switch_Threshold;           /* Expression: 0
                                        * Referenced by: '<S2>/Switch'
                                        */
    real_T I_NumCoef[2];               /* Expression: [kp*T/Tn 0]
                                        * Referenced by: '<S2>/I'
                                        */
    real_T I_DenCoef[2];               /* Expression: [1 -1]
                                        * Referenced by: '<S2>/I'
                                        */
    real_T I_InitialStates;            /* Expression: 0
                                        * Referenced by: '<S2>/I'
                                        */
    real_T Saturation1_UpperSat;       /* Expression: 127.5
                                        * Referenced by: '<S1>/Saturation1'
                                        */
    real_T Saturation1_LowerSat;       /* Expression: -127.5
                                        * Referenced by: '<S1>/Saturation1'
                                        */
};

/* Parameters (default storage) */
typedef struct P_nRegler_T_ P_nRegler_T;

/* Block parameters (default storage) */
extern P_nRegler_T nRegler_P;

/* Block states (default storage) */
extern DW_nRegler_T nRegler_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_nRegler_T nRegler_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_nRegler_T nRegler_Y;

/* Model entry point functions */
extern void nRegler_initialize(void);
extern void nRegler_step(void);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('Regelkreis_ZD/nRegler')    - opens subsystem Regelkreis_ZD/nRegler
 * hilite_system('Regelkreis_ZD/nRegler/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Regelkreis_ZD'
 * '<S1>'   : 'Regelkreis_ZD/nRegler'
 * '<S2>'   : 'Regelkreis_ZD/nRegler/zPI_AWU1'
 */
#endif                                 /* RTW_HEADER_nRegler_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
