# Narrowband ToF

Radio frequency waves travel at the speed of the light (c), which is a known constant (c ≈ 3×108m/s); 1 m is equivalent to ~3.3 ns. A system measuring the ToF would ideally require a high-speed clock (GHz) to generate the timestamps with 1-ns accuracy, which is not possible with today’s low-cost deep submicron technologies. For mass-market applications, where low power consumption, low system complexity, and a tight cost budget are a constraint, a low-cost MCU with an integrated 2.4-GHz radio can be used. Typically, these devices work with a high-accuracy reference clock (Fref). As an example, using a reference clock of 32 MHz provides raw ToF accuracy of about 10 m, which may be adequate to validate a more precise but spoofable distance measurement. However, 10 m is too coarse and might not be useful as most applications require accuracy that is much finer. A typical target is to have accuracy of less than 1–2 m. To achieve such accuracy, a number of ToF measurements may be taken (approximating a normal distribution) and averaged. When using ToF distance estimation, there is a trade-off between accuracy and measurement time.

A narrowband ToF measurement systems consists of the following:
1. Counter for timestamping that is synchronized to the radio subsystem clock reference (Fref)
2. Precise hardware-based timestamping mechanism
3. Measurement protocol that includes the following steps:
   -   Timestamp collection
   -   Preprocessing
   -   Postprocessing
   -   Measurement report

```{include} ../topics/tof_timing_mechanism.md
:heading-offset: 1
```

```{include} ../topics/tof_multistage.md
:heading-offset: 1
```

**Parent topic:**[RTT estimation fundamentals](../topics/rtt_estimation_fundamentals.md)
