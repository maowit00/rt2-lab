/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: posRegler.c
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

#include "posRegler.h"

/* Block parameters (default storage) */
P_posRegler_T posRegler_P =
{
    /* Variable: k_ADC
     * Referenced by: '<S1>/Saturation2'
     */
    204.6,

    /* Variable: k_cond_n_k
     * Referenced by: '<S1>/Saturation2'
     */
    -0.00032,

    /* Variable: n_max
     * Referenced by: '<S1>/Saturation2'
     */
    7800.0,

    /* Expression: [kp]
     * Referenced by: '<S2>/P'
     */
    0.3,

    /* Expression: [1]
     * Referenced by: '<S2>/P'
     */
    1.0,

    /* Expression: 0
     * Referenced by: '<S2>/P'
     */
    0.0
};

/* External inputs (root inport signals with default storage) */
ExtU_posRegler_T posRegler_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_posRegler_T posRegler_Y;

/* Model step function */
void posRegler_step(void)
{
    real_T rtb_Saturation2;
    real_T u1;
    real_T u2;

    /* DiscreteTransferFcn: '<S2>/P' incorporates:
     *  Inport: '<Root>/Ist'
     *  Inport: '<Root>/Soll'
     *  Sum: '<S2>/Differenz'
     */
    rtb_Saturation2 = (posRegler_U.Soll - posRegler_U.Ist) *
        posRegler_P.P_NumCoef / posRegler_P.P_DenCoef;

    /* Outport: '<Root>/StellU' */
    posRegler_Y.StellU = rtb_Saturation2;

    /* Saturate: '<S1>/Saturation2' */
    u1 = posRegler_P.n_max * posRegler_P.k_cond_n_k * posRegler_P.k_ADC;
    u2 = -posRegler_P.n_max * posRegler_P.k_cond_n_k * posRegler_P.k_ADC;
    if (rtb_Saturation2 > u2)
    {
        /* Outport: '<Root>/StellB' */
        posRegler_Y.StellB = u2;
    }
    else if (rtb_Saturation2 < u1)
    {
        /* Outport: '<Root>/StellB' */
        posRegler_Y.StellB = u1;
    }
    else
    {
        /* Outport: '<Root>/StellB' */
        posRegler_Y.StellB = rtb_Saturation2;
    }

    /* End of Saturate: '<S1>/Saturation2' */
}

/* Model initialize function */
void posRegler_initialize(void)
{
    /* Registration code */

    /* external inputs */
    (void)memset(&posRegler_U, 0, sizeof(ExtU_posRegler_T));

    /* external outputs */
    (void) memset((void *)&posRegler_Y, 0,
                  sizeof(ExtY_posRegler_T));
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
