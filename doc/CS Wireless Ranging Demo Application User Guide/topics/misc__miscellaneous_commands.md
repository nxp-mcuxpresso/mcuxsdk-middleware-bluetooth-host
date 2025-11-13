# "misc" – miscellaneous commands

```
-----------commands-------------
xtal_trim [<0...63>] | Set/show XTAL trim value
cw_trim [0|1] | Start/Stop 2440MHz constant wave Tx (used for XTAL trimming)
rtp_algo [<0...7>] | Set/show RTP embedded algorithm bitmap to be run (bit0:CDE, bit2:RADE)
rtp_algo_config [<1(CDE)> [<param1> <param2>]] | Set/show RTP embedded algorithm configuration
```
**Table: Wireless ranging miscellaneous commands**
| Command         | Default | Comment                                                                                                                                                                          |
| --------------- | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| xtal_trim       | 0       | Set/show 32 MHz crystal oscillator trimming value. Combined with cw_trim command, it allows you to adjust Xtal precisely.                                                        |
| cw_trim         | \-      | Turn on/off a 2440 MHz constant wave transmission to tune the Xtal (using xtal_trim)                                                                                             |
| rtp_algo        | 1       | Set RTP embedded algorithm to be run in bitmap format: 0: No embedded algorithm is run<br>bit 0: The firmware runs the CDE algorithm bit 2: The firmware runs the RADE algorithm |
| rtp_algo_config | \-      | This command can be used to customize some parameters of CDE algorithm. It is for internal usage only.                                                                           ||

**Parent topic:**[Main menu](../topics/main_menu.md)

