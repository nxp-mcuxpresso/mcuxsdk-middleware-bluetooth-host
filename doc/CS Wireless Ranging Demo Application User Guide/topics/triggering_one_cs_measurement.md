# Triggering one CS measurement

First, ensure that you have configured the ranging service server and ranging service client boards as described above.

To trigger a measurement, run the following command:

```
range
```

Both devices synchronize to each other and perform a CS test procedure using default settings.

The expected output is a JSON-like structure similar to the following:

```
items:[{mciq:{cfg:{n_ap:1,n_stp:79},result:{vf:79,cde:8.76,cqi:1.000},},tof:{cfg:{n_stp:13},result:{ad:7.8,sr:100},},},]
CRC32:cf49a670
marker:[DONE]
```

If verbosity is increased, the output may contain numerous additional information. It may contain all CS results output as well as debug information.

See section [Command response](command_response.md) for how to interpret these results.

**Parent topic:**[Using the console](../topics/using_the_console.md)

