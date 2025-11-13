# Application parameters

The `--help` option provides most of the explanation regarding command-line options.

See additional information below:

-   The application attempts to detect boards automatically. However, it is possible to force which COM port to connect to using `--comport` option.
-   By default, the application attempts to detect two boards: one acts as a client, and another one acts as the server. Using `-R` option allows you to run the application with a single board \(client\) connected to the PC. Once the client is configured, it can be moved freely without being connected to the PC, as shown in the [figure](../images/Fig.1.svg).
-   The client always acts as the Bluetooth Low Energy central role. Additionally, by default, the client acts as the CS initiator \(CS role can be changed using `--role` option\).
-   The application prints results on the screen. However, using `--target` option \(can be specified multiple times\), it is also possible to generate output files in the following formats:
    -   Text file \(\*.txt\) that contains only the raw data exchanged between firmware and Python script.
    -   Numpy format \(\*.npz\) that can be loaded using Python numpy library. It contains both raw data and results of distance estimations.
    -   Matlab format \(\*.mat\) that can be loaded directly into Matlab. It contains both raw data and results of distance estimations.
    -   CSV file \(\*.csv\) that contains distance estimation results dumped for each measurement.

**Parent topic:**[Using Python host application](../topics/using_python_host_application.md)

