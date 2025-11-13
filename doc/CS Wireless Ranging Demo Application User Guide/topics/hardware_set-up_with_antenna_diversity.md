# Hardware set up with antenna diversity

To enable antenna diversity on KW47-EVK boards, antenna diversity boards \(X-FR-ANTDV-IFA or X-FR-ANTDV-MPIFA\) must be plugged to the EVKs using a similar setup as described in [Hardware set up with single antenna configuration](hardware_set-up_with_single_antenna_configuration.md). The X-FR-ANTDV-IFA is a preferred option. Currently, only the X-FR-ANTDV-IFA is being distributed to the customers.

For a detailed setup description, see *Antenna Diversity Board User's Guide* \(document ADBUG\).
**Figure: Antenna diversity board mounted on KW45 EVK board**
![](../images/image_3.svg "Antenna diversity board mounted on KW45 EVK board ")

From antenna diversity boards, you must choose the pair of antennas, either the two SMA connectors or the two printed antennas, to be used.

Another convenient way to enable antenna diversity testing is to use KW47-LOC board which embeds two printed antennas on the same PCB. For a detailed description of this board and how to set it up, refer to *KW47-LOC Board User Manual*.

To configure antenna diversity properly in the firmware, it is required to configure the following:

-   Antenna diversity board type and antennas used \(SMA connectors or printed antennas\)
-   Antenna configuration index
-   Antenna switching time \(T\_SW\)

**Note:** All platforms support only 1x1 or 2x2 antenna configurations, leading to either one or four antenna paths being used.

**Parent topic:**[Wireless ranging demo application setup](../topics/wireless_ranging_demo_application_setup.md)

