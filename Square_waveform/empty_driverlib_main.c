
#include "driverlib.h"
#include "device.h"


void DACinit(void){
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_DACA);
    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_ADC_VREFHI);
    DAC_setLoadMode(DACA_BASE, DAC_LOAD_SYSCLK);
    DAC_enableOutput(DACA_BASE);
    DAC_setShadowValue(DACA_BASE, 0);
    DEVICE_DELAY_US(10);
}
void main(void)
{

    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull-ups.
    //
    Device_initGPIO();
   DACinit();

    EINT;
    ERTM;
    uint16_t dacVal = 0;
    int clk = 0;
    while(1)
    {   
        DAC_setShadowValue(DACA_BASE, dacVal);
        if(clk == 0){
            dacVal = 0;
            clk =1;
        }
        else{
            dacVal = 4095;
            clk =0;
        }
        DEVICE_DELAY_US(1000);
    }
}

