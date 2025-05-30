# Memory considerations

The GATT Dynamic database module internally manages the memory allocation for the database. There are two ways to specify the memory configuration. The selection is done by setting `gGattDynamicAttrSize_c` and/or `gGattDynamicValSize_c`.

-   *Statically pre-allocated memory area*
    -   `gGattDynamicAttrSize_c` - the total size of the table of characteristics
    -   `gGattDynamicValSize_c` - the total size of the values cumulated from all characteristics.
 If any of these macros is greater than 1 the application is responsible to decide upon the best value. In case the selected value is not big enough gBleOutOfMemory_c is returned by the GATT API calls.
-   *Dynamic allocated memory area*
If the `gMemManagerLightExtendHeapAreaUsage` define is set to `1` in the desired application, the whole available heap is used. In such as case, the user does not have to allocate space for the dynamic database. If this is not done, the user only needs to make sure that the `MinimalHeapSize_c` define is set to a high enough value considering all attributes and attribute values they want to add to the database, as well as other memory requirements the application might have.
This method is used in case only several attributes are added dynamically. If multiple attributes are needed the method from `*Statically pre-allocated memory area*` above is recommended. This is because the heap gets fragmented with each adding of a new attribute.

Internally, two buffers are used by the dynamic database module: an attribute buffer and a value buffer. The attribute buffer size increases with the addition of each attribute to the database. The value buffer size increases depending on the UUID type and value lengths required by the application. The two buffers start with a minimum size and are reallocated whenever new requests to add entries are received and there is not enough available memory left. If the user removes these entries from the database, the memory reserved for those entries is not freed, but shifted, leaving room for new entries. Thus, an add operation after a remove operation might not necessarily reallocate the buffer if the new entries fit. The two buffers used by the Dynamic database module will not be available to the application until the user releases the database.

**Parent topic:**[Creating a GATT database dynamically](../topics/creating_a_gatt_database_dynamically.md)

