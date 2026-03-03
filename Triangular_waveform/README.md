# Triangular Waveform

Generates a **triangular wave** signal on the TMS320F28379D's on-chip DAC (DACA) by linearly incrementing and decrementing the DAC output value.

## How It Works

1. The DAC is initialised with the system clock as the load trigger and VREFHI as the reference.
2. A direction flag `v` controls whether the DAC value is ramping up or down:
   - `v == 0` → increment `dacVal` by 1 each step (ramp up)
   - `v == 1` → decrement `dacVal` by 1 each step (ramp down)
3. When `dacVal` reaches **4095** the direction reverses (ramp down); when it reaches **0** the direction reverses again (ramp up).
4. A **2 µs** delay between steps produces a full triangle cycle in **4095 × 2 × 2 µs ≈ 16.4 ms** (~61 Hz).

## Peripherals Used

| Peripheral | Role |
|------------|------|
| DACA | Outputs the analog triangular wave on the DAC pin |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| Step size | 1 DAC count |
| Step delay | 2 µs |
| Output frequency | ~61 Hz |
| DAC resolution | 12-bit (0 – 4095) |
| DAC reference | ADC VREFHI |
