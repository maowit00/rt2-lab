#ifndef __UTILS__
#define __UTILS__ 1

#include "main.h"
#include "hal.h"
#include <math.h>
#include <limits.h>

// define some types
enum c_state {rpm = 0, position, manual, end_cs};   // state machine for control toggling
enum d_state {disp_only = 0,                        // state machine for display, button, setvalue interpretation
              set_val_gen,                          // automatic waveform generator mode
              set_val_inc_dec, set_val_0_max, set_val_0_min, end_ds};

extern int64_t current_substep_int64;
extern const char *error_msg_l0;
extern const char *error_msg_l1;

//Handle buttons SW07_1, SW07_2, SW07_3, SW07_4
void sw_pressed_action(enum c_state *control_state, enum d_state *display_state,
                       int32_t *set_rpm, int64_t *set_position, int32_t *manual_u );

void SoftScopeSend(int16_t channel1, int16_t channel2);

uint16_t select_median(uint16_t a, uint16_t b, uint16_t c);

bool overload_I2dt(int32_t I_mA);

int32_t sat_add_int32(int32_t a, int32_t b);

#endif // #ifndef __UTILS__
