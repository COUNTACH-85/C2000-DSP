# Sine Wave

Generates a **sine wave** signal on the TMS320F28379D's on-chip DAC (DACA) using a pre-computed lookup table.

## How It Works

1. At startup, `generateSineTable()` fills a 100-element array with 12-bit DAC values representing one full cycle of a sine wave.  
   Each value is computed as:
   ```
   sineTable[i] = (sin(2π·i/100) + 1) × 2047.5
   ```
   This maps the −1 … +1 sine range to 0 … 4095 (12-bit full-scale).

2. The main loop steps through the table one entry at a time, writing each value to the DAC shadow register, and waits **200 µs** between steps.  
   This produces a sine wave with a period of **100 × 200 µs = 20 ms** (50 Hz).

## Peripherals Used

| Peripheral | Role |
|------------|------|
| DACA | Outputs the analog sine wave on the DAC pin |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| Lookup table size | 100 points |
| Step delay | 200 µs |
| Output frequency | ~50 Hz |
| DAC resolution | 12-bit (0 – 4095) |
| DAC reference | ADC VREFHI |
