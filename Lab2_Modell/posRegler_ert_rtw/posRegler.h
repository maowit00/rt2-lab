/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: posRegler.h
 *
 * Code generated for Simulink model 'posRegler'.
 *
 * Model version                  : 6.0
 * Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
 * C/C++ source code generated on : Thu Jun 11 15:34:40 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Freescale->HC(S)12
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_posRegler_h_
#define RTW_HEADER_posRegler_h_
#include "rtwtypes.h"
#include <string.h>
#ifndef posRegler_COMMON_INCLUDES_
#define posRegler_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* posRegler_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Macros for accessing real-time model data structure */

/* External inputs (root inport signals with default storage) */
typedef struct
{
    real_T Soll;                       /* '<Root>/Soll' */
    real_T Ist;                        /* '<Root>/Ist' */
}
ExtU_posRegler_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct
{
    real_T StellU;                     /* '<Root>/StellU' */
    real_T StellB;                     /* '<Root>/StellB' */
}
ExtY_posRegler_T;

/* Parameters (default storage) */
struct P_posRegler_T_
{
    real_T k_ADC;                      /* Variable: k_ADC
                                        * Referenced by: '<S1>/Saturation2'
                                        */
    real_T k_cond_n_k;                 /* Variable: k_cond_n_k
                                        * Referenced by: '<S1>/Saturation2'
                                        */
    real_T n_max;                      /* Variable: n_max
                                        * Referenced by: '<S1>/Saturation2'
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
};

/* Parameters (default storage) */
typedef struct P_posRegler_T_ P_posRegler_T;

/* Block parameters (default storage) */
extern P_posRegler_T posRegler_P;

/* External inputs (root inport signals with default storage) */
extern ExtU_posRegler_T posRegler_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_posRegler_T posRegler_Y;

/* Model entry point functions */
extern void posRegler_initialize(void);
extern void posRegler_step(void);

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
 * hilite_system('Regelkreis_ZD/posRegler')    - opens subsystem Regelkreis_ZD/posRegler
 * hilite_system('Regelkreis_ZD/posRegler/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Regelkreis_ZD'
 * '<S1>'   : 'Regelkreis_ZD/posRegler'
 * '<S2>'   : 'Regelkreis_ZD/posRegler/zP'
 */
#endif                                 /* RTW_HEADER_posRegler_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
