#include "device/usbd.h"

//--------------------------------------------------------------------+
// IMPLEMENTED FUNCTIONALITIES
//--------------------------------------------------------------------+
uint8_t _cmd_usbdm_set_target();
uint8_t _cmd_usbdm_set_vdd(uint8_t* command_buffer);
uint8_t _cmd_usbdm_get_bdm_status(uint8_t* command_buffer);
uint8_t _cmd_usbdm_get_capabilities(uint8_t* command_buffer);
uint8_t _cmd_usbdm_set_options(uint8_t* command_buffer);

uint8_t _cmd_usbdm_connect();
uint8_t _cmd_usbdm_set_speed(uint8_t* command_buffer);
uint8_t _cmd_usbdm_read_status_reg(uint8_t* command_buffer);

// Processes all commands received o ver USB
uint8_t command_exec(uint8_t* command_buffer);
void set_command_status(uint8_t status);
