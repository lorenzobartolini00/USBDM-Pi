#define CMD_USBDM_GET_VER 12

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

#define HW_JB        0x00
#define HW_JM        0x80
#define HW_UF        0xC0

#define H_USBDM                 1  //!< USBDM    - Universal TBDML/OSBDM JB16
#define H_USBDM_JMxxCLD          7  //!< USBDM hardware using 9S08JM16/32/60CLD (44p package)
#define TARGET_HARDWARE H_USBDM_JMxxCLD

#define VERSION_HW  (HW_JM+TARGET_HARDWARE)

#define ICP_VERSION_SW_LOCATION (0xF800)
#define ICP_VERSION_HW_LOCATION (ICP_VERSION_SW_LOCATION+1)

//! ICP version (2 hex digits, major.minor)
#define ICP_VERSION_SW (1<<4|2) // 1.2
 
const uint8_t ICP_Version_SW = ICP_VERSION_SW; 
const uint8_t ICP_Version_HW = VERSION_HW;     