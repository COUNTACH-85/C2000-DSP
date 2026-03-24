#include "driverlib.h"
#include "device.h"
#include <stdio.h>  // Added back for printf!

// Define globally so the debugger can easily see them
volatile uint16_t adcResult = 0;
volatile float voltage = 0.0f;

void main(void) {
    // 1. Initialize System
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    // 2. Setup Heartbeat LED (Blue LED on GPIO 31)
    GPIO_setPadConfig(31, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(31, GPIO_DIR_MODE_OUT);

    // 3. Setup ADC A
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000); 

    // 4. Setup SOC0 (with the fixed 100-cycle sample window)
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 100);

    // 5. Route the hardware interrupts
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    EINT;
    ERTM;

    // 6. The Control Loop
    while(1) {
        // Trigger the ADC
        ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);

        // Wait for conversion to finish 
        while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

        // Read the result (using the fixed ADCARESULT_BASE)
        adcResult = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
        voltage = ((float)adcResult * 3.0f) / 4096.0f;

        // --- THE TEXT OUTPUT ---
        printf("Raw ADC: %d | Voltage: %.2f V\n", adcResult, voltage);
        fflush(stdout); // Force the text out of the buffer immediately

        // Toggle the heartbeat LED
        GPIO_togglePin(31); 

        // Delay 500ms (Half a second so you can actually read the screen)
        DEVICE_DELAY_US(500000); 
    }
}



// #include "driverlib.h"
// #include "device.h"
// #include <stdio.h>  // Added back for printf!

// // Define globally so the debugger can easily see them
// volatile uint16_t adcResult = 0;
// volatile float voltage = 0.0f;

// void main(void) {
//     // 1. Initialize System
//     Device_init();
//     Device_initGPIO();
//     Interrupt_initModule();
//     Interrupt_initVectorTable();

//     // 2. Setup Heartbeat LED (Blue LED on GPIO 31)
//     GPIO_setPadConfig(31, GPIO_PIN_TYPE_STD);
//     GPIO_setDirectionMode(31, GPIO_DIR_MODE_OUT);

//     // 3. Setup ADC A
//     ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
//     ADC_setMode(ADCA_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
//     ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
//     ADC_enableConverter(ADCA_BASE);
//     DEVICE_DELAY_US(1000); 

//     // 4. Setup SOC0 (with the fixed 100-cycle sample window)
//     ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 100);

//     // 5. Route the hardware interrupts
//     ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
//     ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
//     ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

//     EINT;
//     ERTM;

//     // 6. The Control Loop
//     while(1) {
//         // Trigger the ADC
//         ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);

//         // Wait for conversion to finish 
//         while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false);
//         ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

//         // Read the result (using the fixed ADCARESULT_BASE)
//         adcResult = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
//         voltage = ((float)adcResult * 3.3f) / 32768.0f;

//         // --- THE TEXT OUTPUT ---
//         printf("Raw ADC: %d | Voltage: %.2f V\n", adcResult, voltage);
//         fflush(stdout); // Force the text out of the buffer immediately

//         // Toggle the heartbeat LED
//         GPIO_togglePin(31); 

//         // Delay 500ms (Half a second so you can actually read the screen)
//         DEVICE_DELAY_US(500000); 
//     }
// }


// #include "driverlib.h"
// #include "device.h"
// #include <stdio.h>

// // Define globally so the debugger can easily see them
// volatile uint16_t adcResult = 0;
// volatile int16_t signedResult = 0;
// volatile float voltage = 0.0f;

// void main(void) {
//     // 1. Initialize System
//     Device_init();
//     Device_initGPIO();
//     Interrupt_initModule();
//     Interrupt_initVectorTable();

//     // 2. Setup Heartbeat LED (Blue LED on GPIO 31)
//     GPIO_setPadConfig(31, GPIO_PIN_TYPE_STD);
//     GPIO_setDirectionMode(31, GPIO_DIR_MODE_OUT);

//     // 3. Setup ADC A — 16-bit differential mode
//     ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
//     ADC_setMode(ADCA_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
//     ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
//     ADC_enableConverter(ADCA_BASE);
//     DEVICE_DELAY_US(1000);

//     // 4. Setup SOC0 — ADCIN0(+) and ADCIN1(-) differential pair
//     ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
//                  ADC_CH_ADCIN0, 100);

//     // 5. Route the hardware interrupts
//     ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
//     ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
//     ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

//     EINT;
//     ERTM;

//     // 6. The Control Loop
//     while(1) {
//         // Trigger the ADC
//         ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);

//         // Wait for conversion to finish
//         while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false);
//         ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

//         // FIX 1: Read raw unsigned result correctly
//         adcResult = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

//         // FIX 2: Convert offset binary to signed
//         // In 16-bit differential, midpoint is 32768 = 0V
//         // Above 32768 = positive voltage, below 32768 = negative voltage
//         signedResult = (int16_t)((int32_t)adcResult - 32768);

//         // FIX 3: Correct voltage formula for differential mode
//         // Full scale = +3.3V at 32767, -3.3V at -32768
//         voltage = ((float)signedResult * 3.3f) / 32768.0f;

//         // FIX 4: Use %u for unsigned raw value, %d for signed
//         printf("Raw ADC: %u | Signed: %d | Voltage: %.4f V\n",
//                 adcResult, signedResult, voltage);
//         fflush(stdout);

//         // Toggle the heartbeat LED
//         GPIO_togglePin(31);

//         // Delay 500ms
//         DEVICE_DELAY_US(500000);
//     }
// }
// ```

// ---

// ## What Was Wrong and What Was Fixed

// | # | Problem | Old Code | Fixed Code |
// |---|---|---|---|
// | 1 | Wrong voltage formula | `adcResult * 3.3 / 32768` | `signedResult * 3.3 / 32768` |
// | 2 | No signed conversion | Missing entirely | `(int16_t)((int32_t)adcResult - 32768)` |
// | 3 | Wrong printf format | `%d` for uint16_t | `%u` for raw, `%d` for signed |
// | 4 | Low voltage precision | `%.2f` (2 decimal places) | `%.4f` (4 decimal places — needed for mV readings) |

// ---

// ## What You Should Now See

// With a clean 10mV input:
// ```
// Raw ADC: 32867 | Signed: 99 | Voltage: 0.0100 V  ✅

