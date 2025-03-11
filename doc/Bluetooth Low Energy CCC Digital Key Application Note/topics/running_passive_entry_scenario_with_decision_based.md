# Running Passive Entry Scenario with Decision Based Advertising Filtering \(DBAF\)

Decision Based Advertising Filtering can be used in the Passive Entry scenario. Using DBAF optimizes the scanning operation performed by the Digital Key Device of the advertisements sent by the Digital Key Car Anchor devices. This is accomplished by using '`ADV_DECISION_IND`' PDUs, which contain the advertising address in the extended header. Using these PDUs allows the scanner to perform address resolution without the need to retrieve the AUX\_ADV\_IND PDU sent on the data channels.

The DBAF feature is supported on the following platforms:

-   KW45B41Z-EVK
-   KW47-EVK


```{include} ../topics/enabling_dbaf.md
:heading-offset: 1
```

```{include} ../topics/running_the_passive_entry_with_dbaf_scenario.md
:heading-offset: 1
```

