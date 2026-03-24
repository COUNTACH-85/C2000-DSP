//*****************************************************************************
// pi_controller.h
// PI controller interface — no hardware dependencies whatsoever
// Can be unit-tested on any machine, not just the F28379D
//*****************************************************************************
#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

//-----------------------------------------------------------------------------
// PI controller configuration block
// Fill this in before calling PI_init()
//-----------------------------------------------------------------------------
typedef struct
{
    float kp;           // Proportional gain
    float ki;           // Integral gain (pre-multiplied by dt if desired)
    float dt;           // Sample period in seconds (0.1 for 100ms loop)
    float outMin;       // Output clamp minimum (e.g. 0.0f)
    float outMax;       // Output clamp maximum (e.g. 10000.0f = PWM_PERIOD)
    float integralMin;  // Anti-windup clamp min (typically same as outMin)
    float integralMax;  // Anti-windup clamp max (typically same as outMax)
} PI_Config;

//-----------------------------------------------------------------------------
// PI controller state — one instance per control loop
// Initialize with PI_init(), then call PI_update() every sample period
//-----------------------------------------------------------------------------
typedef struct
{
    PI_Config cfg;      // Configuration (copied in at init)
    float integral;     // Accumulated integral term
    float prevError;    // Previous error — reserved for future D term
    float lastOutput;   // Last computed output — useful for debugging
} PI_Controller;

//-----------------------------------------------------------------------------
// API
//-----------------------------------------------------------------------------

// Initialize controller and zero all state
void  PI_init(PI_Controller *ctrl, const PI_Config *cfg);

// Reset integrator and state (call on enable/disable transitions)
void  PI_reset(PI_Controller *ctrl);

// Run one control cycle; returns output clamped to [outMin, outMax]
float PI_update(PI_Controller *ctrl, float setpoint, float measured);

#endif // PI_CONTROLLER_H

