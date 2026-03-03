# Square Waveform

Generates a **square wave** signal on the TMS320F28379D's on-chip DAC (DACA) by alternating the output between 0 (logic low) and 4095 (logic high).

## How It Works

1. The DAC is initialised with the system clock as the load trigger and VREFHI as the reference.
2. The main loop toggles a clock variable (`clk`) on each iteration:
   - `clk == 0` → DAC output set to **0** (0 V)
   - `clk == 1` → DAC output set to **4095** (full-scale)
3. A **1 ms** delay between toggles produces a square wave with a period of **2 ms** (500 Hz).

## Peripherals Used

| Peripheral | Role |
|------------|------|
| DACA | Outputs the analog square wave on the DAC pin |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| Low level | 0 (0 V) |
| High level | 4095 (VREFHI) |
| Toggle delay | 1000 µs |
| Output frequency | ~500 Hz |
| DAC resolution | 12-bit (0 – 4095) |
| DAC reference | ADC VREFHI |
