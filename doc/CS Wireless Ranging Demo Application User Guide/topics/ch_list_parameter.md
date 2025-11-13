# "ch\_list" parameter
The "ch\_list" command parameter defines the CS channel list. It only applies in test mode. See for example, the code below:
```
set [<0...78>,..,<0...78> [<repeat>]] | Set/show channel list
generate <0...78> <0...78> [rand] | Generate channel list from first and last CS channels
```



Two modes can be used:

-  The "set" mode to input a CS channel list that is input to the firmware.

    ```
    parameter ch_list set 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
    ```

    This mode defines a CS sequence of 17 steps on CS channels in the given order.

-   The "generate" mode makes the firmware generate the channel list based on a start and stop frequency.

    ```
    parameter ch_list generate 0 78
    ```

    This mode defines a CS sequence of 79 steps on CS channels 0 to 78 \(linear sweep\). 
    This is the default behavior of the firmware.


There is also a possibility to randomize the generated frequency list.

```
parameter ch_list generate 0 78 rand
```

It defines a CS sequence of 79 steps on CS channels 0 to 78 in a pseudo-randomized order.

**Parent topic:**["parameter" - CS parameters](../topics/parameter_-_cs_parameters.md)

