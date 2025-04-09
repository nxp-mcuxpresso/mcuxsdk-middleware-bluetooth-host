#ifndef _BLE_PORT_FSCI_TYPES_
#define _BLE_PORT_FSCI_TYPES_

typedef struct _fsciPortRegisterCmdData_t
{
    uint8_t id;
    uint8_t argc;
    uint8_t aArgs[];
} fsciPortRegisterCmdData_t;

#endif