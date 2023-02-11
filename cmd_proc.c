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
static USBDM_ErrorCode command_status = BDM_RC_OK;
static uint8_t response_size = 1;

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
    case CMD_USBDM_GET_COMMAND_STATUS:  //0
    {
      break;
    }
    case CMD_USBDM_SET_TARGET:  //1
    {
      command_status = _cmd_usbdm_set_target(command_buffer);
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
    case CMD_USBDM_CONTROL_PINS:  //8
    {
      // Not implemented
      command_status = _cmd_usbdm_control_pins(command_buffer);
      break;
    }
    case CMD_USBDM_CONNECT:  //15
    {
      command_status = _cmd_usbdm_connect();
      break;
    }
    case CMD_USBDM_SET_SPEED:  //16
    {
      command_status = _cmd_usbdm_set_speed(command_buffer);
      break;
    }
    case CMD_USBDM_GET_SPEED:  //17
    {
      command_status = _cmd_usbdm_get_speed(command_buffer);
      break;
    }
    case CMD_USBDM_READ_STATUS_REG:  //20
    {
      command_status = _cmd_usbdm_read_status_reg(command_buffer);
      break;
    }
    case CMD_USBDM_WRITE_CONTROL_REG:  //21
    {
      command_status = _cmd_usbdm_write_control_reg(command_buffer);
      break;
    }
    case CMD_USBDM_TARGET_RESET:  //22
    {
      command_status = _cmd_usbdm_reset(command_buffer);
      break;
    }
    case CMD_USBDM_TARGET_STEP:  //23
    {
      command_status = _cmd_usbdm_step(command_buffer);
      break;
    }
    case CMD_USBDM_TARGET_GO:  //24
    {
      command_status = _cmd_usbdm_go(command_buffer);
      break;
    }
    case CMD_USBDM_TARGET_HALT:  //25
    {
      command_status = _cmd_usbdm_halt(command_buffer);
      break;
    }
    case CMD_USBDM_WRITE_REG:  //26
    {
      command_status = _cmd_usbdm_write_reg(command_buffer);
      break;
    }
    case CMD_USBDM_READ_REG:  //27
    {
      command_status = _cmd_usbdm_read_reg(command_buffer);
      break;
    }
    case CMD_USBDM_WRITE_DREG:  //30
    {
      command_status = _cmd_usbdm_write_bkpt(command_buffer);
      break;
    }
    case CMD_USBDM_READ_DREG:  //31
    {
      command_status = _cmd_usbdm_read_bkpt(command_buffer);
      break;
    }
    case CMD_USBDM_WRITE_MEM:  //32
    {
      command_status = _cmd_usbdm_write_mem(command_buffer);
      break;
    }
    case CMD_USBDM_READ_MEM:  //33
    {
      command_status = _cmd_usbdm_read_mem(command_buffer);
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
//!   In our implementation, only one target is available.
//!
uint8_t _cmd_usbdm_set_target(uint8_t* command_buffer)
{
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


//! Directly control pins
//!
//! @note
//!   commandBuffer\n
//!     Entry: [2..3] = control value\n
//!     Exit:  [1..2] = pin values (MSB unused) - not yet implemented
//!
uint8_t _cmd_usbdm_control_pins(uint8_t* command_buffer) 
{
  response_size = 3;
  command_buffer[1] = 0;
  command_buffer[2] = 0;

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
  bdm_cmd_sync();

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

//! HCS12,HCS08,RS08 & CFV1 -  Read current speed
//!
//! @return
//!    == \ref BDM_RC_OK => success                \n
//!    != \ref BDM_RC_OK => error                  \n
//!                                                \n
//!  command_buffer                                 \n
//!   - [1..2] => 16-bit Sync value in 60MHz ticks
//!
uint8_t _cmd_usbdm_get_speed(uint8_t* command_buffer) 
{
  uint16_t sync_length = bdm_cmd_get_sync_length();
  command_buffer[1] = (uint8_t)(sync_length>>8);
  command_buffer[2] = (uint8_t)(sync_length&0xFF);

  response_size = 3;
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

  // Save status on command_buffer[4]
  bdm_cmd_read_status(command_buffer);

  return BDM_RC_OK;
}

//! HCS12/HCS08/RS08/CFV1 -  Write Target BDM Control Register
//!
//! @note
//!  command_buffer                                          \n
//!   - [2..5] => 8-bit control register value [MSBs ignored]
//!
//! @return
//!    == \ref BDM_RC_OK => success       \n
//!    != \ref BDM_RC_OK => error         \n
//!
uint8_t _cmd_usbdm_write_control_reg(uint8_t* command_buffer)
{
  bdm_cmd_write_control(command_buffer);

  return BDM_RC_OK;
}

//! HCS12/HCS08/RS08/CFV1 -  Reset Target
//!
//! @note
//!  commandBuffer                                          \n
//!   - [2] => 8-bit reset control [see \ref TargetMode_t]
//!
//! @return
//!    == \ref BDM_RC_OK => success       \n
//!    != \ref BDM_RC_OK => error         \n
//!
uint8_t _cmd_usbdm_reset(uint8_t* command_buffer)
{
  switch (command_buffer[2] & RESET_TYPE_MASK)
  {
  case RESET_HARDWARE:
  {
    return BDM_RC_FEATURE_NOT_SUPPORTED;
  }
  case RESET_POWER:
  {
    return BDM_RC_FEATURE_NOT_SUPPORTED;
  }
  case RESET_SOFTWARE:
  {
    // Soft reset HCS08
    bdm_cmd_reset();
    break;
  }
  default:
  {
    return BDM_RC_ILLEGAL_PARAMS;
  }
  }

  return BDM_RC_OK;
}

uint8_t _cmd_usbdm_step(uint8_t* command_buffer)
{
  bdm_cmd_trace();
  return BDM_RC_OK;
}

uint8_t _cmd_usbdm_go(uint8_t* command_buffer)
{
  bdm_cmd_go();
  return BDM_RC_OK;
}

uint8_t _cmd_usbdm_halt(uint8_t* command_buffer)
{
  bdm_cmd_halt();
  return BDM_RC_OK;
}


//! RS08/HCS08 Write core register
//!
//! @note
//!  command_buffer                                         \n
//!  - [2..3] => 16-bit register number [MSB ignored]      \n
//!  - [4..7] => 32-bit register value  [some MSBs ignored]
//!
//! @return
//!    == \ref BDM_RC_OK => success       \n
//!    != \ref BDM_RC_OK => error         \n
//!
uint8_t _cmd_usbdm_write_reg(uint8_t* command_buffer)
{

  switch (command_buffer[3]) {
    case HCS08_RegPC :
        // 16 bit register
        bdm_cmd_write_pc(command_buffer);
        break;
    case HCS08_RegHX  :
        // 16 bit register
        bdm_cmd_write_hx(command_buffer);
        break;
    case HCS08_RegSP :
        // 16 bit register
        bdm_cmd_write_sp(command_buffer);
        break;
    case HCS08_RegA  :
        // 8 bit register
        bdm_cmd_write_a(command_buffer);
        break;
    case HCS08_RegCCR :
        // 8 bit register
        bdm_cmd_write_ccr(command_buffer);
        break;
    default:
        return BDM_RC_ILLEGAL_PARAMS;
  }

  return BDM_RC_OK;
}


//! RS08/HCS08 Read core register
//!
//! @note
//!  command_buffer                                         \n
//!  - [2..3] => 16-bit register number [MSB ignored]      \n
//!
//! @return
//!    == \ref BDM_RC_OK => success                         \n
//!    != \ref BDM_RC_OK => error                           \n
//!                                                         \n
//!  command_buffer                                          \n
//!  - [1..4] => 32-bit register value  [some MSBs ignored]
//!
uint8_t _cmd_usbdm_read_reg(uint8_t* command_buffer)
{
  command_buffer[1] = 0;
  command_buffer[2] = 0;

  switch (command_buffer[3]) {
    case HCS08_RegPC :
        // 16 bit register
        bdm_cmd_read_pc(command_buffer);
        break;
    case HCS08_RegHX  :
        // 16 bit register
        bdm_cmd_read_hx(command_buffer);
        break;
    case HCS08_RegSP :
        // 16 bit register
        bdm_cmd_read_sp(command_buffer);
        break;
    case HCS08_RegA  :
        // 8 bit register
        bdm_cmd_read_a(command_buffer);
        break;
    case HCS08_RegCCR :
        // 8 bit register
        bdm_cmd_read_ccr(command_buffer);
        break;
    default:
        return BDM_RC_ILLEGAL_PARAMS;
  }

  response_size = 5;

  return BDM_RC_OK;
}

//! HCS08/RS08 Write to Breakpoint reg
//!
//! @note
//!  commandBuffer                                    \n
//!  - [2..3] => 16-bit register number [ignored]     \n
//!  - [4..7] => 32-bit register value  [MSBs ignored]
//!
//! @return
//!    == \ref BDM_RC_OK => success        \n
//!    != \ref BDM_RC_OK => error
//!
uint8_t _cmd_usbdm_write_bkpt(uint8_t* command_buffer)
{
  uint8_t addr_h = command_buffer[6];
  uint8_t addr_l = command_buffer[7];

  bdm_cmd_write_bkpt(addr_h, addr_l);

  return BDM_RC_OK;
}


//! HCS08/RS08 Read from Breakpoint reg
//!
//! @note
//!  commandBuffer                                    \n
//!  - [2..3] => 16-bit register number [ignored]     \n
//!  - [1..4] => 32-bit register value  [MSBs zeroed]
//!
//! @return
//!    == \ref BDM_RC_OK => success                         \n
//!    != \ref BDM_RC_OK => error                           \n
//!                                                         \n
//!  commandBuffer                                          \n
//!  - [1..4] => 32-bit register value  [some MSBs ignored]
//!
uint8_t _cmd_usbdm_read_bkpt(uint8_t* command_buffer)
{
  command_buffer[1] = 0;
  command_buffer[2] = 0;

  // Save 16 bit reg in command_buffer
  bdm_cmd_read_bkpt(command_buffer+2);

  return BDM_RC_OK;
}


//! HCS08/RS08 -  Write block of bytes to memory
//!
//! @note
//!  command_buffer                           \n
//!  - [2]    = element size/mode            \n
//!  - [3]    = # of bytes                   \n
//!  - [4..7] = address [MSB ignored]        \n
//!  - [8..N] = data to write
//!
//! @return
//!    == \ref BDM_RC_OK => success       \n
//!    != \ref BDM_RC_OK => error         \n
//!
uint8_t _cmd_usbdm_write_mem(uint8_t* command_buffer) 
{
  uint8_t mode        = command_buffer[2];
  uint8_t  count      = command_buffer[3];
  uint8_t addr_h      = command_buffer[6];
  uint8_t addr_l      = command_buffer[7];
  uint16_t addr       = (uint16_t)((addr_h<<8) | addr_l);
  uint8_t *data_ptr   = command_buffer+8;

  if (mode & MS_FAST)
  {
    // Not supported (yet)
    return BDM_RC_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    while (count > 0)
    {
      bdm_cmd_write_byte((uint8_t)(addr>>8), (uint8_t)(addr&0xFF), *data_ptr);

      count--;        // decrement count of bytes
      data_ptr++;     // increment buffer pointer
      addr++;         // increment memory address
    }
  }

  return BDM_RC_OK;
}

//! HCS08/RS08 -  Read block of data from memory
//!
//! @note
//!  command_buffer                       \n
//!  - [2]    = element size/mode        \n
//!  - [3]    = # of bytes               \n
//!  - [4..7] = address [MSB ignored]
//!
//! @return
//!    == \ref BDM_RC_OK => success      \n
//!    != \ref BDM_RC_OK => error        \n
//!                                      \n
//!  commandBuffer                       \n
//!  - [1..N]  = data read
//!
uint8_t _cmd_usbdm_read_mem(uint8_t* command_buffer) 
{
  uint8_t mode = command_buffer[2];
  uint8_t  count      = command_buffer[3];
  uint8_t addr_h      = command_buffer[6];
  uint8_t addr_l      = command_buffer[7];
  uint16_t addr       = (uint16_t)((addr_h<<8) | addr_l);
  uint8_t  *data_ptr  = command_buffer+1;

  if (count > MAX_COMMAND_SIZE-1)
  {
    return BDM_RC_ILLEGAL_PARAMS; // requested block + status is too long to fit into the buffer
  }

  response_size = count + 1;

  if (mode & MS_FAST)
  {
    // Not supported (yet)
    return BDM_RC_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    while (count > 0)
    {
      bdm_cmd_read_byte((uint8_t)(addr>>8), (uint8_t)(addr&0xFF), data_ptr);

      count--;        // decrement count of bytes
      data_ptr++;     // increment buffer pointer
      addr++;         // increment memory address
    }
  }

  return BDM_RC_OK;
}