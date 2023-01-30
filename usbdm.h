//==========================================================================================
// List of commands

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
// Error code

typedef enum  {
 BDM_RC_OK                      = 0,     //!< - No error
 BDM_RC_ILLEGAL_PARAMS          = 1,     //!< - Illegal parameters to command
 BDM_RC_FAIL                    = 2,     //!< - General Fail
 BDM_RC_BUSY                    = 3,     //!< - Busy with last command - try again - don't change
 BDM_RC_ILLEGAL_COMMAND         = 4,     //!< - Illegal (unknown) command (may be in wrong target mode)
 BDM_RC_NO_CONNECTION           = 5,     //!< - No connection to target
 BDM_RC_OVERRUN                 = 6,     //!< - New command before previous command completed
 BDM_RC_CF_ILLEGAL_COMMAND      = 7,     //!< - Coldfire BDM interface did not recognize the command
 //
 BDM_RC_UNKNOWN_TARGET          = 15,    //!< - Target not supported
 BDM_RC_NO_TX_ROUTINE           = 16,    //!< - No Tx routine available at measured BDM communication speed
 BDM_RC_NO_RX_ROUTINE           = 17,    //!< - No Rx routine available at measured BDM communication speed
 BDM_RC_BDM_EN_FAILED           = 18,    //!< - Failed to enable BDM mode in target (warning)
 BDM_RC_RESET_TIMEOUT_FALL      = 19,    //!< - RESET signal failed to fall
 BDM_RC_BKGD_TIMEOUT            = 20,    //!< - BKGD signal failed to rise/fall
 BDM_RC_SYNC_TIMEOUT            = 21,    //!< - No response to SYNC sequence
 BDM_RC_UNKNOWN_SPEED           = 22,    //!< - Communication speed is not known or cannot be determined
 BDM_RC_WRONG_PROGRAMMING_MODE  = 23,    //!< - Attempted Flash programming when in wrong mode (e.g. Vpp off)
 BDM_RC_FLASH_PROGRAMING_BUSY   = 24,    //!< - Busy with last Flash programming command
 BDM_RC_VDD_NOT_REMOVED         = 25,    //!< - Target Vdd failed to fall
 BDM_RC_VDD_NOT_PRESENT         = 26,    //!< - Target Vdd not present/failed to rise
 BDM_RC_VDD_WRONG_MODE          = 27,    //!< - Attempt to cycle target Vdd when not controlled by BDM interface
 BDM_RC_CF_BUS_ERROR            = 28,    //!< - Illegal bus cycle on target (Coldfire)
 BDM_RC_USB_ERROR               = 29,    //!< - Indicates USB transfer failed (returned by driver not BDM)
 BDM_RC_ACK_TIMEOUT             = 30,    //!< - Indicates an expected ACK was missing
 BDM_RC_FAILED_TRIM             = 31,    //!< - Trimming of target clock failed (out of clock range?).
 BDM_RC_FEATURE_NOT_SUPPORTED   = 32,    //!< - Feature not supported by this version of hardware/firmware
 BDM_RC_RESET_TIMEOUT_RISE      = 33,    //!< - RESET signal failed to rise

 // The following are used by additional USBDM code
 BDM_RC_WRONG_BDM_REVISION      = 34,    //!< - BDM Hardware is incompatible with driver/program
 BDM_RC_WRONG_DLL_REVISION      = 35,    //!< - Program is incompatible with DLL
 BDM_RC_NO_USBDM_DEVICE         = 36,    //!< - No usbdm device was located

 BDM_RC_JTAG_UNMATCHED_REPEAT   = 37,    //!< - Unmatched REPEAT-END_REPEAT
 BDM_RC_JTAG_UNMATCHED_RETURN   = 38,    //!< - Unmatched CALL-RETURN
 BDM_RC_JTAG_UNMATCHED_IF       = 39,    //!< - Unmatched IF-END_IF
 BDM_RC_JTAG_STACK_ERROR        = 40,    //!< - Underflow in call/return sequence, unmatched REPEAT etc.
 BDM_RC_JTAG_ILLEGAL_SEQUENCE   = 41,    //!< - Illegal JTAG sequence
 BDM_RC_TARGET_BUSY             = 42,    //!< - Target is busy (executing?)
 BDM_RC_JTAG_TOO_LARGE          = 43,    //!< - Subroutine is too large to cache
 BDM_RC_DEVICE_NOT_OPEN         = 44,    //!< - USBDM Device has not been opened
 BDM_RC_UNKNOWN_DEVICE          = 45,    //!< - Device is not in database
 BDM_RC_DEVICE_DATABASE_ERROR   = 46,    //!< - Device database not found or failed to open/parse

 BDM_RC_ARM_PWR_UP_FAIL         = 47,    //!< - ARM System power failed
 BDM_RC_ARM_ACCESS_ERROR        = 48,    //!< - ARM Access error

 BDM_JTAG_TOO_MANY_DEVICES      = 49,    //!< - JTAG chain is too long (or greater than 1!)

 BDM_RC_SECURED                 = 50,    //!< - ARM Device is secured (& operation failed?)
} USBDM_ErrorCode;

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

