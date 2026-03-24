//*****************************************************************************
// pi_controller.c
// Pure math — no driverlib, no hardware registers, fully portable
//*****************************************************************************
#include "pi_controller.h"

//-----------------------------------------------------------------------------
// PI_init()
// Copy config into controller struct and zero all runtime state
//-----------------------------------------------------------------------------
void PI_init(PI_Controller *ctrl, const PI_Config *cfg)
{
    ctrl->cfg       = *cfg;     // struct copy
    ctrl->integral  = 0.0f;
    ctrl->prevError = 0.0f;
    ctrl->lastOutput= 0.0f;
}

//-----------------------------------------------------------------------------
// PI_reset()
// Use this when the motor is disabled or setpoint jumps dramatically —
// prevents integrator carrying stale state into the new operating point
//-----------------------------------------------------------------------------
void PI_reset(PI_Controller *ctrl)
{
    ctrl->integral  = 0.0f;
    ctrl->prevError = 0.0f;
    ctrl->lastOutput= 0.0f;
}

//-----------------------------------------------------------------------------
// PI_update()
// Called once per sample period. Returns PWM output counts.
//
// Algorithm:
//   error    = setpoint - measured
//   integral += ki * error * dt          (accumulated over time)
//   integral  = clamp(integral)          (anti-windup)
//   output   = kp * error + integral
//   output   = clamp(output)             (hardware limit)
//-----------------------------------------------------------------------------
float PI_update(PI_Controller *ctrl, float setpoint, float measured)
{
    //-------------------------------------------------------------------------
    // 1. Error — positive means motor is too slow, negative means too fast
    //-------------------------------------------------------------------------
    float error = setpoint - measured;

    //-------------------------------------------------------------------------
    // 2. Integrate error over time
    //    Ki * error * dt = how much to add to integral this cycle
    //    Example: Ki=0.05, error=100 RPM, dt=0.1s → adds 0.5 counts/cycle
    //    Over 20 cycles (2 seconds) that's 10 PWM counts of I contribution
    //-------------------------------------------------------------------------
    ctrl->integral += ctrl->cfg.ki * error * ctrl->cfg.dt;

    //-------------------------------------------------------------------------
    // 3. Anti-windup clamp on integral term alone
    //    Prevents runaway accumulation when output is already saturated.
    //    Example: motor stalled, error=3000 RPM — without this clamp the
    //    integrator would wind up to millions of counts and take minutes
    //    to unwind once the motor starts moving.
    //-------------------------------------------------------------------------
    if(ctrl->integral > ctrl->cfg.integralMax)
        ctrl->integral = ctrl->cfg.integralMax;
    else if(ctrl->integral < ctrl->cfg.integralMin)
        ctrl->integral = ctrl->cfg.integralMin;

    //-------------------------------------------------------------------------
    // 4. Sum P and I terms
    //-------------------------------------------------------------------------
    float output = (ctrl->cfg.kp * error) + ctrl->integral;

    //-------------------------------------------------------------------------
    // 5. Clamp total output to hardware limits
    //-------------------------------------------------------------------------
    if(output > ctrl->cfg.outMax)       output = ctrl->cfg.outMax;
    else if(output < ctrl->cfg.outMin)  output = ctrl->cfg.outMin;

    //-------------------------------------------------------------------------
    // 6. Save state
    //-------------------------------------------------------------------------
    ctrl->prevError  = error;
    ctrl->lastOutput = output;

    return output;
}
