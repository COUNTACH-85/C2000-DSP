# PWM–ADC Interaction

Demonstrates **closed-loop PWM duty-cycle control** on the TMS320F28379D: an analog input voltage sampled by the ADC is used to dynamically adjust the duty cycle of an EPWM output, with DMA transferring ADC results to a buffer in RAM.

## How It Works

1. **DAC** – generates a slowly ramping voltage (0 → 3.3 V sawtooth) on DACA, which is wired externally to the ADC input pin as a test signal.
2. **EPWM1** – configured in up-count mode with a period of 6000 counts (~33 kHz at 200 MHz).  
   - EPWM1A and EPWM1B carry complementary outputs with a **1 µs dead-band**.  
   - An ADC Start-of-Conversion (SOC) trigger fires at counter zero each PWM cycle.
3. **ADCA** – channel 0 is sampled on every EPWM1 SOC trigger. Interrupt 1 fires after each conversion.
4. **DMA Channel 1** – triggered by ADCA Interrupt 1; transfers each ADC result into a 100-element circular buffer (`adcDataBuffer`) in GS0 RAM.
5. **DMA ISR** – when the buffer is full, reads the latest sample and maps it to a new PWM compare value:
   ```
   duty = (adcResult × 6000) / 4095
   ```
   This linearly scales the 12-bit ADC reading (0–4095) to the full PWM period (0–6000 counts).

## Peripherals Used

| Peripheral | Role |
|------------|------|
| DACA | Provides a test analog signal to the ADC input |
| EPWM1A / EPWM1B | Complementary PWM outputs with dead-band |
| ADCA Ch 0 | Samples the analog input, triggered by EPWM SOC |
| DMA Ch 1 | Transfers ADC results to RAM buffer; fires ISR on completion |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| EPWM period | 6000 counts |
| PWM frequency | ~33 kHz (200 MHz / 6000) |
| Dead-band | 200 cycles = 1 µs |
| ADC resolution | 12-bit (0 – 4095) |
| DMA buffer size | 100 samples |
| DAC ramp step | 20 counts / 2 ms |

## GPIO Pin Mapping

| GPIO | Function |
|------|----------|
| GPIO 0 | EPWM1A output |
| GPIO 1 | EPWM1B output (complementary) |
| ADCIN0 | Analog input from DACA (test signal) |
