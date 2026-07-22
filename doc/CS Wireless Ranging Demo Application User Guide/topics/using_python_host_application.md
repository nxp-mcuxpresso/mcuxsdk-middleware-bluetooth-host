# Using Python host application

Using Python application to trigger CS measurements is the preferred option. It takes care of automatically interpreting the CS results, passing them into the selected algorithm, and presenting the output in user-friendly formats.

At this stage, we assume that the setup is properly connected and firmware and host application have been properly installed and configured.

**Note:** Be sure that you have configured the ranging service server board as Bluetooth Low Energy peripheral, as described above.

The application is in the form of a Python script located in: `<app_folder>\python\Scripts\record_range_measurement.py`.

To check that everything is installed properly, enter:

```
python record_range_measurement.py --help
```

It should display command usage. If any Python issue shows up, revisit the installation section.


```{include} ../topics/application_parameters.md
:heading-offset: 2
```

```{include} ../topics/performing_cs_measurements_with_only_step_mode_2s.md
:heading-offset: 2
```

```{include} ../topics/performing_distance_estimation_using_mode_2s_as_ma.md
:heading-offset: 2
```

```{include} ../topics/configuring_embedded_algorithms.md
:heading-offset: 2
```

```{include} ../topics/troubleshooting.md
:heading-offset: 2
```

**Parent topic:**[Running wireless ranging demo application](../topics/running_wireless_ranging_demo_application.md)

