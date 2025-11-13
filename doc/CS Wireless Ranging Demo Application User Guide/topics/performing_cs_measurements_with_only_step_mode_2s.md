# Performing CS measurements with only step mode 2’s

The following command triggers a series of 5 CS measurements using RTP:

```
pipenv run python record_range_measurement.py --mainmode 2 -n 5
```

The above command uses default CS parameters configured in the firmware.

To change some parameters, `--dutconfig` arguments can be added to the command.

For example to select 2 Mbit/s PHY, use:

```
--dutconfig "param rtt_phy 1"
```

To enable antenna diversity 2x2 with SMA connectors from antenna diversity board, use:

```
--dutconfig "param ant 7" --dutconfig "param ant_type 1" --dutconfig "param timings t_sw 2"
```

See [Embedded firmware command reference](embedded_firmware_command_reference.md) for the exhaustive firmware configuration commands that can be passed through the `--dutconfig` option.

The output of the script shows:

-   Several miscellaneous information, such as serial ports used, configuration, and so on:

```
2024-12-04 10:44:40 [INFO] __main__: Working on win32
2024-12-04 10:44:40 [INFO] ranging_platform_pkg.utils: Configuring RangingPlatform_kw45 as peer on port: COM26 with unique_id: 0x000000000000
2024-12-04 10:44:41 [INFO] ranging_platform_pkg.utils: Configuring RangingPlatform_kw45 as dut on port: COM27 with unique_id: 0x000000000000
2024-12-04 10:44:43 [INFO] ranging_platform_pkg.utils: Starting measurement with 1 parameters and 5 measurements
2024-12-04 10:44:43 [INFO] ranging_platform_pkg.utils: COM27:   central initiator
```

-   A series of measurements returning estimated distance, initiator RSSI, reflector RSSI, and measurement duration:

-   Measurements statistics \(average and standard deviation\):

```
==============Measurement stats ======================
Results for board pair (1, 1):
RTP: avg=0.29, stdev=0.00, min=0.29, max=0.29
RADE: avg=0.29, stdev=0.00, min=0.29, max=0.29
======================================================       
       
```

Depending on the `--target` option selected, different output files are generated in `<app_folder>\python\Scripts\records\ folder`.

**Parent topic:**[Using Python host application](../topics/using_python_host_application.md)

