//*****************************************************************************
// hal.h — Hardware Abstraction Layer
// Swap HAL_SIM_MODE on/off to toggle between real hardware and simulation
//*****************************************************************************
#ifndef HAL_H
#define HAL_H

#include <stdint.h>

//-----------------------------------------------------------------------------
// SET THIS TO 1 FOR BENCH TESTING WITHOUT PERIPHERALS
// SET TO 0 WHEN REAL HARDWARE IS CONNECTED
//-----------------------------------------------------------------------------
#define HAL_SIM_MODE    1

#define PWM_PERIOD_COUNTS   10000U      // ePWM1 period counts (20kHz @ 200MHz)
#define ENCODER_PPR         374U        // pulses per revolution — match yours
#define MAX_RPM_SETPOINT    3000.0f     // map full pot swing to this RPM

//-----------------------------------------------------------------------------
// Init functions — in sim mode these are mostly no-ops
//-----------------------------------------------------------------------------
void HAL_initAll(void);         // call once in main before loop
void HAL_initADC(void);
void HAL_initEPWM(void);
void HAL_initEncoder(void);

//-----------------------------------------------------------------------------
// Runtime I/O — in sim mode these return synthetic values
//-----------------------------------------------------------------------------
uint16_t HAL_readADC(void);                     // returns 0–4095
void     HAL_setPWMDuty(uint16_t counts);        // writes CMPA register
uint32_t HAL_getAndResetEncoderCount(void);      // atomic snapshot + reset

#endif // HAL_H

