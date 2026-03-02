#include "driverlib.h"
#include "device.h"

#define RESULTS_BUFFER_SIZE 100
#pragma DATA_SECTION(adcDataBuffer, "ramgs0"); // Place in Global Shared RAM 0
uint16_t adcDataBuffer[RESULTS_BUFFER_SIZE];

uint16_t adcResult;

void initDAC(void)
{
    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_ADC_VREFHI);
    DAC_enableOutput(DACA_BASE);
    DAC_setShadowValue(DACA_BASE, 0);
}

void initEPWM1(void)
{
    EPWM_setTimeBasePeriod(EPWM1_BASE, 6000);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);

    EPWM_setCounterCompareValue(EPWM1_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                3000);

    EPWM_setActionQualifierAction(
        EPWM1_BASE,
        EPWM_AQ_OUTPUT_A,
        EPWM_AQ_OUTPUT_HIGH,
        EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO
    );

    EPWM_setActionQualifierAction(
        EPWM1_BASE,
        EPWM_AQ_OUTPUT_A,
        EPWM_AQ_OUTPUT_LOW,
        EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA

    );

    EPWM_setADCTriggerSource(
        EPWM1_BASE,
        EPWM_SOC_A,
        EPWM_SOC_TBCTR_ZERO
    );

    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1);

    // 1. Tell the DB module to use the full system clock (200MHz)
    EPWM_setDeadBandCounterConfig(EPWM1_BASE, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);

    // 2. Set Polarity: Make EPWM1B the INVERSE of EPWM1A (Active High Complementary)
    // This is what prevents both being high at the same time.
    EPWM_setDeadBandDelayPolarity(EPWM1_BASE, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
    EPWM_setDeadBandDelayPolarity(EPWM1_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);

    // 2. Set the Delay values (How long is the "Dead" time?)
    // At 200MHz, one clock cycle is 5ns. 200 cycles = 1 microsecond.
    EPWM_setFallingEdgeDeadBandDelayInput(EPWM1_BASE, 200); 
    EPWM_setRisingEdgeDeadBandDelayInput(EPWM1_BASE, 200);

    // 3. Switch the output to use the Dead-Band generator instead of the raw AQ
    EPWM_setDeadBandDelayMode(EPWM1_BASE, EPWM_DB_RED, true);
    EPWM_setDeadBandDelayMode(EPWM1_BASE, EPWM_DB_FED, true);
    }


void initADC(void)
{
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCA_BASE,
                ADC_RESOLUTION_12BIT,
                ADC_MODE_SINGLE_ENDED);

    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);

    ADC_setupSOC(ADCA_BASE,
                 ADC_SOC_NUMBER0,
                 ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN0,
                 15);

    ADC_setInterruptSource(ADCA_BASE,
                           ADC_INT_NUMBER1,
                           ADC_SOC_NUMBER0);

    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
}

__interrupt void adcISR(void)
{
    adcResult = ADC_readResult(ADCARESULT_BASE,
                               ADC_SOC_NUMBER0);

    uint16_t duty = (adcResult * 6000) / 4095;

    EPWM_setCounterCompareValue(
        EPWM1_BASE,
        EPWM_COUNTER_COMPARE_A,
        duty
    );

    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void dmaCh1ISR(void)
{
    // The buffer is now full of 100 fresh samples!
    // Let's use the most recent one to update our PWM duty
    adcResult = adcDataBuffer[RESULTS_BUFFER_SIZE - 1];

    uint16_t duty = (adcResult * 6000) / 4095;
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, duty);

    // Acknowledge DMA interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7); 
}

void initDMA(void)
{
    DMA_initController();

    DMA_configAddresses(DMA_CH1_BASE, (uint16_t *)&adcDataBuffer, 
                        (uint16_t *)(ADCARESULT_BASE + ADC_O_RESULT0));

    DMA_configBurst(DMA_CH1_BASE, 1, 0, 1);
    DMA_configTransfer(DMA_CH1_BASE, RESULTS_BUFFER_SIZE, 0, 1);
    DMA_configWrap(DMA_CH1_BASE, 0, 0, RESULTS_BUFFER_SIZE, 0);

    // Trigger Source: ADC A Interrupt 1
    DMA_configTrigger(DMA_CH1_BASE, DMA_TRIGGER_ADCA1);
    
    // --- NEW STUFF BELOW ---
    // Enable DMA interrupt at the end of the transfer
    DMA_setInterruptMode(DMA_CH1_BASE, DMA_INT_AT_END);
    DMA_enableInterrupt(DMA_CH1_BASE);
    // -----------------------

    DMA_enableTrigger(DMA_CH1_BASE);
    DMA_startChannel(DMA_CH1_BASE);
}

// int main(void)
// {
//     Device_init();
//     // Give DMA access to GS0 RAM (where our buffer is)
//     EALLOW;
//     MemCfgRegs.GSxMSEL.bit.MSEL_GS0 = 1; 
//     EDIS;
//     Device_initGPIO();
//     initDAC();

//     GPIO_setPinConfig(GPIO_0_EPWM1A);
//     GPIO_setPinConfig(GPIO_1_EPWM1B);

//     Interrupt_initModule();
//     Interrupt_initVectorTable();

//     Interrupt_register(INT_ADCA1, &adcISR);
//     Interrupt_enable(INT_ADCA1);

//     initEPWM1();
//     initADC();

//     EINT;
//     ERTM;

//     uint16_t dacVal = 0;

//     while(1)
//     {
//         DAC_setShadowValue(DACA_BASE, dacVal);

//         dacVal += 20;
//         if(dacVal > 4095)
//             dacVal = 0;

//         DEVICE_DELAY_US(2000);
//     }
// }


int main(void)
{
    Device_init();
    Device_initGPIO();
    
   EALLOW;
    // Set GS0 RAM to be mastered by the DMA
    MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GS0, MEMCFG_GSRAMMASTER_CPU1_DMA);
    EDIS;

    initDAC();

    GPIO_setPinConfig(GPIO_0_EPWM1A);
    GPIO_setPinConfig(GPIO_1_EPWM1B);

    Interrupt_initModule();
    Interrupt_initVectorTable();

    // Register DMA ISR instead of ADC ISR
    Interrupt_register(INT_DMA_CH1, &dmaCh1ISR);
    Interrupt_enable(INT_DMA_CH1);

    initEPWM1();
    initADC();
    initDMA(); // Don't forget to call this!

    EINT;
    ERTM;

    uint16_t dacVal = 0;
    while(1)
    {
        // Your while loop is now much "freer" to do complex tasks
        DAC_setShadowValue(DACA_BASE, dacVal);
        dacVal += 20;
        if(dacVal > 4095) dacVal = 0;
        DEVICE_DELAY_US(2000);
    }
}