# PDE implementation

A two-way PDE estimation algorithm typically requires precise acquisition of phase changes across a span of frequencies by each participating device. The phases are derived from the captured I and Q samples in the receiver of a device for a tone transmitted by the peer device with a known raster.

The PDE implementation mainly consists of four steps: initialization and calibration, IQ data capture, phase processing, and distance estimation.

```{include} ../topics/initialization_calibration.md
:heading-offset: 1
```

```{include} ../topics/iq_data_capture.md
:heading-offset: 1
```

```{include} ../topics/phase_processing.md
:heading-offset: 1
```

```{include} ../topics/distance_estimation.md
:heading-offset: 1
```

**Parent topic:**[RTP estimation fundamentals](../topics/rtp_estimation_fundamentals.md)