//=====================================================================================
// The following lines choose a Hardware configuration
//=====================================================================================
#define H_USBDM                  1  //!< USBDM    - Universal TBDML/OSBDM JB16
#define H_TBDML                  2  //!< TBDML    - Minimal JB16 version (JB16DWE,JB16JDWE)
#define H_TBDMLSwin              3  //!< No longer used
#define H_OSBDM                  4  //!< OSBDM    - Basic OSBDM hardware
#define H_WTBDM                  5  //!< WTBDM08  - Wiztronics BDMS08/12
#define H_OSBDME                 6  //!< OSBDM+E  - OSBDM+Flash supply
#define H_USBDM_JMxxCLD          7  //!< USBDM hardware using 9S08JM16/32/60CLD (44p package)
#define H_USBDM_JMxxCLC          8  //!< USBDM hardware using 9S08JM16CLC (32p package)
#define H_USBSPYDER              9  //!< USBSPYDER - SofTec USBSPYDER08 - not functional
#define H_USBDM_UF32PBE         10  //!< USBDM hardware using MC9S12UF32PBE (64p package)
#define H_USBDM_CF_JS16CWJ      11  //!< USBDM hardware CF/DSC only using MC9S08JS16CWJ (20p SOIC package)
#define H_USBDM_CF_JMxxCLD      12  //!< Combined USBDM/TBLCF using 9S08JM16/32/60CLD (44p package)
#define H_USBDM_JS16CWJ         13  //!< USBDM hardware using MC9S08JS16CWJ (20p SOIC package)
#define H_USBDM_MC56F8006DEMO   14  //!< MC56F8006DEMO Board (Axiom)
#define H_CUSTOM                15  //!< Reserved for USER created custom hardware
#define H_USBDM_CF_SER_JS16CWJ  16  //!< USBDM hardware CF/DSC only using MC9S08JS16CWJ (20p SOIC package) with serial interface
#define H_USBDM_SER_JS16CWJ     17  //!< USBDM hardware using MC9S08JS16CWJ (20p SOIC package) with Serial interface
#define H_USBDM_CF_SER_JMxxCLD  18  //!< Combined USBDM/TBLCF/Serial using 9S08JM16/32/60CLD (44p package)
#define H_USBDM_TWR_KINETIS     19  //!< TWR Kinetis boards
#define H_USBDM_TWR_CFV1        20  //!< TWR Coldfire V1 boards
#define H_USBDM_TWR_HCS08       21  //!< TWR HCS08 boards
#define H_USBDM_TWR_CFVx        22  //!< TWR Coldfire Vx boards
#define H_USBDM_SWD_SER_JS16CWJ 23  //!< USBDM MC9S08JS16CWJ with BDM, SWD & Serial interfaces
#define H_USBDM_SWD_JS16CWJ     24  //!< USBDM MC9S08JS16CWJ with BDM & SWD interfaces
#define H_USBDM_OPENSDA         25  //!< Freescale FRDM-KL25 board (MK20 chip)
#define H_USBDM_MKL25Z          26  //!< Experimental MKL25Z
#define H_USBDM_MK20D5          27  //!< Experimental MK20DX5
#define H_USBDM_TWR_HCS12       28  //!< TWR HCS12 boards

//=====================================================================================
// Command status
//=====================================================================================
typedef enum  {
 CMD_OK=0,
 CMD_WAIT=1,
 CMD_FAIL=2,
} USBDM_cmd_status;


USBDM_cmd_status receive_USB_command(void);
void send_USB_response(uint8_t *buffer, uint8_t byte_count);