#include "driverlib.h"
#include "device.h"

void initDAC(void)
{
    // Enable DAC peripheral clock
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_DACA);

    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_ADC_VREFHI);
    DAC_setLoadMode(DACA_BASE, DAC_LOAD_SYSCLK);
    DAC_enableOutput(DACA_BASE);
    DAC_setShadowValue(DACA_BASE, 0);
    DEVICE_DELAY_US(10);
}


void main(void)
{
    Device_init();
    Device_initGPIO();

    

    initDAC();

    uint16_t dacVal = 0;

    while(1)
    {
        
        DAC_setShadowValue(DACA_BASE, dacVal);

        dacVal++;
        if(dacVal > 4095)
        {
            dacVal=0;
            

        }
                
        DEVICE_DELAY_US(2); 
    }

}