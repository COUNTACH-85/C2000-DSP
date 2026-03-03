# Sawtooth Waveform

Generates a **sawtooth wave** signal on the TMS320F28379D's on-chip DAC (DACA) by continuously incrementing the DAC output value from 0 to 4095 and then wrapping back to 0.

## How It Works

1. The DAC is initialised with the system clock as the load trigger and VREFHI as the reference.
2. The main loop increments `dacVal` by 1 on each step and writes it to the DAC shadow register.
3. When `dacVal` exceeds **4095** it wraps back to **0**, creating the characteristic sawtooth shape.
4. A **2 µs** delay between steps produces a full sawtooth cycle in **4096 × 2 µs ≈ 8.2 ms** (~122 Hz).

## Peripherals Used

| Peripheral | Role |
|------------|------|
| DACA | Outputs the analog sawtooth wave on the DAC pin |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| Step size | 1 DAC count |
| Step delay | 2 µs |
| Output frequency | ~122 Hz |
| DAC resolution | 12-bit (0 – 4095) |
| DAC reference | ADC VREFHI |
