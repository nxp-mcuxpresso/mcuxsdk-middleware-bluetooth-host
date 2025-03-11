# Memory considerations

The GATT Dynamic database module internally manages the memory allocation for the database. If the `gMemManagerLightExtendHeapAreaUsage` define is set to `1` in the desired application, the whole available heap is used. In such as case, the user does not have to allocate space for the dynamic database. If this is not done, the user only needs to make sure that the `MinimalHeapSize_c` define is set to a high enough value considering all attributes and attribute values they want to add to the database, as well as other memory requirements the application might have.

Internally, two buffers are used by the dynamic database module: an attribute buffer and a value buffer. The attribute buffer size increases with the addition of each attribute to the database. The value buffer size increases depending on the UUID type and value lengths required by the application. The two buffers start with a minimum size and are reallocated whenever new requests to add entries are received and there is not enough available memory left. If the user removes these entries from the database, the memory reserved for those entries is not freed, but shifted, leaving room for new entries. Thus, an add operation after a remove operation might not necessarily reallocate the buffer if the new entries fit. The two buffers used by the Dynamic database module will not be available to the application until the user releases the database.

**Parent topic:**[Creating a GATT database dynamically](../topics/creating_a_gatt_database_dynamically.md)

