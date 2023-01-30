#include "device/usbd.h"


//--------------------------------------------------------------------+
// IMPLEMENTED FUNCTIONALITIES
//--------------------------------------------------------------------+
uint8_t _cmd_usbdm_get_capabilities(uint8_t* command_buffer);


// Processes all commands received over USB
uint8_t command_exec(uint8_t* command_buffer);
