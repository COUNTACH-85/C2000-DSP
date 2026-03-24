//*****************************************************************************
// hal.c — Hardware Abstraction Layer implementation
// Compile with HAL_SIM_MODE=1 for no-hardware bench testing
// Compile with HAL_SIM_MODE=0 for real F28379D peripheral access
//*****************************************************************************
#include "hal.h"
#include "driverlib.h"
#include "device.h"

//=============================================================================
// SIMULATION MODE — no hardware required
// The sim block generates a synthetic ramp setpoint and a first-order
// motor model so the PI controller has something real to fight against.
//=============================================================================
#if HAL_SIM_MODE == 1

#include <stdint.h>

//-----------------------------------------------------------------------------
// Simulated encoder state
// The sim increments a virtual pulse count that the PI loop reads
//-----------------------------------------------------------------------------
static volatile uint32_t sim_encoderCount = 0UL;

//-----------------------------------------------------------------------------
// Simulated motor model — first-order low-pass:
//   motorRPM += (targetRPM - motorRPM) * motorTau
// tau=0.15 means ~15% of the error closes per 100ms step,
// roughly mimicking a small DC motor's mechanical time constant
//-----------------------------------------------------------------------------
static float sim_motorRPM   = 0.0f;
static float sim_pwmDuty    = 0.0f;    // last PWM counts written
static const float sim_tau  = 0.15f;   // motor time constant (tune to feel real)

//-----------------------------------------------------------------------------
// Simulated ADC — ramps setpoint up and down slowly so you can watch
// the controller track it on the DSO and console without touching anything
//-----------------------------------------------------------------------------
static uint16_t sim_adcValue    = 0U;
static int16_t  sim_adcDelta    = 40;   // counts per 100ms step (+/-)

void HAL_initAll(void)  { 
    // We need to initialize the device and GPIO to actually output a signal!
    Device_init();
    Device_initGPIO();
    
    // Route EPWM1A to GPIO0 (J4/40)
    GPIO_setPinConfig(GPIO_0_EPWM1A);
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_OUT);
    
    HAL_initEPWM(); // Call the PWM setup
 }
void HAL_initADC(void)  { }
void HAL_initEPWM(void) {
    // Actually configure the hardware PWM module
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBasePeriod(EPWM1_BASE, PWM_PERIOD_COUNTS);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);
    EPWM_setSyncOutPulseMode(EPWM1_BASE, EPWM_SYNC_OUT_PULSE_DISABLED);
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 0U);
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
 }
void HAL_initEncoder(void) { }

//-----------------------------------------------------------------------------
// HAL_readADC() — returns a slowly-ramping synthetic ADC value (0–4095)
// Simulates someone turning the pot back and forth
//-----------------------------------------------------------------------------
uint16_t HAL_readADC(void)
{
    // Use a signed 32-bit integer for safe math before storing it
    int32_t tempValue = (int32_t)sim_adcValue + sim_adcDelta;

    // Bounce direction at limits
    if(tempValue >= 4095)
    {
        sim_adcValue = 4095U;
        sim_adcDelta = -40;     // start ramping down
    }
    else if(tempValue <= 0)
    {
        sim_adcValue = 0U;
        sim_adcDelta = 40;      // start ramping up
    }
    else
    {
        sim_adcValue = (uint16_t)tempValue;
    }

    return sim_adcValue;
}

//-----------------------------------------------------------------------------
// HAL_setPWMDuty() — stores PWM counts and drives the motor sim model
// The simulated motor RPM chases the PWM-implied target with lag
//-----------------------------------------------------------------------------
void HAL_setPWMDuty(uint16_t counts)
{
    sim_pwmDuty = (float)counts;

    // Convert PWM duty to implied motor RPM target
    // Linear model: full duty (10000 counts) = MAX_RPM_SETPOINT
    float targetRPM = (sim_pwmDuty / (float)PWM_PERIOD_COUNTS) * MAX_RPM_SETPOINT;

    // First-order lag — motor RPM approaches target at rate sim_tau per step
    sim_motorRPM += (targetRPM - sim_motorRPM) * sim_tau;

    // Compute how many encoder pulses would have arrived in 100ms at this RPM
    // pulses = (RPM / 60) * PPR * dt
    //        = (RPM / 60) * 374 * 0.1
    float pulsesF = (sim_motorRPM / 60.0f) * (float)ENCODER_PPR * 0.1f;
    sim_encoderCount += (uint32_t)pulsesF;

    // ADD THIS LINE: Actually push the simulated duty cycle to the real hardware pin
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, counts);
}

//-----------------------------------------------------------------------------
// HAL_getAndResetEncoderCount() — returns simulated pulse count, resets it
//-----------------------------------------------------------------------------
uint32_t HAL_getAndResetEncoderCount(void)
{
    uint32_t cnt    = sim_encoderCount;
    sim_encoderCount = 0UL;
    return cnt;
}

//=============================================================================
// REAL HARDWARE MODE
//=============================================================================
#else   // HAL_SIM_MODE == 0

//-----------------------------------------------------------------------------
// Real encoder count — written by XINT1 ISR (defined in main.c)
//-----------------------------------------------------------------------------
extern volatile uint32_t encoderPulseCount;

void HAL_initAll(void)
{
    Device_init();
    Device_initGPIOs();
    GPIO_setPinConfig(GPIO_0_EPWM1A);
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_OUT);
    HAL_initADC();
    HAL_initEPWM();
    HAL_initEncoder();
}

void HAL_initADC(void)
{
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000UL);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0,
                 ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 75U);
}

void HAL_initEPWM(void)
{
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSPCLKDIV_1);
    EPWM_setTimeBasePeriod(EPWM1_BASE, PWM_PERIOD_COUNTS);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);
    EPWM_setSyncOutPulseMode(EPWM1_BASE, EPWM_SYNC_OUT_PULSE_DISABLED);
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 0U);
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void HAL_initEncoder(void)
{
    GPIO_setDirectionMode(28U, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28U, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28U, GPIO_QUAL_3SAMPLE);
    GPIO_setInterruptPin(28U, GPIO_INT_XINT1);
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_RISING_EDGE);
    GPIO_enableInterrupt(GPIO_INT_XINT1);
    Interrupt_register(INT_XINT1, &encoderISR);
    Interrupt_enable(INT_XINT1);
}

uint16_t HAL_readADC(void)
{
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
    while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false) { ; }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    return ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
}

void HAL_setPWMDuty(uint16_t counts)
{
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, counts);
}

uint32_t HAL_getAndResetEncoderCount(void)
{
    DINT;
    uint32_t cnt     = encoderPulseCount;
    encoderPulseCount = 0UL;
    EINT;
    return cnt;
}

#endif  // HAL_SIM_MODE

