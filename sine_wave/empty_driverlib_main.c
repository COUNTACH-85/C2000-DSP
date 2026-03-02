#include "driverlib.h"
#include "device.h"
#include <math.h>

#define PI 3.141592653589793f
#define SINE_POINTS 100

uint16_t sineTable[SINE_POINTS];

void generateSineTable(void)
{
    int i;
    for(i = 0; i < SINE_POINTS; i++)
    {
        float angle = 2.0f * PI * i / SINE_POINTS;
        float s = sinf(angle);   // -1 to +1
        sineTable[i] = (uint16_t)((s + 1.0f) * 2047.5f); // 0–4095
    }
}

void main(void)
{
    Device_init();
    Device_initGPIO();

    // -------- DAC INIT --------
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_DACA);

    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_ADC_VREFHI);
    DAC_setLoadMode(DACA_BASE, DAC_LOAD_SYSCLK);
    DAC_enableOutput(DACA_BASE);
    DEVICE_DELAY_US(10);
    // --------------------------

    generateSineTable();

    uint16_t index = 0;

    while(1)
    {
        DAC_setShadowValue(DACA_BASE, sineTable[index]);

        index++;
        if(index >= SINE_POINTS)
            index = 0;

        DEVICE_DELAY_US(200);
    }
}
