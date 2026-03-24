# Voltage Sensor

Reads an analog voltage on the TMS320F28379D's on-chip ADC (ADCA, channel 0) and prints the raw ADC count and calculated voltage over the JTAG CIO console using `printf`.

## How It Works

1. **ADC initialization** – ADCA is configured in **12-bit single-ended** mode with a clock prescaler of `/4` and software-only (manual) SOC triggering.
2. **SOC configuration** – SOC0 is set up on ADCIN0 with a 100-cycle acquisition window.
3. **Control loop** (500 ms period):
   1. Trigger SOC0 (`ADC_forceSOC`).
   2. Poll the ADC interrupt flag until the conversion is complete.
   3. Read the result from `ADCARESULT_BASE`.
   4. Convert to voltage:
      ```
      voltage = (adcResult × 3.0) / 4096
      ```
   5. Print raw count and voltage over JTAG CIO (`printf`).
   6. Toggle the heartbeat LED (GPIO 31 — blue LED on the LaunchPad).
   7. Wait 500 ms.

## Peripherals Used

| Peripheral | Role |
|------------|------|
| ADCA Ch 0 (ADCIN0) | Samples the analog input voltage |
| GPIO 31 | Heartbeat LED — toggles every 500 ms |

## Key Parameters

| Parameter | Value |
|-----------|-------|
| ADC resolution | 12-bit (0 – 4095) |
| ADC mode | Single-ended |
| ADC reference voltage | 3.0 V |
| Acquisition window | 100 cycles |
| Sample interval | 500 ms |
| Voltage formula | `V = adcResult × 3.0 / 4096` |

## GPIO Pin Mapping

| GPIO | Function |
|------|----------|
| ADCIN0 | Analog voltage input (0 – 3.0 V) |
| GPIO 31 | Blue heartbeat LED |

## Console Output

Connect the LaunchPad via JTAG and open a CIO terminal in Code Composer Studio. You will see a continuous stream of readings, for example:

```
Raw ADC: 1365 | Voltage: 1.00 V
Raw ADC: 2048 | Voltage: 1.50 V
```
