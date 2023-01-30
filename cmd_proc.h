#include "device/usbd.h"


typedef enum {
   // Common to all targets
   CMD_USBDM_GET_COMMAND_RESPONSE  = 0,   //!< Status of last/current command
   CMD_USBDM_SET_TARGET            = 1,   //!< Set target,  @param [2] 8-bit target value @ref TargetType_t
   CMD_USBDM_SET_VDD               = 2,   //!< Set target Vdd (immediate effect)
   CMD_USBDM_DEBUG                 = 3,   //!< Debugging commands (parameter determines actual command) @param [2]  Debug command see \ref DebugSubCommands
   CMD_USBDM_GET_BDM_STATUS        = 4,   //!< Get BDM status\n @return [1] 16-bit status value reflecting BDM status
   CMD_USBDM_GET_CAPABILITIES      = 5,   //!< Get capabilities of BDM, see \ref HardwareCapabilities_t
   CMD_USBDM_SET_OPTIONS           = 6,   //!< Set BDM options, see \ref BDM_Options_t
//   CMD_USBDM_GET_SETTINGS        = 7,   //!< Get BDM setting
   CMD_USBDM_CONTROL_PINS          = 8,   //!< Directly control BDM interface levels
   // Reserved 7..11
   CMD_USBDM_GET_VER               = 12,  //!< Sent to ep0 \n Get firmware version in BCD \n
                                          //!< @return [1] 8-bit HW (major+minor) revision \n [2] 8-bit SW (major+minor) version number
   CMD_GET_VER                     = 12,  //!< Deprecated name - Previous version
   // Reserved 13
   CMD_USBDM_ICP_BOOT              = 14,  //!< Sent to ep0 \n
                                          //!< Requests reboot to ICP mode. @param [2..5] must be "BOOT"
   CMD_SET_BOOT                    = 14,  //!< Deprecated - Previous version

   // Target specific versions
   CMD_USBDM_CONNECT               = 15,  //!< Try to connect to the target
   CMD_USBDM_SET_SPEED             = 16,  //!< Sets-up the BDM interface for a new bit rate & tries
                                          //!    to enable ackn feature, @param [2..3] 16-bit tick count
   CMD_USBDM_GET_SPEED             = 17,  //!< Read speed of the target: @return [1..2] 16-bit tick coun

   CMD_CUSTOM_COMMAND              = 18,  //!< Directly control BDM interface levels
   // Reserved 19

   CMD_USBDM_READ_STATUS_REG       = 20,  //!< Get BDM status
                                          //! @return [1] 8-bit status byte made up as follows: \n
                                          //!    - (HC08/12/RS08/CFV1) bit0   - ACKN, \n
                                          //!    - (All)               bit1   - target was reset (this bit is cleared after reading),  \n
                                          //!    - (CFVx only)         bit2   - current RSTO value \n
                                          //!    - (HC08/12/RS08/CFV1) bit4-3 - comm status: 00=NOT CONNECTED, 01=SYNC, 10=GUESS,  11=USER SUPPLIED \n
                                          //!    - (All)               bit7   - target has power

   CMD_USBDM_WRITE_CONTROL_REG     = 21,  //!< Write to target Control register

   CMD_USBDM_TARGET_RESET          = 22,  //!< Reset target @param [2] \ref TargetMode_t
   CMD_USBDM_TARGET_STEP           = 23,  //!< Perform single step
   CMD_USBDM_TARGET_GO             = 24,  //!< Start code execution
   CMD_USBDM_TARGET_HALT           = 25,  //!< Stop the CPU and bring it into background mode

   CMD_USBDM_WRITE_REG             = 26,  //!< Write to target register
   CMD_USBDM_READ_REG              = 27,  //!< Read target register

   CMD_USBDM_WRITE_CREG            = 28,  //!< Write target Core register
   CMD_USBDM_READ_CREG             = 29,  //!< Read from target Core register

   CMD_USBDM_WRITE_DREG            = 30,  //!< Write target Debufg register
   CMD_USBDM_READ_DREG             = 31,  //!< Read from target Debug register

   CMD_USBDM_WRITE_MEM             = 32,  //!< Write to target memory
   CMD_USBDM_READ_MEM              = 33  //!< Read from target memory
} BDMCommands;

