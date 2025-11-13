# Performing distance estimation using mode 2’s as main mode and mode 1 as submode

The following command triggers a series of 5 CS measurements using RTP/RTT:

```
pipenv run python record_range_measurement.py --mainmode 2 --submode 1 -n5
```

Expected output is as follows where SRDE and RTT distance are computed:

```
2021-11-18 09:49:05 [INFO] record_range_measurement: ===========================================================
2021-11-18 09:49:05 [INFO] ranging_platform_pkg.utils: - parameter0 - meas_nr has 5 values
2021-11-18 09:49:05 [INFO] ranging_platform_pkg.utils: Iteration -    0/5    @(meas_nr = 00) => RTP/RTT-distance = 12.47m/13.56m (100%), RSSI = -33dBm/-47dBm, Duration = 335ms
```

**Parent topic:**[Using Python host application](../topics/using_python_host_application.md)

