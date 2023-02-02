#include "cmd_proc.h"

#include "config.h"
#include "BDM_options.h"

#include "bdm.h"

//! Options for the BDM
//!
//! see \ref BDM_Option_t
BDM_Option_t bdm_option = {
 /* cycleVddOnReset    */   false,               //!< Cycle target Power when resetting
 /* cycleVddOnConnect  */   false,               //!< Cycle target Power if connection problems (when resetting?)
 /* leaveTargetPowered */   false,               //!< Leave target power on when exiting
 /* guessSpeed         */   true,                //!< Guess speed for target w/o ACKN
 /* useResetSignal     */   false,               //!< Use RESET signal on BDM interface
 /* targetVdd          */   BDM_TARGET_VDD_OFF,  //!< Target Vdd (off, 3.3V or 5V)
 /* useAltBDMClock     */   CS_DEFAULT,          //!< Use alternative BDM clock source in target
 /* autoReconnect      */   AUTOCONNECT_STATUS,  //!< Automatically re-connect to target (for speed change)
 /* SBDFRaddress       */   HCS08_SBDFR_DEFAULT, //!< Default HCS08_SBDFR address
 /* reserved           */   {0}                  //   Reserved
};

//! Status of the BDM
//!
static CableStatus_t cable_status =  {
   T_HCS08,             // target_type
   WAIT,              // ackn
   NO_RESET_ACTIVITY, // reset
   SPEED_SYNC,     // speed
   BDM_TARGET_VDD_EXT,                 // power
   0,                 // wait150_cnt
   0,                 // wait64_cnt
   0,                 // sync_length
   0,                 // bdmpprValue
};

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
   // 12(Illegal):  CMD_USBD M_GET_VER (EP0)
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
USBDM_ErrorCode command_status = BDM_RC_OK;
uint8_t response_size = 1;

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

  // Default response size (maybe will be changed inside a function)
  response_size = 1;

  switch((uint8_t)command)
  {
    case CMD_USBDM_SET_TARGET:  //1
    {
      command_status = _cmd_usbdm_set_target();
      break;
    }
    case CMD_USBDM_SET_VDD:  //2
    {
      command_status = _cmd_usbdm_set_vdd(command_buffer);
      break;
    }
    case CMD_USBDM_GET_BDM_STATUS:  //4
    {
      command_status = _cmd_usbdm_get_bdm_status(command_buffer);
      break;
    }
    case CMD_USBDM_GET_CAPABILITIES:  //5
    {
      command_status = _cmd_usbdm_get_capabilities(command_buffer);
      break;
    }
    case CMD_USBDM_SET_OPTIONS:  //6
    {
      command_status = _cmd_usbdm_set_options(command_buffer);
      break;
    }
    case CMD_USBDM_CONNECT:  //15
    {
      command_status = BDM_RC_OK;
      break;
    }
    case CMD_USBDM_SET_SPEED:  //16
    {
      command_status = _cmd_usbdm_set_speed(command_buffer);
      break;
    }
    case CMD_USBDM_READ_STATUS_REG:  //20
    {
      command_status = _cmd_usbdm_read_status_reg(command_buffer);
      break;
    }
    default: 
    {
      command_status = BDM_RC_FAIL; 
      break;
    }
  }

  // Save command status in buffer
  command_buffer[0] = command_status;

  return response_size;
}

void set_command_status(uint8_t status)
{
  command_status = (USBDM_ErrorCode)status;
}

//--------------------------------------------------------------------+
// USB COMMAND FUNCTIONS
//--------------------------------------------------------------------+

//! Set target type
//! Initialise interface for given target
//! @note
//!   command_buffer        \n
//!   - [2] = target type
//!  @note
//!   In our implementation, only one target is available. This function will initialise BDM interface.
//!
uint8_t _cmd_usbdm_set_target(uint8_t* command_buffer)
{
  // Init BDM
  // ...

  return BDM_RC_OK;
}

//! Directly control Target Vdd
//!
//! @note
//!   command_buffer\n
//!     Entry: [2..3] = control value (MSB unused)\n
//!     Exit:  none
//!
uint8_t _cmd_usbdm_set_vdd(uint8_t* command_buffer)
{
  // Save the request in bdm_option
  bdm_option.targetVdd = command_buffer[3];

  // Vdd  is not actually set. Our target is self powered.

  return BDM_RC_OK;
}

uint8_t _cmd_usbdm_get_bdm_status(uint8_t* command_buffer)
{
  uint16_t status = 0;
  switch (cable_status.speed) 
  {
//  case SPEED_NO_INFO       : status |= S_NOT_CONNECTED;  break; 
    case SPEED_USER_SUPPLIED : status |= S_USER_DONE;      break; 
    case SPEED_SYNC          : status |= S_SYNC_DONE;      break; 
    case SPEED_GUESSED       : status |= S_GUESS_DONE;     break; 
  }
  // Assume power present
  status |= S_POWER_EXT;

  command_buffer[1] = (uint8_t) (status>>8);
  command_buffer[2] = (uint8_t) status;
  response_size = 3;

  return BDM_RC_OK;
}

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
   // Copy BDM capabilities
   (void)memcpy(command_buffer+1, capabilities, sizeof(capabilities));
   response_size = sizeof(capabilities) + 1;

   return BDM_RC_OK;
}

//! Set various options
//!
//! @note
//!   command_buffer\n
//!   - [2..N] = options (image of \ref BDM_Option_t)
//!
//!  @return
//!    BDM_RC_OK => success
//!
uint8_t _cmd_usbdm_set_options(uint8_t* command_buffer) 
{
   // Copy BDM Options
   (void)memcpy((uint8_t*)&bdm_option, command_buffer+2, sizeof(bdm_option));
   return BDM_RC_OK;
}

//! HCS12/HCS08/RS08/CFV1 - Try to connect to the target
//!
//! @return
//!    == \ref BDM_RC_OK => success   
//!
uint8_t _cmd_usbdm_connect(void)
{
  // Since we have no control over target's power supply, we can't connect it via software. 
  // To connect the target, press pico's board button and cycle target power supply(turn off->turn on)

  return BDM_RC_OK;
}

//! HCS12/HCS08/RS08/CFV1 -  Set comm speed to user supplied value
//!
//! @note
//!  command_buffer                                 \n
//!  - [1..2] = 16-bit Sync value in 60MHz ticks
//!
//! @return
//!    == \ref BDM_RC_OK => success        \n
//!    != \ref BDM_RC_OK => error
//!
uint8_t _cmd_usbdm_set_speed(uint8_t* command_buffer)
{
  uint16_t sync_value = *(uint16_t*)(command_buffer+2); // Save the new speed
  cable_status.sync_length = sync_value;
  cable_status.speed       = SPEED_USER_SUPPLIED; // User told us (even if it doesn't work!)

  return BDM_RC_OK;
}

//! HCS12/HCS08/RS08/CFV1 -  Read Target BDM Status Register
//!
//! @return
//!    == \ref BDM_RC_OK => success       \n
//!    != \ref BDM_RC_OK => error         \n
//!                                       \n
//!  command_buffer                        \n
//!  - [1..4] => 8-bit Status register [MSBs are zero]
//!
uint8_t _cmd_usbdm_read_status_reg(uint8_t* command_buffer)
{
  response_size = 5;
  command_buffer[1] = 0;
  command_buffer[2] = 0;
  command_buffer[3] = 0;

  // Return data_buffer, which contains byte status
  uint8_t* data_buffer = bdm_cmd_read_status();
  command_buffer[4] = data_buffer[3];

  return BDM_RC_OK;
}
