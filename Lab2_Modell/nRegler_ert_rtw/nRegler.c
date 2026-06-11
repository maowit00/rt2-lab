/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: nRegler.c
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

#include "nRegler.h"

/* Block parameters (default storage) */
P_nRegler_T nRegler_P =
{
    /* Expression: 0
     * Referenced by: '<S2>/0'
     */
    0.0,

    /* Expression: [kp]
     * Referenced by: '<S2>/P'
     */
    1.2,

    /* Expression: [1]
     * Referenced by: '<S2>/P'
     */
    1.0,

    /* Expression: 0
     * Referenced by: '<S2>/P'
     */
    0.0,

    /* Expression: 0
     * Referenced by: '<S2>/Unit Delay'
     */
    0.0,

    /* Expression: 0
     * Referenced by: '<S2>/Switch'
     */
    0.0,

    /* Expression: [kp*T/Tn 0]
     * Referenced by: '<S2>/I'
     */
    {
        0.036070450097847359, 0.0
    },

    /* Expression: [1 -1]
     * Referenced by: '<S2>/I'
     */
    {
        1.0, -1.0
    },

    /* Expression: 0
     * Referenced by: '<S2>/I'
     */
    0.0,

    /* Expression: 127.5
     * Referenced by: '<S1>/Saturation1'
     */
    127.5,

    /* Expression: -127.5
     * Referenced by: '<S1>/Saturation1'
     */
    -127.5
};

/* Block states (default storage) */
DW_nRegler_T nRegler_DW;

/* External inputs (root inport signals with default storage) */
ExtU_nRegler_T nRegler_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_nRegler_T nRegler_Y;

/* Model step function */
void nRegler_step(void)
{
    real_T rtb_Regeldifferenz;
    real_T rtb_Saturation1;
    real_T rtb_u;

    /* Sum: '<S2>/Regel- differenz' incorporates:
     *  Inport: '<Root>/Ist'
     *  Inport: '<Root>/Soll'
     */
    rtb_Regeldifferenz = nRegler_U.Soll - nRegler_U.Ist;

    /* DiscreteTransferFcn: '<S2>/P' */
    rtb_u = rtb_Regeldifferenz * nRegler_P.P_NumCoef / nRegler_P.P_DenCoef;

    /* UnitDelay: '<S2>/Unit Delay' */
    rtb_Saturation1 = nRegler_DW.UnitDelay_DSTATE;

    /* Switch: '<S2>/Switch' incorporates:
     *  Constant: '<S2>/0'
     *  Fcn: '<S2>/Fcn'
     */
    if ((real_T)((rtb_Saturation1 > 127.5) || (rtb_Saturation1 < -127.5)) * -2.0
        + 1.0 < nRegler_P.Switch_Threshold)
    {
        rtb_Regeldifferenz = nRegler_P.u_Value;
    }

    /* End of Switch: '<S2>/Switch' */

    /* DiscreteTransferFcn: '<S2>/I' */
    rtb_Regeldifferenz = (rtb_Regeldifferenz - nRegler_P.I_DenCoef[1L] *
                          nRegler_DW.I_states) / nRegler_P.I_DenCoef[0];

    /* Sum: '<S2>/.' incorporates:
     *  DiscreteTransferFcn: '<S2>/I'
     */
    rtb_u += nRegler_P.I_NumCoef[0] * rtb_Regeldifferenz + nRegler_P.I_NumCoef
        [1L] * nRegler_DW.I_states;

    /* Outport: '<Root>/StellU' */
    nRegler_Y.StellU = rtb_u;

    /* Saturate: '<S1>/Saturation1' */
    if (rtb_u > nRegler_P.Saturation1_UpperSat)
    {
        /* Outport: '<Root>/StellB' */
        nRegler_Y.StellB = nRegler_P.Saturation1_UpperSat;
    }
    else if (rtb_u < nRegler_P.Saturation1_LowerSat)
    {
        /* Outport: '<Root>/StellB' */
        nRegler_Y.StellB = nRegler_P.Saturation1_LowerSat;
    }
    else
    {
        /* Outport: '<Root>/StellB' */
        nRegler_Y.StellB = rtb_u;
    }

    /* End of Saturate: '<S1>/Saturation1' */

    /* Update for UnitDelay: '<S2>/Unit Delay' */
    nRegler_DW.UnitDelay_DSTATE = rtb_u;

    /* Update for DiscreteTransferFcn: '<S2>/I' */
    nRegler_DW.I_states = rtb_Regeldifferenz;
}

/* Model initialize function */
void nRegler_initialize(void)
{
    /* Registration code */

    /* states (dwork) */
    (void) memset((void *)&nRegler_DW, 0,
                  sizeof(DW_nRegler_T));

    /* external inputs */
    (void)memset(&nRegler_U, 0, sizeof(ExtU_nRegler_T));

    /* external outputs */
    (void) memset((void *)&nRegler_Y, 0,
                  sizeof(ExtY_nRegler_T));

    /* InitializeConditions for UnitDelay: '<S2>/Unit Delay' */
    nRegler_DW.UnitDelay_DSTATE = nRegler_P.UnitDelay_InitialCondition;

    /* InitializeConditions for DiscreteTransferFcn: '<S2>/I' */
    nRegler_DW.I_states = nRegler_P.I_InitialStates;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
