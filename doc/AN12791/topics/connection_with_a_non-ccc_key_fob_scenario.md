# Connection with a non-CCC Key Fob Scenario

The Car Anchor also supports connecting as a Bluetooth Low Energy central device \(scanner\) to a non-CCC key fob \(advertiser\). To enable scanning on the `*digital\_key\_car\_anchor*` project set the *`gAppScanNonCCC_d`* macro to 1 in the project's `*app\_preinclude.h*` before building.

After flashing the board and running "`factoryreset`", run the "`sd op`" or "`sd pe`" command exactly as in the Owner Pairing or Passive Entry scenarios. The Car Anchor will simultaneously advertise for a CCC Device and scan for a non-CCC key fob.

Connection with the non-CCC key fob and the subsequent application functionality is currently out of this application note's scope.

**Parent topic:**[Running CCC Digital Key scenarios using the Shell Interface](../topics/running_ccc_digital_key_scenarios_using_the_shell_.md)

