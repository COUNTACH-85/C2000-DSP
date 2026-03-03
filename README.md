# C2000-DSP

A collection of DSP projects for the **Texas Instruments TMS320F28379D** (C2000 Delfino) microcontroller, developed using **Code Composer Studio (CCS)** and the **TI DriverLib** peripheral driver library.

## Projects

| Folder | Description |
|--------|-------------|
| [`sine_wave`](./sine_wave) | Generates a sine wave output via the on-chip DAC using a pre-computed lookup table. |
| [`Square_waveform`](./Square_waveform) | Generates a square wave output via the on-chip DAC by toggling between 0 and full-scale values. |
| [`Triangular_waveform`](./Triangular_waveform) | Generates a triangular wave output via the on-chip DAC by linearly ramping up and down. |
| [`Sawtooth`](./Sawtooth) | Generates a sawtooth wave output via the on-chip DAC by incrementing the DAC value and wrapping around. |
| [`Pwm_adc_interaction`](./Pwm_adc_interaction) | Demonstrates closed-loop PWM duty-cycle control driven by ADC readings, using EPWM1 to trigger ADC conversions and DMA to transfer samples. |

## Hardware

- **MCU**: TMS320F28379D (dual-core C28x, 200 MHz)
- **DAC output**: DACA (all waveform projects)
- **PWM output**: EPWM1A / EPWM1B with dead-band (PWM–ADC project)
- **ADC input**: ADCA Channel 0 (PWM–ADC project)

## Getting Started

1. Install [Code Composer Studio](https://www.ti.com/tool/CCSTUDIO) and the C2000Ware SDK.
2. Import the desired project folder into CCS as an existing CCS project.
3. Connect the TMS320F28379D LaunchPad (or custom board) via USB/JTAG.
4. Build and flash the project using CCS.

## Project Structure

Each sub-folder is a self-contained CCS project and contains:

```
<project>/
├── empty_driverlib_main.c   # Main application source
├── c2000.syscfg             # SysConfig peripheral configuration
├── 2837xD_RAM_lnk_cpu1.cmd  # Linker command file (RAM build)
├── 2837xD_FLASH_lnk_cpu1.cmd# Linker command file (Flash build)
├── device/                  # Device-specific support files
├── targetConfigs/           # CCS target configuration
└── results/                 # Captured oscilloscope/DAC output screenshots
```
