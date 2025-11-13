# "timings" – CS timings configuration

```
-----------commands-------------
intervals <150|80|50> <145|80|40> [<145|80|40> ] | Set/show HADM interval timings T_FCS, T_IP1, T_IP2 (us)
t_pm <20|40> | Set/show HADM T_PM timing (us)
t_sw <2|4|10> | Set/show HADM T_SW timing (us)
-------------end----------------
```
**Table: Wireless ranging CS timings commands**

| Command   | Default                                             | Description                                                                                                                                                           |
| --------- | --------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| intervals | 150, 145, 145 (test mode)<br>50, 40, 40 (connected) | Set T_FCS, T_IP1 and T_IP2 interval in µs, as defined by CS. Allowed values are: 150 µs, 80 µs, and 50 µs for T_FCS and 145 µs, 80 µs, and 40 µs for T_IP1 and T_IP2. |
| t_pm      | 20                                                  | Set T_PM, duration in µs, as defined by CS Allowed values are: 20 µs and 40 µs                                                                                        |
| t_sw      | 0                                                   | When antenna diversity is used, this command controls the antenna switch time which can be set to 2 µs, 4 µs, or 10 µs. Otherwise 0 is used.                          |


**Note:** The 'Timings' input is relevant for test mode only. In connected mode, the BLE controller chooses timings based on CS capability exchanged between the two devices. NXP controller implementation chooses the fastest timings supported by the two devices.

**Parent topic:**["parameter" - CS parameters](../topics/parameter_-_cs_parameters.md)

