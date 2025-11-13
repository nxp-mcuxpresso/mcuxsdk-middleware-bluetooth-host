# Configuring embedded algorithms

The following command triggers CS measurements using the embedded CDE + RADE algorithms and RTP/RTT with minimal verbosity output:

```
pipenv run python record_range_measurement.py --mainmode 2 --submode 1 --algo 0,5
```

```
2024-12-04 11:03:51 [INFO] __main__: Working on win32
2024-12-04 11:03:51 [INFO] ranging_platform_pkg.utils: Configuring RangingPlatform_kw45 as peer on port: COM26 with unique_id: 0x000000000000
2024-12-04 11:03:52 [INFO] ranging_platform_pkg.utils: Configuring RangingPlatform_kw45 as dut on port: COM27 with unique_id: 0x000000000000
2024-12-04 11:03:54 [INFO] ranging_platform_pkg.utils: Starting measurement with 1 parameters and 16 measurements
2024-12-04 11:03:54 [INFO] ranging_platform_pkg.utils: COM27:   central initiator
2024-12-04 11:03:54 [INFO] ranging_platform_pkg.utils: - parameter0 - meas_nr has 16 values
2024-12-04 11:03:55 [INFO] ranging_platform_pkg.utils: Iteration -    0/16   @(meas_nr = 00) => CDE-distance =  0.71m RADE-distance =  0.87m RTT-distance =  0.70m (100%), Duration = 493ms
2024-12-04 11:03:55 [INFO] ranging_platform_pkg.utils: Iteration -    1/16   @(meas_nr = 01) => CDE-distance =  0.71m RADE-distance =  0.87m RTT-distance =  1.30m (100%), Duration = 484ms
2024-12-04 11:03:56 [INFO] ranging_platform_pkg.utils: Iteration -    2/16   @(meas_nr = 02) => CDE-distance =  0.70m RADE-distance =  0.87m RTT-distance =  0.30m (100%), Duration = 483ms
2024-12-04 11:03:56 [INFO] ranging_platform_pkg.utils: Iteration -    3/16   @(meas_nr = 03) => CDE-distance =  0.70m RADE-distance =  0.87m RTT-distance =  0.10m (100%), Duration = 480ms
2024-12-04 11:03:57 [INFO] ranging_platform_pkg.utils: Iteration -    4/16   @(meas_nr = 04) => CDE-distance =  0.70m RADE-distance =  0.87m RTT-distance = -0.60m ( 92%), Duration = 450ms
2024-12-04 11:03:57 [INFO] ranging_platform_pkg.utils: Iteration -    5/16   @(meas_nr = 05) => CDE-distance =  0.70m RADE-distance =  0.87m RTT-distance = -0.30m (100%), Duration = 479ms
2024-12-04 11:03:58 [INFO] ranging_platform_pkg.utils: Iteration -    6/16   @(meas_nr = 06) => CDE-distance =  0.71m RADE-distance =  0.87m RTT-distance =  0.60m (100%), Duration = 451ms
2024-12-04 11:03:58 [INFO] ranging_platform_pkg.utils: Iteration -    7/16   @(meas_nr = 07) => CDE-distance =  0.71m RADE-distance =  0.87m RTT-distance =  0.70m (100%), Duration = 451ms
2024-12-04 11:03:59 [INFO] ranging_platform_pkg.utils: Iteration -    8/16   @(meas_nr = 08) => CDE-distance =  0.72m RADE-distance =  0.87m RTT-distance =  0.70m (100%), Duration = 479ms
2024-12-04 11:03:59 [INFO] ranging_platform_pkg.utils: Iteration -    9/16   @(meas_nr = 09) => CDE-distance =  0.71m RADE-distance =  0.87m RTT-distance = -0.40m (100%), Duration = 480ms
2024-12-04 11:04:00 [INFO] ranging_platform_pkg.utils: Iteration -   10/16   @(meas_nr = 10) => CDE-distance =  0.69m RADE-distance =  0.87m RTT-distance =  0.70m (100%), Duration = 479ms
2024-12-04 11:04:00 [INFO] ranging_platform_pkg.utils: Iteration -   11/16   @(meas_nr = 11) => CDE-distance =  0.72m RADE-distance =  0.87m RTT-distance = -1.00m (100%), Duration = 451ms
2024-12-04 11:04:01 [INFO] ranging_platform_pkg.utils: Iteration -   12/16   @(meas_nr = 12) => CDE-distance =  0.72m RADE-distance =  0.87m RTT-distance = -0.60m (100%), Duration = 450ms
2024-12-04 11:04:01 [INFO] ranging_platform_pkg.utils: Iteration -   13/16   @(meas_nr = 13) => CDE-distance =  0.72m RADE-distance =  0.87m RTT-distance = -0.00m (100%), Duration = 449ms
2024-12-04 11:04:01 [INFO] ranging_platform_pkg.utils: Iteration -   14/16   @(meas_nr = 14) => CDE-distance =  0.72m RADE-distance =  0.87m RTT-distance = -0.10m (100%), Duration = 480ms
2024-12-04 11:04:02 [INFO] ranging_platform_pkg.utils: Iteration -   15/16   @(meas_nr = 15) => CDE-distance =  0.72m RADE-distance =  0.87m RTT-distance =  1.00m (100%), Duration = 481ms
```

**Parent topic:**[Using Python host application](../topics/using_python_host_application.md)

