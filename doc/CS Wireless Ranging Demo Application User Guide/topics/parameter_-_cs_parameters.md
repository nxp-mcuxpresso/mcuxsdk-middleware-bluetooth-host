# "parameter" - CS parameters

The *CS parameters menu* lists and allows modification of CS configuration. The options displayed here are explained in the table below.

```
-----------commands-------------
ch_list | Channel list commands (test mode only)
ch_map [<bitmask> [<repeat>]] | Set/show channel map as 10 hexa octets (79 bits used)
cs_algo [<0|1> [<0|1>] [<2...8>]] | Set/show channel selection algo type (0=3b, 1=3c), and for #3c: shape (0=hat, 1=X) and jump
main_mode_nb [<1...160> <1...160> <0...3>] | Set/show numbers of HADM main mode steps (min, max, repetition)
mode0_nb [<1...3>] | Set/show number of HADM mode0 steps
mode_type [<1...3> [<1...3>]] | Set/show main mode and sub-mode types
role [<initiator,reflector>] | Set/show HADM role
rtt_phy [<0|1>] | Set/show RTT phy rate (0=1Mbps, 1=2Mbps)
tx_pwr [<-12...MAX>] | Set/show TX power in dBm of CS measurement. Range is -12dBm to max chip capability
debug [<0-255>] | Set/show debug options (REPORT_DBG_INFO<2>, IQ_AVERAGE_DISABLE<1>, IQ_DETAILS<0>)
ant_cfg [<0|7> [<0...23|255>]] | Set/show antenna config index and (test mode only) permutation index
ant_type [<0-5> ] | Set/show antenna board type: 0:none, 1:X-FR-ANTDIV SMA, 2:X-FR-ANTDIV printed, 3:LOC SMA, 4:LOC printed, 5:LOC printed SMA1 dummy
pn_seq [<AA initiator> [<AA reflector>]] | Set/show PN sequences (must be a hex number, test mode only)
tone_ext [<0...4>] | Set/show TP tone extension (test mode only)
timings | Timings commands
-------------end----------------
```

**Table: Wireless ranging CS parameters commands**
| Command      | Default   | Description                                                                                                                                                                            |
| ------------ | --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ch_list      | \-        | Used to define the CS channel list in test mode                                                                                                                                        |
| ch_map       | All ones  | Used to define the CS channel map in connected mode<br>Use "param ch_map 0x1FFFFFFFFFFFFC7FFFFC" to mask advertising channels                                                          |
| cs_algo      | 0         | Used to define channel selection algorithm type (0=3b, 1=3c), and for #3c: shape (0=hat, 1=X) and jump                                                                                 |
| main_mode_nb | 4, 4, 0   | Numbers of CS main mode steps (min, max, repetition)                                                                                                                                   |
| mode0_nb     | 2         | Set/show numbers of CS mode 0 steps                                                                                                                                                    |
| mode_type    | 2,1       | Set/show main mode and submode types as defined by CS<br>**Note that the step mode 3 is only supported on KW47 family and FRDM-KW43 platforms.**                                                         |
| role         | Initiator | Set/show CS role                                                                                                                                                                       |
| rtt_phy      | 0         | Set/show RTT PHY rate (0=1 Mbit/s, 1=2 Mbit/s)                                                                                                                                         |
| tx_pwr       | 0         | TX power in dBm used during the CS measurement;<br>[-12…10] dBm                                                                                                                           |
| pn_seq       | \-        | Set/show the PN sequences to be used in test mode                                                                                                                                      |
| tone_ext     | \-        | Set/show the tone extension behavior in test mode                                                                                                                                      |
| timings      | \-        | Timings submenu (see below); Only for test mode                                                                                                                                        |
| debug        | 0         | For internal debug purpose                                                                                                                                                             |
| ant_cfg      | 0, 0      | Antenna configuration index as defined in CS specs<br> (0:1x1, 7:2x2) followed by  a permutation index <br>(0-23 for fixed permutation index and <br>255 for rolling over all permutation indexes) |
| ant_type     | 0         | Antenna diversity board to be used   <br> (0:None,<br>   1: X-FR-ANTDIV SMA connectors, <br>2: X-FR-ANTDIV printed antennas, <br>3: LOC SMA, <br>4: LOC printed, <br>5: LOC printed + SMA1 for dummy antenna)         |

```{include} ../topics/ch_list_parameter.md
:heading-offset: 3
```

```{include} ../topics/timings__cs_timings_configuration.md
:heading-offset: 3
```

**Parent topic:**[Main menu](../topics/main_menu.md)

