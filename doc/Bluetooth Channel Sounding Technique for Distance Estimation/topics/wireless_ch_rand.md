# Wireless channels randomization

The whole ISM frequency band is used for the CS technique for distance estimation. There is an important aspect about the frequency channel switching. The ISM band is not swept linearly, but there is a procedure to create randomly selected channels. The BLE standard assumes the use of the channel-selection algorithm for two cases:
-   Channel-selection algorithm for mode 0 steps
-   Channel-selection algorithm for non-mode 0 steps

The channel-selection algorithm #3a is used to generate a randomized channel map with uniform distribution for mode 0 CS steps. The number of times that the mode 0 channel list is required to be regenerated in a CS procedure depends on several factors, including the size of the list of included filtered channels, as indicated by the filter-channel bitmap Chesterfield (the number of non-mode 0 steps included in a CS event and a CS procedure along with the runtime construction of CS events whose content may vary from event to event), depending on the CS mode sequencing.

The channel-selection algorithms #3b and #3c support channel selection for each non-mode 0 CS step within a CS procedure. Only one of the two algorithms shall be used within a single CS procedure. The number of times that the non-mode 0 channel list is required to be regenerated in a CS procedure depends on several factors, including the size of the list of included filtered channels, as indicated by the filter channel bitmap CSFilteredChM, the number of times the filtered channel map is repeated in a procedure for non-Mode 0 steps, along with the runtime construction of CS events whose content may vary from event to event (depending on the CS mode sequencing).

Detailed information about the wireless channels' randomization algorithms is in the BLE standard.

**Parent topic:**[The channel sounding standard description](../topics/cs_standard_description.md)