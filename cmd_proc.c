#include "cmd_proc.h"

#include "config.h"

//--------------------------------------------------------------------+
// COMMANDS CODE
//--------------------------------------------------------------------+

   // ---------- Common to all targets-------------
   // 0:  CMD_USBDM_GET_COMMAND_STATUS
   // 1:  CMD_USBDM_SET_TARGET
   // 2:  CMD_USBDM_SET_VDD
   // 3:  CMD_USBDM_DEBUG
   // 4:  CMD_USBDM_GET_BDM_STATUS
   // 5:  CMD_USBDM_GET_CAPABILITIES
   // 6:  CMD_USBDM_SET_OPTIONS
   // 7(Illegal):  RESERVED
   // 8:  CMD_USBDM_CONTROL_PINS
   // 9(Illegal):  RESERVED
   // 10(Illegal):  RESERVED
   // 11(Illegal):  RESERVED
   // 12(Illegal):  CMD_USBDM_GET_VER (EP0)
   // 13(Illegal):  RESERVED
   // 14(Illegal):  CMD_USBDM_ICP_BOOT (EP0)

// ---------- Target specific versions(HCS08/RS08) -------------
   // 15:  CMD_USBDM_CONNECT
   // 16:  CMD_USBDM_SET_SPEED
   // 17:  CMD_USBDM_GET_SPEED

   // 18(Illegal):  CMD_CUSTOM_COMMAND
   // 19(Illegal):  RESERVED

   // 20:  CMD_USBDM_READ_STATUS_REG
   // 21:  CMD_USBDM_WRITE_CONTROL_REG

   // 22:  CMD_USBDM_TARGET_RESET
   // 23:  CMD_USBDM_TARGET_STEP
   // 24  CMD_USBDM_TARGET_GO
   // 25:  CMD_USBDM_TARGET_HALT

   // 26:  CMD_USBDM_WRITE_REG
   // 27:  CMD_USBDM_READ_REG

   // 28:  CMD_USBDM_WRITE_CREG
   // 29:  CMD_USBDM_READ_CREG

   // 30:  CMD_USBDM_WRITE_DREG
   // 31:  CMD_USBDM_READ_DREG

   // 32:  CMD_USBDM_WRITE_MEM
   // 33:  CMD_USBDM_READ_MEM
//--------------------------------------------------------------------+

/*
 *   Processes all commands received over USB
 *
 *   The command is expected to be in \ref command_buffer[1..N]
 *
 *   @return Number of bytes left in commandBuffer to be sent back as response.\n
 *          command_buffer[0]    = result code, BDM_RC_OK => success, else failure error code\n
 *          command_buffer[1..N] = command results
 */
uint8_t command_exec(uint8_t* command_buffer)
{
  uint8_t command_size = command_buffer[0];
  BDMCommands command = command_buffer[1];

  uint8_t size = 1;

  switch((uint8_t)command)
  {
    case CMD_USBDM_GET_CAPABILITIES:
    {
      size = _cmd_usbdm_get_capabilities(command_buffer);
      command_buffer[0] = BDM_RC_OK;
      break;
    }
    default: 
    {
      size=1;
      command_buffer[0] = BDM_RC_FAIL; 
      break;
    }
  }

  return size;
}

//--------------------------------------------------------------------+
// USB COMMAND FUNCTIONS
//--------------------------------------------------------------------+

// Capability vector
static const uint8_t capabilities[] = 
{
	// Inversion is hidden by driver!
	// Note: CAP_HCS08 & CAP_CFV1 are returned inverted for backwards compatibility
  // (Returns 16-bit value)
   (uint8_t)((TARGET_CAPABILITY^(CAP_HCS08|CAP_CFV1))>>8),     // MSB
   (uint8_t)((TARGET_CAPABILITY^(CAP_HCS08|CAP_CFV1))&0xFF),   // LSB

   (uint8_t)(MAX_COMMAND_SIZE>>8),  // MSB
   (uint8_t)MAX_COMMAND_SIZE,       // LSB

   VERSION_MAJOR,             // Extended firmware version number nn.nn.nn
   VERSION_MINOR,
   VERSION_MICRO,
};

//! Returns capability vector for hardware
//! @return
//!  command_buffer                                                \n
//!   - [1..2] = BDM capability, see cmd_proc.h  \n
//!   - [3..4] = Maximum command buffer size  \n
//!
uint8_t _cmd_usbdm_get_capabilities(uint8_t* command_buffer) 
{
   // Copy BDM Options
   (void)memcpy(command_buffer+1, capabilities, sizeof(capabilities));
   uint8_t return_size = sizeof(capabilities) + 1;

   return return_size;
}

