//*****************************************************************************
// main.c — PI Motor Speed Controller
// LAUNCHXL-F28379D @ 200MHz
//
// To run WITHOUT hardware: set HAL_SIM_MODE 1 in hal.h
// To run WITH hardware:    set HAL_SIM_MODE 0 in hal.h
//
// DSO probe points (real hardware mode):
//   GPIO0  — ePWM1A output, 20kHz PWM, duty tracks PI output
//   GPIO28 — encoder input (probe here to verify signal quality)
//
// DSO probe points (sim mode):
//   GPIO0 — still outputs PWM! The sim drives HAL_setPWMDuty which
//            writes CMPA, so you can watch the PI output on the DSO
//            even with no motor connected. The PWM waveform's duty
//            cycle will slowly track the simulated setpoint ramp.
//*****************************************************************************

#include "driverlib.h"
#include "device.h"
#include "hal.h"
#include "pi_controller.h"
#include <stdio.h>


// This is the key line — tells CCS linker to pull in
// the JTAG CIO (Character I/O) printf implementation
// Without this, printf silently does nothing on C2000
extern int add_device();    // not needed — see below

//-----------------------------------------------------------------------------
// Encoder ISR — only compiled in real hardware mode
// In sim mode the encoder is updated inside HAL_setPWMDuty()
//-----------------------------------------------------------------------------
#if HAL_SIM_MODE == 0
volatile uint32_t encoderPulseCount = 0UL;

__interrupt void encoderISR(void)
{
    encoderPulseCount++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
#endif

//-----------------------------------------------------------------------------
// PI tuning parameters — adjust these on the bench
// See tuning guide in the previous response for methodology
//-----------------------------------------------------------------------------
#define KP_INIT         1.0f
#define KI_INIT         0.05f
#define SAMPLE_PERIOD_S 0.1f    // 100ms loop

//-------------------------------------------------------------------------
    // Loop variables
    //-------------------------------------------------------------------------
    uint16_t adcRaw       = 0;
    float    setpointRPM  = 0.0f;
    float    measuredRPM  = 0.0f;
    float    pwmOutput    = 0.0f;
    uint16_t pwmCounts    = 0;
    uint32_t pulses       = 0UL;
    uint32_t loopCount    = 0UL;    // for periodic console output pacing

//=============================================================================
// main()
//=============================================================================
void main(void)
{
    //-------------------------------------------------------------------------
    // Board and peripheral init — HAL abstracts real vs sim
    //-------------------------------------------------------------------------
    HAL_initAll();
    EINT;
    ERTM;

    //-------------------------------------------------------------------------
    // Configure PI controller
    //-------------------------------------------------------------------------
    PI_Config cfg = {
        .kp           = KP_INIT,
        .ki           = KI_INIT,
        .dt           = SAMPLE_PERIOD_S,
        .outMin       = 0.0f,
        .outMax       = (float)PWM_PERIOD_COUNTS,
        .integralMin  = -(float)PWM_PERIOD_COUNTS,
        .integralMax  =  (float)PWM_PERIOD_COUNTS
    };

    PI_Controller piCtrl;
    PI_init(&piCtrl, &cfg);

    

    printf("\r\n=== PI Motor Controller Started ===\r\n");
#if HAL_SIM_MODE == 1
    printf("    *** SIMULATION MODE — no hardware required ***\r\n");
    printf("    Setpoint ramps 0->3000->0 RPM automatically\r\n");
    printf("    Probe GPIO0 on DSO to see PWM duty tracking\r\n\r\n");
#endif

    for(;;)
    {
        //---------------------------------------------------------------------
        // 1. Wait 100ms — this is your control period
        //    In a production design you'd use a timer interrupt here,
        //    but DEVICE_DELAY_US is fine for bench validation
        //---------------------------------------------------------------------
        DEVICE_DELAY_US(100000UL);

        //---------------------------------------------------------------------
        // 2. Snapshot encoder pulses from the last 100ms window
        //    HAL handles the atomic read and reset internally
        //---------------------------------------------------------------------
        pulses      = HAL_getAndResetEncoderCount();

        // Convert pulses → RPM
        // pulses/PPR = revolutions in 100ms
        // * 10 = revolutions per second
        // * 60 = RPM
        measuredRPM = ((float)pulses / (float)ENCODER_PPR) * 600.0f;

        //---------------------------------------------------------------------
        // 3. Read setpoint from ADC (pot or simulated ramp)
        //    Scale 0–4095 → 0–MAX_RPM_SETPOINT
        //---------------------------------------------------------------------
        adcRaw      = HAL_readADC();
        setpointRPM = ((float)adcRaw / 4095.0f) * MAX_RPM_SETPOINT;

        //---------------------------------------------------------------------
        // 4. Run PI controller — returns PWM counts [0, PWM_PERIOD_COUNTS]
        //---------------------------------------------------------------------
        pwmOutput = PI_update(&piCtrl, setpointRPM, measuredRPM);
        pwmCounts = (uint16_t)pwmOutput;

        //---------------------------------------------------------------------
        // 5. Write PWM duty cycle
        //    In sim mode: this drives the motor model AND writes ePWM CMPA
        //    In real mode: this writes the ePWM register directly
        //---------------------------------------------------------------------
        HAL_setPWMDuty(pwmCounts);

        //---------------------------------------------------------------------
        // 6. Print telemetry every 5 loops (500ms) to avoid JTAG flooding
        //    Printing every 100ms creates a visible slowdown in the loop
        //---------------------------------------------------------------------
        loopCount++;
        // if(loopCount >= 5U)
        // {
        //     loopCount = 0U;

        //     // Duty cycle as a percentage for readability
        //     float dutyPct = ((float)pwmCounts / (float)PWM_PERIOD_COUNTS) * 100.0f;

        //     printf("SP: %6.1f RPM | Meas: %6.1f RPM | PWM: %5u/10000 (%4.1f%%)"
        //            " | Err: %+6.1f | I: %+7.2f\r\n",
        //            setpointRPM,
        //            measuredRPM,
        //            pwmCounts,
        //            dutyPct,
        //            setpointRPM - measuredRPM,   // live error
        //            piCtrl.integral);             // integrator state
        // }
    }
}


