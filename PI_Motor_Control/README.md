# PI Motor Speed Controller

Implements a **closed-loop PI (Proportional–Integral) motor speed controller** on the TMS320F28379D. An ADC-connected potentiometer (or simulated ramp) sets the target RPM, a quadrature encoder measures the actual speed, and an ePWM output drives the motor to track the setpoint.

## How It Works

1. **HAL (Hardware Abstraction Layer)** – `hal.c`/`hal.h` wraps all peripheral access so the same application code runs in two modes:
   - `HAL_SIM_MODE 1` – fully simulated (no hardware required); the encoder count is modelled from the applied PWM duty cycle and the setpoint ramps automatically 0 → 3000 → 0 RPM.
   - `HAL_SIM_MODE 0` – real hardware; uses EPWM1, ADCA, and an eQEP encoder input.

2. **PI Controller** – `pi_controller.c`/`pi_controller.h` implements a portable, hardware-independent PI algorithm with anti-windup integral clamping.

3. **Control Loop** (100 ms period in `main.c`):
   1. Wait 100 ms (`DEVICE_DELAY_US`).
   2. Snapshot encoder pulses and convert to RPM:
      ```
      measuredRPM = (pulses / ENCODER_PPR) × 600
      ```
   3. Read ADC potentiometer and scale to RPM setpoint:
      ```
      setpointRPM = (adcRaw / 4095) × MAX_RPM_SETPOINT
      ```
   4. Run PI update — returns PWM counts `[0, PWM_PERIOD_COUNTS]`.
   5. Write new duty cycle to ePWM1 CMPA register.

## Peripherals Used

| Peripheral | Role |
|------------|------|
| EPWM1A | PWM output driving the motor (20 kHz) |
| ADCA Ch 0 | Reads potentiometer for speed setpoint (0–4095) |
| eQEP / GPIO28 | Quadrature encoder input for speed measurement |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| PWM frequency | 20 kHz (PWM_PERIOD_COUNTS = 10 000 @ 200 MHz) |
| Control period | 100 ms |
| Encoder PPR | 374 pulses/revolution (configurable in `hal.h`) |
| Max RPM setpoint | 3000 RPM (configurable in `hal.h`) |
| Default Kp | 1.0 |
| Default Ki | 0.05 |

## GPIO Pin Mapping

| GPIO | Function |
|------|----------|
| GPIO 0 | EPWM1A output — PWM duty tracks PI output |
| GPIO 28 | Encoder input (eQEP) |

## Simulation Mode

Set `HAL_SIM_MODE 1` in `hal.h` to run without any motor or encoder connected:
- The setpoint ramps automatically 0 → 3000 → 0 RPM.
- `HAL_setPWMDuty()` updates ePWM CMPA, so you can probe **GPIO 0** on an oscilloscope to observe the duty cycle tracking the simulated setpoint.
