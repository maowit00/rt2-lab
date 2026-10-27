#ifndef __ENCODER__
#define __ENCODER__ 1

/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "hal.h"

//
// ---- quadrature encoder interface with sub-step accuracy
//
// At low speeds, or high sample rates, a quadrature encoder can produce a small
// number of steps per sample, which makes any speed estimate made from those
// counts to be very noisy.
//
// This code uses a PIO program to count steps like the standard quadrature
// encoder example, but also mark the timestamp of the last step transition,
// which then allows the sampling code to measure the actual speed by computing
// the time passed between transitions and the "distance" traveled. See
// README.md for more details


// the quadrature encoder code starts here

typedef struct substep_state_t {
    // configuration data:
    uint calibration_data[4]; // relative phase sizes
    uint clocks_per_us;       // save the clk_sys frequency in clocks per us
    uint idle_stop_samples;   // after these samples without transitions, assume the encoder is stopped
    PIO pio;
    uint sm;

    // internal fields to keep track of the previous state:
    uint prev_trans_pos, prev_trans_us;
    uint prev_step_us;
    uint prev_low, prev_high;
    uint idle_stop_sample_count;
    int speed_2_20;
    int stopped;

    // output of the encoder update function:
    int speed;     // estimated speed in substeps per second
    uint position; // estimated position in substeps

    uint raw_step; // raw step count
} substep_state_t;

void quadrature_encoder_substep_get_counts(PIO pio, uint sm, uint *step, int *cycles, uint *us);
void quadrature_encoder_substep_program_init(PIO pio, uint sm, uint pin_A);

// main functions to be used by user code

void substep_init_state(PIO pio, int sm, int pin_a, substep_state_t *state);

void substep_update(substep_state_t *state);

void substep_calibrate_phases(PIO pio, uint sm);

void substep_set_calibration_data(substep_state_t *state, int step0, int step1, int step2);

#endif  // #ifndef __ENCODER__
