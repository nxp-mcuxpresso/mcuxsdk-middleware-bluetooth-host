# Selecting the RADE Configuration

The localization examples support four RADE configurations: RADE1, RADE2, RADEX1, and RADEX2. Each configuration is provided as a dedicated Kconfig fragment and is selected at build time using the `-DCONF_FILE=` west build argument.

| Configuration | CONF_FILE value |
|---|---|
| RADE1 | `examples/wireless_examples/bluetooth/config/channel_sounding_rade1.conf` |
| RADE2 (default) | `examples/wireless_examples/bluetooth/config/channel_sounding_rade2.conf` |
| RADEX1 | `examples/wireless_examples/bluetooth/config/channel_sounding_radex1.conf` |
| RADEX2 | `examples/wireless_examples/bluetooth/config/channel_sounding_radex2.conf` |

To build with a specific configuration, pass the corresponding path to `-DCONF_FILE=`. For example, to use RADE1:

```
west build -b <board> -- -DCONF_FILE=examples/wireless_examples/bluetooth/config/channel_sounding_rade1.conf
```

Replace `channel_sounding_rade1.conf` with the desired configuration file name from the table above.

**Notes:**
- RADE1 and RADEX1 configurations do not use LCE acceleration. Running the RADE algorithm without LCE support increases stack size requirements. Ensure the stack size for the task on which the algorithm runs is at least 4000 octets.
- Make sure that conflicting Kconfig options are not duplicated across configuration files. If duplicates exist, remove them before building the project.