//==========================================================================================
// Capabilities of the hardware - used to enable/disable appropriate code in build
// HW_CAPABILITY
//
#define CAP_RST_OUT     (1<<0)   // RESET can be driven/sensed (req. for HC12)
#define CAP_FLASH       (1<<1)   // 12 V Flash programming supply available (req. RS08)
#define CAP_VDDCONTROL  (1<<2)   // Control over target Vdd
#define CAP_VDDSENSE    (1<<3)   // Sensing of target Vdd
#define CAP_CFVx_HW     (1<<4)   // Supports CFVx extensions beyond basic JTAG (TA etc)
#define CAP_BDM         (1<<5)   // Supports 1-wire BDM interface (BKGD I/O)
#define CAP_JTAG_HW     (1<<7)   // Supports JTAG interface (TCK/TDI/TDO/TMS/TRST?)
#define CAP_SWD_HW      (1<<8)   // Supports SWD interface (SWD/SWCLK)
#define CAP_RST_IN      (1<<9)   // RESET can be sensed
#define CAP_CDC         (1<<12)  // Supports CDC USB interface
#define CAP_CORE_REGS   (1<<31)  // Supports reading core regs

//==========================================================================================
// Targets and visible capabilities supported - related to above but not exactly!
// e.g. CAP_HCS12 => CAP_BDM+CAP_RST_OUT
//      CAP_RS08  => CAP_BDM+CAP_FLASH(+CAP_RST_OUT)
//      CAP_HCS08 => CAP_BDM(+CAP_RST_OUT)
//      CAP_CFVx  => CAP_JTAG_HW+CAP_CFVx_HW+CAP_RST_OUT
//      CAP_DSC   => CAP_JTAG_HW+CAP_RST_OUT + s/w routines
//      CAP_JTAG  => CAP_JTAG_HW+CAP_RST_OUT
//      CAP_RST   => CAP_RST_OUT
//
#define CAP_HCS12       (1<<0)      // Supports HCS12 targets
#define CAP_RS08        (1<<1)      // Supports RS08 targets
#define CAP_VDDCONTROL  (1<<2)      // Control over target Vdd
#define CAP_VDDSENSE    (1<<3)      // Sensing of target Vdd
#define CAP_CFVx        (1<<4)		// Supports CFVx
#define CAP_HCS08       (1<<5)		// Supports HCS08 targets - inverted when queried
#define CAP_CFV1        (1<<6)		// Supports CFV1 targets  - inverted when queried
#define CAP_JTAG        (1<<7)		// Supports JTAG targets
#define CAP_DSC         (1<<8)		// Supports DSC targets
#define CAP_ARM_JTAG    (1<<9)      // Supports ARM targets via JTAG
#define CAP_RST         (1<<10)     // Control & sensing of RESET
#define CAP_PST         (1<<11)     // Supports PST signal sensing
#define CAP_CDC         (1<<12)     // Supports CDC Serial over USB interface
#define CAP_ARM_SWD     (1<<13)     // Supports ARM targets via SWD
#define CAP_S12Z        (1<<14)     // Supports HCS12ZVM

#define HW_CAPABILITY       (CAP_RST_OUT|CAP_BDM)
#define TARGET_CAPABILITY   (CAP_HCS08)

#define MAX_COMMAND_SIZE       (254)

#define VERSION_MAJOR 4 
#define VERSION_MINOR 12
#define VERSION_MICRO 1

//--------------------------------------------------------------------+
// IMPLEMENTED FUNCTIONALITIES
//--------------------------------------------------------------------+
uint8_t _cmd_usbdm_get_capabilities(uint8_t* command_buffer);


// Processes all commands received over USB
uint8_t command_exec(uint8_t* command_buffer);
